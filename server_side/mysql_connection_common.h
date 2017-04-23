#ifndef MYSQL_CONNECTION_COMMON_H
#define MYSQL_CONNECTION_COMMON_H

//*** I/O-ing with MySQL
#include <mysql/my_global.h>
#include <mysql/my_sys.h>
#include <mysql/mysql.h>
#include <mysql/errmsg.h>

//*** Usual libraries
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sized_string.h"

struct configuration_struct_tag;

typedef struct {
	char* ip;
	char* port;
	char* database;
	char* login;
	char* password;
} mysql_configuration;

mysql_configuration* mysql_configuration_new( void );
void mysql_configuration_free( mysql_configuration* );

void connect_database( struct configuration_struct_tag*, MYSQL* );

void execute_query( struct configuration_struct_tag*, MYSQL*, char*, ... );
char* convert( unsigned long, int );

void parse_row( struct configuration_struct_tag*, MYSQL_ROW*, unsigned long*, char*, ... );

void disconnect_database( MYSQL* );

#endif
