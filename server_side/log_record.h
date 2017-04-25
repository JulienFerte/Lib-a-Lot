#ifndef LOG_RECORD_H
#define LOG_RECORD_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include "configuration.h"

#define LOG_ADMIN_EVENT				0
#define LOG_LIBALOT_EVENT			1
#define LOG_MYSQL_EVENT				3
#define LOG_ENGINE_EVENT			4
#define LOG_MODULE_EVENT			10

#define LOG_ADMIN_ERROR				5
#define LOG_LIBALOT_ERROR			6
#define LOG_MYSQL_ERROR				8
#define LOG_ENGINE_ERROR			9
#define LOG_MODULE_ERROR			11

bool clear_file( char*, FILE** );

bool clear_log_file( configuration_struct*, unsigned int );
bool clear_log_event( log_names_files* );
bool clear_log_error( log_names_files* );

void* clear_logs( void* );

void write_log( configuration_struct*, unsigned int, const char*, ... );
void write_log_module( configuration_struct*, unsigned int, int, const char*, ... );

#endif
