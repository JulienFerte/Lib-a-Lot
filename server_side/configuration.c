#include "configuration.h"

#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

module* module_new( void ) {
	module* result = malloc( sizeof( module ) );

	result->name = NULL;
	result->path = NULL;
	result->handle = NULL;
	result->need_name = NULL;
	result->exec_name = NULL;
	result->need = NULL;
	result->exec = NULL;
	result->log = NULL;

	return result;
}
void module_free( module* mod ) {
	if( mod->name != NULL )
		free( mod->name );
	if( mod->path != NULL )
		free( mod->path );
	dlclose( mod->handle );
	if( mod->need_name != NULL )
		free( mod->need_name );
	if( mod->exec_name != NULL )
		free( mod->exec_name );
	if( mod->log != NULL )
		log_names_files_free( mod->log );

	free( mod );
}

void getXmlNodeProperty( xmlNode* node, xmlChar* property_name, char** pointer_string ) {
	xmlChar* property_value = xmlGetProp( node, property_name );
	if( property_value != NULL ) {
		size_t property_value_length = (size_t) xmlStrlen( property_value );
		(*pointer_string) = malloc( sizeof( char ) * ( property_value_length + 1 ) );
		strncpy( *pointer_string, (char*) property_value, property_value_length );
		(*pointer_string)[ property_value_length ] = '\0';
		xmlFree( property_value );
	}
	else {
		*pointer_string = NULL;
	}
}

log_names_files* log_names_files_new( xmlNode* node ) {
	log_names_files* result = malloc( sizeof( log_names_files ) );

	xmlChar* xml_event = xmlCharStrdup( "event" );
	xmlChar* xml_record_event = xmlCharStrdup( "record_event" );
	xmlChar* xml_error = xmlCharStrdup( "error" );

	char* value = NULL;

//*** Open the event log file
	getXmlNodeProperty( node, xml_event, &(result->event_name) );
	getXmlNodeProperty( node, xml_record_event, &value );
	if( result->event_name != NULL ) {
//printf( "Event log file path : %s\n", result->event_name );
		if( value != NULL && strcmp( value, "true" ) == 0 ) {
//printf( "Event logs recording\n" );
			result->event_file = fopen( result->event_name, "a" );
		}
		else {
//printf( "Event logs NOT recording\n" );
			result->event_file = NULL;
		}

		if( value != NULL ) {
			free( value );
			value = NULL;
		}
	}

//*** Open the error log file
	getXmlNodeProperty( node, xml_error, &(result->error_name) );
	if( result->error_name != NULL ) {
//printf( "Error log file path : %s\n", result->error_name );
		result->error_file = fopen( result->error_name, "a" );
	}

	xmlFree( xml_event );
	xmlFree( xml_record_event );
	xmlFree( xml_error );

	return result;
}
void log_names_files_free( log_names_files* log ) {
	if( log->event_name != NULL ) {
		free( log->event_name );
	}
	if( log->event_file != NULL ) {
		fclose( log->event_file );
	}
	if( log->error_name != NULL ) {
		free( log->error_name );
	}
	if( log->error_file != NULL ) {
		fclose( log->error_file );
	}
	free( log );
}

void configuration_load( configuration_struct* configuration ) {
	xmlDoc* config = NULL;
	xmlNode* current_element = NULL;

//*** Open the config file
	const char* configuration_file_path = "/absolute/path/to/.LibALot.conf";
	if ( ( config = xmlReadFile( configuration_file_path, NULL, 0 ) ) == NULL ) {
		fprintf( stderr, "configuration_load error: could not parse file %s\n", configuration_file_path );
		return;
	}

//*** Initialize the configuration struct
	configuration->mysql_server_configuration = mysql_configuration_new();

	configuration->engine_ip = NULL;
	configuration->engine_port = NULL;
	configuration->engine_reuse_address = 0;
	configuration->engine_reuse_port = 0;

	configuration->module_array = NULL;

	configuration->log_admin = NULL;
	configuration->log_libalot = NULL;
	configuration->log_mysql = NULL;
	configuration->log_engine = NULL;

//*** Prepare all the name needed in the parsing
	xmlChar* xml_configuration = xmlCharStrdup( "configuration" );
	xmlChar* xml_log = xmlCharStrdup( "log" );
	xmlChar* xml_libalot = xmlCharStrdup( "libalot" );
	xmlChar* xml_admin = xmlCharStrdup( "admin" );
	xmlChar* xml_servers = xmlCharStrdup( "servers" );
	xmlChar* xml_mysql = xmlCharStrdup( "mysql" );
	xmlChar* xml_client = xmlCharStrdup( "client" );
	xmlChar* xml_engine = xmlCharStrdup( "engine" );
	xmlChar* xml_reuseaddress = xmlCharStrdup( "reuseaddress" );
	xmlChar* xml_reuseport = xmlCharStrdup( "reuseport" );
	xmlChar* xml_core = xmlCharStrdup( "core" );
	xmlChar* xml_modulelist = xmlCharStrdup( "modulelist" );
	xmlChar* xml_module = xmlCharStrdup( "module" );
	xmlChar* xml_ip = xmlCharStrdup( "ip" );
	xmlChar* xml_port = xmlCharStrdup( "port" );
	xmlChar* xml_database = xmlCharStrdup( "database" );
	xmlChar* xml_login = xmlCharStrdup( "login" );
	xmlChar* xml_password = xmlCharStrdup( "password" );
	xmlChar* xml_name = xmlCharStrdup( "name" );
	xmlChar* xml_path = xmlCharStrdup( "path" );
	xmlChar* xml_need = xmlCharStrdup( "need" );
	xmlChar* xml_exec = xmlCharStrdup( "exec" );

	char* error;
	char* value = NULL;

	xmlNode* child;
	xmlNode* child2;
	xmlNode* child3;

//*** Parsing the config file to get the server's IP and port, and to get the list of tools. From XML to xmlNode
	current_element = xmlDocGetRootElement(config);
	if( xmlStrcmp( current_element->name, xml_configuration ) == 0 ) {

//*** The document root of the XML file is named "Configuration"
//*** Scan the children, named servers, modulelist, source
		for( child = current_element->children; child != NULL; child = child->next ) {
			if( xmlStrcmp( child->name, xml_log ) == 0 ) {

//*** We are in the node "log"
				for( child2 = child->children; child2 != NULL; child2 = child2->next ) {

					if( xmlStrcmp( child2->name, xml_admin ) == 0 ) {
						if( configuration->log_admin != NULL ) {
							log_names_files_free( configuration->log_admin );
						}
						configuration->log_admin = log_names_files_new( child2 );
					}

					if( xmlStrcmp( child2->name, xml_libalot ) == 0 ) {
						if( configuration->log_libalot != NULL ) {
							log_names_files_free( configuration->log_libalot );
						}
						configuration->log_libalot = log_names_files_new( child2 );
					}

					if( xmlStrcmp( child2->name, xml_mysql ) == 0 ) {
						if( configuration->log_mysql != NULL ) {
							log_names_files_free( configuration->log_mysql );
						}
						configuration->log_mysql = log_names_files_new( child2 );
					}

					if( xmlStrcmp( child2->name, xml_engine ) == 0 ) {
						if( configuration->log_engine != NULL ) {
							log_names_files_free( configuration->log_engine );
						}
						configuration->log_engine = log_names_files_new( child2 );
					}
				}
			}

			if( xmlStrcmp( child->name, xml_servers ) == 0 ) {

//*** We are in the node "servers"
				for( child2 = child->children; child2 != NULL; child2 = child2->next ) {

					if( xmlStrcmp( child2->name, xml_mysql ) == 0 ) {

//*** Put the value of the property "ip" into the variable mysql_server_ip
						if( configuration->mysql_server_configuration->ip != NULL ) {
							free( configuration->mysql_server_configuration->ip );
						}
						getXmlNodeProperty( child2, xml_ip, &(configuration->mysql_server_configuration->ip) );

//printf( "MySQL server IP : %s\n", configuration->mysql_server_configuration->ip );

//*** do the same about the property "port"
						if( configuration->mysql_server_configuration->port != NULL ) {
							free( configuration->mysql_server_configuration->port );
						}
						getXmlNodeProperty( child2, xml_port, &(configuration->mysql_server_configuration->port) );

//printf( "MySQL server port : %s\n", configuration->mysql_server_configuration->port );

//*** do the same about the property "database"
						if( configuration->mysql_server_configuration->database != NULL ) {
							free( configuration->mysql_server_configuration->database );
						}
						getXmlNodeProperty( child2, xml_database, &(configuration->mysql_server_configuration->database) );

//printf( "MySQL server database : %s\n", configuration->mysql_server_configuration->database );

						for( child3 = child2->children; child3 != NULL; child3 = child3->next ) {

//*** get the engine's login and password to MySQL
							if( xmlStrcmp( child3->name, xml_engine ) == 0 ) {
								if( configuration->mysql_server_configuration->login != NULL ) {
									free( configuration->mysql_server_configuration->login );
								}
								getXmlNodeProperty( child3, xml_login, &(configuration->mysql_server_configuration->login) );

//printf( "MySQL server login : %s\n", configuration->mysql_server_configuration->login );

								if( configuration->mysql_server_configuration->password != NULL ) {
									free( configuration->mysql_server_configuration->password );
								}
								getXmlNodeProperty( child3, xml_password, &(configuration->mysql_server_configuration->password) );

//printf( "MySQL server password : %s\n", configuration->mysql_server_configuration->password );

							}
						}
					}

					if( xmlStrcmp( child2->name, xml_core ) == 0 ) {

//*** Put the value of the property "ip" into the variable engine_incoming_port
						if( configuration->engine_ip != NULL ) {
							free( configuration->engine_ip );
						}
						getXmlNodeProperty( child2, xml_ip, &(configuration->engine_ip) );

//*** Put the value of the property "port" into the variable engine_incoming_port
						if( configuration->engine_port != NULL ) {
							free( configuration->engine_port );
						}
						getXmlNodeProperty( child2, xml_port, &(configuration->engine_port) );

//*** Put the value of the property "reuseaddress" into the variable engine_reuse_address
						if( value != NULL ) {
							free( value );
						}
						getXmlNodeProperty( child2, xml_reuseaddress, &value );
						if( value != NULL ) {
							configuration->engine_reuse_address = ( strcmp( value, "true" ) == 0 ? 0xFF : 0 );
							free( value );
							value = NULL;
						}
						else {
							configuration->engine_reuse_address = true;
						}

//*** Put the value of the property "reuseport" into the variable engine_reuse_port
						if( value != NULL ) {
							free( value );
						}
						getXmlNodeProperty( child2, xml_reuseport, &value );
						if( value != NULL ) {
							configuration->engine_reuse_port = ( strcmp( value, "true" ) == 0 ? 0xFF : 0 );
							free( value );
							value = NULL;
						}
						else {
							configuration->engine_reuse_port = false;
						}
					}
				}
			}

			if( xmlStrcmp( child->name, xml_modulelist ) == 0 ) {

//*** We are in the node "modulelist"
				if(configuration->module_array != NULL ) {
					int i = 0;
					for( ; i < configuration->module_count; ++i ) {
						free( (configuration->module_array)[i] );
					}
					free( configuration->module_array );
				}
				configuration->module_array = NULL;
				configuration->module_count = 0;

//*** First we count the number of modules
				for( child2 = child->children; child2 != NULL; child2 = child2->next ) {
					if( xmlStrcmp( child2->name, xml_module ) == 0 ) {
						++(configuration->module_count);
					}
				}

				configuration->module_array = malloc( sizeof( module* ) * (configuration->module_count) );

				int i = 0;
				for( child2 = child->children; child2 != NULL; child2 = child2->next ) {

					if( xmlStrcmp( child2->name, xml_module ) == 0 ) {
						module* new_module = malloc( sizeof( module ) );

//*** Get the properties name, prefix and path from the XML document
						getXmlNodeProperty( child2, xml_name, &(new_module->name) );
						getXmlNodeProperty( child2, xml_path, &(new_module->path) );
						getXmlNodeProperty( child2, xml_need, &(new_module->need_name) );
						getXmlNodeProperty( child2, xml_exec, &(new_module->exec_name) );

//*** Get the functions from the library
						new_module->handle = dlopen( new_module->path, RTLD_NOW | RTLD_GLOBAL );
						if( !(new_module->handle) ) {
							fprintf( stderr, "%s\n", dlerror() );
						}
						dlerror();

						new_module->need = dlsym( new_module->handle, new_module->need_name );
						if((error = dlerror()) != NULL ) {
							fprintf( stderr, "%s\n", error );
						}
						else {
//							fprintf( stdout, "Successfully loaded the function %s\n", new_module->need_name );
						}

						new_module->exec = dlsym( new_module->handle, new_module->exec_name );
						if((error = dlerror()) != NULL ) {
							fprintf( stderr, "%s\n", error );
						}
						else {
//							fprintf( stdout, "Successfully loaded the function %s\n", new_module->exec_name );
						}

						if( value != NULL ) {
							free( value );
						}

						new_module->log = log_names_files_new( child2 );

						(configuration->module_array)[i] = new_module;
						++i;
					}
				}
			}
		}
	}

//*** Remove all the names needed beforehand
	xmlFree( xml_exec );
	xmlFree( xml_need );
	xmlFree( xml_path );
	xmlFree( xml_name );
	xmlFree( xml_password );
	xmlFree( xml_login );
	xmlFree( xml_database );
	xmlFree( xml_port );
	xmlFree( xml_ip );
	xmlFree( xml_module );
	xmlFree( xml_modulelist );
	xmlFree( xml_core );
	xmlFree( xml_reuseaddress );
	xmlFree( xml_reuseport );
	xmlFree( xml_engine );
	xmlFree( xml_client );
	xmlFree( xml_mysql );
	xmlFree( xml_servers );
	xmlFree( xml_log );
	xmlFree( xml_libalot );
	xmlFree( xml_admin );
	xmlFree( xml_configuration );

	xmlFreeDoc( config );
	xmlCleanupParser();
}

void configuration_unload( configuration_struct* configuration ) {
//*** Firstly, free everything
	if( configuration->mysql_server_configuration != NULL )
		mysql_configuration_free( configuration->mysql_server_configuration );

	if( configuration->engine_ip != NULL )
		free( configuration->engine_ip );
	if( configuration->engine_port != NULL )
		free( configuration->engine_port );

	if( configuration->module_array != NULL ) {
		int i = 0;
		for( ; i < configuration->module_count; ++i ) {
			free( (configuration->module_array)[i] );
		}
		free( configuration->module_array );
	}

	if( configuration->log_admin != NULL )
		log_names_files_free( configuration->log_admin );
	if( configuration->log_libalot != NULL )
		log_names_files_free( configuration->log_libalot );
	if( configuration->log_mysql != NULL )
		log_names_files_free( configuration->log_mysql );
	if( configuration->log_engine != NULL )
		log_names_files_free( configuration->log_engine );

//*** Secondly, blank everything
	configuration->mysql_server_configuration = NULL;

	configuration->engine_ip = NULL;
	configuration->engine_port = NULL;
	configuration->engine_reuse_address = 0;
	configuration->engine_reuse_port = 0;

	configuration->module_array = NULL;

	configuration->log_admin = NULL;
	configuration->log_libalot = NULL;
	configuration->log_mysql = NULL;
	configuration->log_engine = NULL;
}

void configuration_reload( configuration_struct* configuration ) {
	configuration_unload( configuration );
	configuration_load( configuration );
}
