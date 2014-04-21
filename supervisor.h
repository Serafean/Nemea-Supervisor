/**
 * \file supervisor.h
 * \brief Structures and functions for supervisor.
 * \author Marek Svepes <svepemar@fit.cvut.cz>
 * \date 2013
 */
/*
 * Copyright (C) 2013 CESNET
 *
 * LICENSE TERMS
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of the Company nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * ALTERNATIVELY, provided that this notice is retained in full, this
 * product may be distributed under the terms of the GNU General Public
 * License (GPL) version 2 or later, in which case the provisions
 * of the GPL apply INSTEAD OF those given above.
 *
 * This software is provided ``as is'', and any express or implied
 * warranties, including, but not limited to, the implied warranties of
 * merchantability and fitness for a particular purpose are disclaimed.
 * In no event shall the company or contributors be liable for any
 * direct, indirect, incidental, special, exemplary, or consequential
 * damages (including, but not limited to, procurement of substitute
 * goods or services; loss of use, data, or profits; or business
 * interruption) however caused and on any theory of liability, whether
 * in contract, strict liability, or tort (including negligence or
 * otherwise) arising in any way out of the use of this software, even
 * if advised of the possibility of such damage.
 *
 */

#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <limits.h>

#ifndef PERM_LOGSDIR
#define PERM_LOGSDIR 	(S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH) ///< Permissions of directory with stdout and stderr logs of modules
#endif

#ifndef PERM_LOGFILE
#define PERM_LOGFILE 	(S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH) ///< Permissions of files with stdout and stderr logs of module
#endif

#define TRUE 								1 ///< Bool true
#define FALSE 								0 ///< Bool false
#define RUNNING_MODULES_ARRAY_START_SIZE 	10
#define IFCES_ARRAY_START_SIZE				5
#define MODULES_ARRAY_START_SIZE 			10

/***********STRUCTURES***********/

/** Structure with information about one interface of module */
typedef struct interface {
	char *	ifc_note; ///< Interface ID
	char *	ifc_type; ///< Interface type (TCP ~ t, UNIXSOCKET ~ u)
	char * 	ifc_params; ///< Interface parameters (IN interface ~ address,port; OUT interface ~ port,number of connections)
	char *	ifc_direction; ///< Interface direction (IN, OUT)
} interface_t;

typedef struct remote_info_s {
	int 	command_num;
	int 	size_of_recv;
} remote_info_t;

/** Structure with information about one module in configuration */
// typedef struct module_atr {
// 	int 			module_running;
// 	char *			module_path; ///< Path to module from current directory
// 	char *			module_name; ///< Name of module (flowcounter...)
// 	char *			module_params; ///< Module parameter (Input file...)
// 	int 			module_ifces_array_size;
// 	int 			module_ifces_cnt;
// 	interface_t	* 	module_ifces; ///< Array of "interface" structures with information about every interface of module
// } module_atr_t;

/** Structure with information about one running module */
typedef struct running_module {
	int 			module_cloned;
	int 			module_served_by_service_thread;
	int 			module_ifces_array_size;
	int 			module_running;
	char *			module_params; ///< Module parameter (Input file...)
	int 			module_enabled;
	int 			module_ifces_cnt;
	int 			module_num_out_ifc;
	int 			module_num_in_ifc;
	int *			module_counters_array;
	int 			module_service_ifc_isconnected; ///< if supervisor is connected to module ~ 1, else ~ 0
	int 			module_has_service_ifc; ///< if module has service interface ~ 1, else ~ 0
	int 			module_service_sd; ///< socket descriptor of the service connection
	char *			module_name; ///< Module name (flowcounter...)
	int 			module_status; ///< Module status (TRUE ~ ok, FALSE ~ not ok)
	int				module_restart_cnt; ///< Number of module restarts (after MAX_RESTARTS supervisor will not try to restart broken module again)
	pid_t			module_pid; ///< Module PID
	char *			module_path; ///< Path to module from current directory
	interface_t *	module_ifces; ///< Array of "interface" structures with information about every interface of module
	int 			last_cpu_usage_kernel_mode;
	int 			last_cpu_usage_user_mode;
	int 			percent_cpu_usage_kernel_mode;
	int 			percent_cpu_usage_user_mode;
	int 			num_periods_overload;
	int 			remote_module;
	int 			module_number;
} running_module_t;


/***********FUNCTIONS***********/

/** Opens the xml config_file, parses it and saves information about modules to structures "module_atr" 
 * @param[in] config_file Name of xml config file
 */
int load_configuration(const int choice, const char * config_file);



/** Prints current loaded configuration - modules and all information about them */
void print_configuration ();



/** Connects types and parameters of all interfaces of module to format for libtrap plus name of module and "-i"
 * It is used after creating new process (module)
 * @param[in] number_of_module Number of module in loaded configuration
 * @return Array of strings with information for command "execvp"
 */
char ** make_module_arguments(const int number_of_module);



/** Prints menu with options of commands for supervisor and reads one char from terminal (selected command).
 * @return Number of selected command.
 */
int print_menu();



/** Creates new process, saves information about module to structure "running_module", redirects stdout and stderr and executes new module.
 * @param[in] module_number Number of module in loaded configuration
 */
void start_module(const int module_number);



/** Creates new process, increments "module_restart_cnt, redirects stdout and stderr and re-executes module.
 * @param[in] module_number Number of module in loaded configuration
 */
void restart_module(const int module_number);



/** Updates status of every running module in their variable "module_status". */
void update_module_status();

#endif