#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "mysql_connection_common.h"

//*** Treating XML documents, the config file of Lib-a-Lot is an XML document
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

#include <stdbool.h>

#include "module_context.h"

typedef struct {
	char* event_name;
	FILE* event_file;
	char* error_name;
	FILE* error_file;
} log_names_files;

void getXmlNodeProperty( xmlNode*, xmlChar*, char** );

log_names_files* log_names_files_new( xmlNode* );
void log_names_files_free( log_names_files* );

typedef struct {
	char* name;
	char* path;
	void* handle;
	char* need_name;
	char* exec_name;

	bool (*need)( module_context*, unsigned long, unsigned long, unsigned long );
	unsigned long (*exec)( module_context*, unsigned long, unsigned long, unsigned long );

	log_names_files* log;
} module;

module* module_new( void );
void module_free( module* );

typedef struct configuration_struct_tag {
//*** For handling mysql connections
	mysql_configuration* mysql_server_configuration;

//*** For handling engine connections
	char* engine_ip;
	char* engine_port;
	int engine_reuse_address;
	int engine_reuse_port;

//*** For handling the modules
	size_t module_count;
	module** module_array;

//*** For fetching files from the internet
	char* path_sifts;
	char* path_fasta;
	char* path_pdb;
	char* path_mmtf;
	char* path_mmcif;

//*** The log files
	log_names_files* log_admin;
	log_names_files* log_libalot;
	log_names_files* log_protmutlibalot;
	log_names_files* log_mysql;
	log_names_files* log_engine;
} configuration_struct;

void configuration_load( configuration_struct* );
void configuration_unload( configuration_struct* );
void configuration_reload( configuration_struct* );

#endif
