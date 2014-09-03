/*
   Copyright (c) 2007 Sergio Castillo Pérez
   Written & tested using Linux Kernel v.2.6.20.1

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* -------------------------------------------------------------------------- */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/in.h>
#include <linux/string.h>
#include <linux/inet.h>

#include "mfw.h"
#include "procfs.h"

/* -------------------------------------------------------------------------- */

extern states status;
extern rule rules_set[MAX_RULES];

static struct
proc_dir_entry *procfs_dir,	/* Directory in /proc that we will use */
*procfs_file_status,		/* File in procfs to change the status */
*procfs_file_rules;		    /* File in procfs to add rules         */

extern int numberOfRules;
int TCPDrop  = 0, TCPTotal = 0;
int UDPDrop  = 0, UDPTotal  = 0;
int ICMPDrop = 0, ICMPTotal = 0;

/* -------------------------------------------------------------------------- */

int init_procfs_dir(char *dirname)
{
	/* Create the directory "dirname" in proc filesystem */
	procfs_dir = proc_mkdir(dirname, NULL);
	if (procfs_dir == NULL) return -ENOMEM;

	procfs_dir->owner = THIS_MODULE;
	return 0;
}

/* -------------------------------------------------------------------------- */

int init_procfs_status(char *filename)
{
	/* Create the file "filename" in proc filesystem */
	procfs_file_status = create_proc_entry(filename, RW_PERM, procfs_dir);
	if (procfs_file_status == NULL) return -ENOMEM;

	/* Assign the read/write functions associated to the "filename" */
	procfs_file_status->read_proc = read_proc_status;
	procfs_file_status->write_proc = write_proc_status;

	return 0;
}

int init_procfs_rules(char *filename)
{
	/* Create the file "filename" in proc filesystem */
	procfs_file_rules = create_proc_entry(filename, RW_PERM, procfs_dir);
	if (procfs_file_rules == NULL) return -ENOMEM;

	/* Assign the read/write functions associated to the "filename" */
	procfs_file_rules->read_proc = read_proc_rules;
	procfs_file_rules->write_proc = write_proc_rules;

	return 0;
}


/* -------------------------------------------------------------------------- */

void cleanup_procfs_file(char *filename)
{
	remove_proc_entry(filename, procfs_dir);
}

void cleanup_procfs_dir(char *dirname)
{
	remove_proc_entry(dirname, NULL);
}


/* -------------------------------------------------------------------------- */

int read_proc_status(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	if (status == ON) strcpy(page, "1\n");
	else strcpy(page, "0\n");

	/* Return the lenght of the string "0\n" OR the string "1\n" */
	return 2;
}


int read_proc_rules(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int i = 0;
	char bf[50] = {0};
	char buffer[1000] = {0};
	
    for(i = 0; i < numberOfRules; i++) {
        char protocol = (rules_set[i].protocol == IPPROTO_TCP)? 'T' : (rules_set[i].protocol == IPPROTO_UDP)? 'U' : 'I';
        sprintf(bf, "%c %u.%u.%u.%u %d\n", protocol, NIPQUAD(rules_set[i].daddr), ntohs(rules_set[i].port));
		strcat(buffer, bf);
    }
	
	sprintf(bf, "TCPTotal:%d \t TCPDrop: %d\n", TCPTotal, TCPDrop);
	strcat(buffer, bf);
		
	sprintf(bf, "UDPTotal:%d \t UDPDrop: %d\n", UDPTotal, UDPDrop);
	strcat(buffer, bf);
		
	sprintf(bf, "ICMPTotal:%d \t ICMPDrop: %d\n", ICMPTotal, ICMPDrop);
	strcat(buffer, bf);
		
	strcpy(page, buffer);
	printk(KERN_INFO "Mostrando las reglas (read_proc_status(...))\n");
	
	return strlen(page);
}


/* -------------------------------------------------------------------------- */

int write_proc_status(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char new_status[1];

	try_module_get(THIS_MODULE);
	if (copy_from_user(new_status, buffer, 1)) {
		module_put(THIS_MODULE);
		return -EFAULT;
	}
	
	module_put(THIS_MODULE);

	switch (new_status[0]) {
	case '0':
		status = OFF;	    /* '0' means OFF         */
		printk(KERN_INFO "Estado cambiado a OFF\n");
		break;
	case '1':
		status = ON;	    /* '1' means ON          */
		printk(KERN_INFO "Estado cambiado a ON\n");
		break;
	case '2':
	    numberOfRules = 0;  /* '2' mens RESET roles list */
	    printk(KERN_INFO "Reiniciada la lista de reglas\n");
	    break;
	}

	/* Return the number of bytes read */
	return 1;
}


int write_proc_rules(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char write_buff[1000] = {0};
	char ipv4_protocol = 0;
	char ipv4_ip[16] = {0};
	int ipv4_port = 0;
	
	try_module_get(THIS_MODULE);
	if (copy_from_user(write_buff, buffer, 1000)) {	
		module_put(THIS_MODULE);
		return -EFAULT;
	}
	
	sscanf(write_buff, "%c %s %d", &ipv4_protocol, ipv4_ip, &ipv4_port);
		
	if((ipv4_protocol == 'T' || ipv4_protocol == 'U' || ipv4_protocol == 'I') && numberOfRules < MAX_RULES) {
	    rules_set[numberOfRules].port      = htons(ipv4_port);
	    rules_set[numberOfRules].daddr     = in_aton(ipv4_ip);
	    rules_set[numberOfRules].protocol  = (ipv4_protocol == 'T')? IPPROTO_TCP : (ipv4_protocol == 'U')? IPPROTO_UDP : IPPROTO_ICMP;
	    numberOfRules++;
	    printk(KERN_INFO "Nueva regla añadida: %c %s %d\n", ipv4_protocol, ipv4_ip, ipv4_port);
	} else {
	    printk(KERN_INFO "Error al añadir nuva regla o tamaño maximo de reglas alcanzado!\n");
	}
	
	module_put(THIS_MODULE);
	return strlen(write_buff);
}

/* -------------------------------------------------------------------------- */
