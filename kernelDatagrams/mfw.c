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
#include <linux/netfilter_ipv4.h>
#include <linux/types.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>

#include "mfw.h"

MODULE_DESCRIPTION("MFW: a Minimal FireWall.");
MODULE_AUTHOR("Sergio Castillo <scastillo@deic.uab.es>");
MODULE_LICENSE("GPL");


/* -------------------------------------------------------------------------- */

static struct nf_hook_ops nfho;	/* Struct. to register a hook funct.  */
states status = ON;		        /* Filtering status ON/OFF            */
rule rules_set[MAX_RULES];	    /* Set of rules                       */

/* -------------------------------------------------------------------------- */

int numberOfRules = 0;
extern int TCPDrop, TCPTotal;
extern int UDPDrop, UDPTotal;
extern int ICMPDrop, ICMPTotal;

unsigned int hook_func(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn) (struct sk_buff *))
{
    int i = 0;
	
	if((*skb)->nh.iph->protocol == IPPROTO_TCP) {
		TCPTotal++;
	} else if((*skb)->nh.iph->protocol == IPPROTO_UDP) {
		UDPTotal++;
	} else {
		ICMPTotal++;
	}
        
    for(i = 0; i < numberOfRules && status == ON; i++) {
        if((*skb)->nh.iph->protocol == rules_set[i].protocol && (*skb)->nh.iph->daddr == rules_set[i].daddr) {
            if(rules_set[i].protocol == IPPROTO_TCP && (*skb)->h.th->dest == rules_set[i].port) {
				TCPDrop++;
				printk(KERN_INFO "Dropeando packete TCP)\n");
                return(NF_DROP);
            } else if(rules_set[i].protocol == IPPROTO_UDP && (*skb)->h.uh->dest == rules_set[i].port) {
				UDPDrop++;
				printk(KERN_INFO "Dropeando packete UDP)\n");
                return(NF_DROP);
            } else if(rules_set[i].protocol == IPPROTO_ICMP) {
				ICMPDrop++;
				printk(KERN_INFO "Dropeando packete ICMP)\n");
                return(NF_DROP);
			}
        }
    }

	return NF_ACCEPT;
}


/* -------------------------------------------------------------------------- */

int init_nf_hook(void)
{
	nfho.hook     = hook_func;	     /* Hook function for filter rules    */
	nfho.hooknum  = NF_IP_LOCAL_OUT; /* Hook point                        */
	nfho.pf       = PF_INET;	     /* INET Protocol Family              */
	nfho.priority = NF_IP_PRI_FIRST; /* Make our function the first       */

	return nf_register_hook(&nfho);
}


/* -------------------------------------------------------------------------- */

int init_mod(void)
{
	int res;

	res = init_procfs_dir(PROCFS_DIR);
	if (res != SUCCESS) {
		printk(KERN_ERR "mfw: problems creating the dir procfs entry\n");
		return res;
	}

	res = init_procfs_status(PROCFS_FILE_STATUS);
	if (res != SUCCESS) {
		printk(KERN_ERR "mfw: problems creating the procfs status entry\n");
		return res;
	}

	res = init_procfs_rules(PROCFS_FILE_RULES);
	if (res != SUCCESS) {
		printk(KERN_ERR "mfw: problems creating the procfs rules entry\n");
		return res;
	}

	res = init_nf_hook();
	if (res != SUCCESS) {
		printk(KERN_ERR "mfw: problems registering the NF hook\n");
		return res;
	}

	printk(KERN_NOTICE "mfw: Loaded successfully!\n");
	return 0;
}

void exit_mod(void)
{
	cleanup_procfs_file(PROCFS_FILE_STATUS);
	cleanup_procfs_file(PROCFS_FILE_RULES);
	cleanup_procfs_dir(PROCFS_DIR);
	nf_unregister_hook(&nfho);

	printk(KERN_NOTICE "mfw: Unloaded successfully!\n");
}

module_init(init_mod);
module_exit(exit_mod);

/* -------------------------------------------------------------------------- */
