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

#ifndef _PROCFS_H_
#define _PROCFS_H_

/* -------------------------------------------------------------------------- */

#define RW_PERM		0600

int init_procfs_dir(char *);
int init_procfs_status(char *);
int init_procfs_rules(char *);
void cleanup_procfs_file(char *);
void cleanup_procfs_dir(char *);
int read_proc_status(char *, char **, off_t, int, int *, void *);
int write_proc_status(struct file *, const char *, unsigned long, void *);
int read_proc_rules(char *, char **, off_t, int, int *, void *);
int write_proc_rules(struct file *, const char *, unsigned long, void *);

#define NIPQUAD(addr) \
((unsigned char *)&addr)[0], \
((unsigned char *)&addr)[1], \
((unsigned char *)&addr)[2], \
((unsigned char *)&addr)[3]
#define NIPQUAD_FMT "%u.%u.%u.%u" 

/* -------------------------------------------------------------------------- */

#endif
