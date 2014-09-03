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

#ifndef _MFW_H_
#define _MFW_H_

/* -------------------------------------------------------------------------- */

#define PROCFS_DIR		    "mfw"
#define PROCFS_FILE_STATUS	"status"
#define PROCFS_FILE_RULES	"rules"
#define MAX_RULES		    30
#define SUCCESS			    0
#define FAILS			    -1

typedef struct {
    __u8	protocol;
    __be32	daddr;
    __be16	port;
} rule;

typedef enum {
    ON,
    OFF
} states;


extern int	init_procfs_dir(char *);
extern int	init_procfs_status(char *);
extern int	init_procfs_rules( char *);
extern void	cleanup_procfs_file(char *);
extern void	cleanup_procfs_dir(char *);

/* -------------------------------------------------------------------------- */

#endif
