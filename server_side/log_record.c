#include "log_record.h"

#include "configuration.h"
#include "thread_context.h"

bool clear_file( char* file_name, FILE** file_handle ) {
	fclose( *file_handle );
	*file_handle = fopen( file_name, "w" );
	fclose( *file_handle );
	*file_handle = fopen( file_name, "a" );

	return ( (*file_handle) != NULL );
}

bool clear_log_file( configuration_struct* configuration, unsigned int id ) {
	char* file_name = NULL;
	FILE** file_handle = NULL;

//*** Get the correct file name and handle
	switch( id ) {
		case LOG_ADMIN_EVENT :
				file_name = configuration->log_admin->event_name;
				file_handle = &(configuration->log_admin->event_file);
			break;
		case LOG_LIBALOT_EVENT :
				file_name = configuration->log_libalot->event_name;
				file_handle = &(configuration->log_libalot->event_file);
			break;
		case LOG_MYSQL_EVENT :
				file_name = configuration->log_mysql->event_name;
				file_handle = &(configuration->log_mysql->event_file);
			break;
		case LOG_ENGINE_EVENT :
				file_name = configuration->log_engine->event_name;
				file_handle = &(configuration->log_engine->event_file);
			break;

		case LOG_ADMIN_ERROR :
				file_name = configuration->log_admin->error_name;
				file_handle = &(configuration->log_admin->error_file);
			break;
		case LOG_LIBALOT_ERROR :
				file_name = configuration->log_libalot->error_name;
				file_handle = &(configuration->log_libalot->error_file);
			break;
		case LOG_MYSQL_ERROR :
				file_name = configuration->log_mysql->error_name;
				file_handle = &(configuration->log_mysql->error_file);
			break;
		case LOG_ENGINE_ERROR :
				file_name = configuration->log_engine->error_name;
				file_handle = &(configuration->log_engine->error_file);
			break;
	}

	return ( ( file_name != NULL && file_handle != NULL ) ? clear_file( file_name, file_handle ) : false );
}
bool clear_log_event( log_names_files* log ) {
	return ( ( log != NULL ) && clear_file( log->event_name, &( log->event_file ) ) );
}
bool clear_log_error( log_names_files* log ) {
	return ( ( log != NULL ) && clear_file( log->error_name, &( log->error_file ) ) );
}

void* clear_logs( void* parameter ) {
	thread_context* context = (thread_context*) parameter;

	unsigned int i;
	for( i = LOG_ADMIN_EVENT; i <= LOG_ENGINE_ERROR; ++i ) {
		clear_log_file( context->configuration, i );
	}
	for( i = 0; i < context->configuration->module_count; ++i ) {
		clear_log_event( context->configuration->module_array[i]->log );
		clear_log_error( context->configuration->module_array[i]->log );
	}

	return NULL;
}

void write_log( configuration_struct* configuration, unsigned int id, const char* format, ... ) {
	FILE* log_file = NULL;

//*** Get the correct file handle
	switch( id ) {
		case LOG_ADMIN_EVENT :
			log_file = configuration->log_admin->event_file;
			break;
		case LOG_LIBALOT_EVENT :
			log_file = configuration->log_libalot->event_file;
			break;
		case LOG_MYSQL_EVENT :
			log_file = configuration->log_mysql->event_file;
			break;
		case LOG_ENGINE_EVENT :
			log_file = configuration->log_engine->event_file;
			break;

		case LOG_ADMIN_ERROR :
			log_file = configuration->log_admin->error_file;
			break;
		case LOG_LIBALOT_ERROR :
			log_file = configuration->log_libalot->error_file;
			break;
		case LOG_MYSQL_ERROR :
			log_file = configuration->log_mysql->error_file;
			break;
		case LOG_ENGINE_ERROR :
			log_file = configuration->log_engine->error_file;
			break;
	}

	if( log_file == NULL ) {
		switch( id ) {
			case LOG_ADMIN_EVENT :
				log_file = stdout;
				fprintf( log_file, "No admin event log file. " );
				break;
			case LOG_LIBALOT_EVENT :
				log_file = stdout;
				fprintf( log_file, "No Lib-a-Lot event log file. " );
				break;
			case LOG_MYSQL_EVENT :
				log_file = stdout;
				fprintf( log_file, "No MySQL event log file. " );
				break;
			case LOG_ENGINE_EVENT :
				log_file = stdout;
				fprintf( log_file, "No engine event log file. " );
				break;

			case LOG_ADMIN_ERROR :
				log_file = stderr;
				fprintf( log_file, "No admin error log file. " );
				break;
			case LOG_LIBALOT_ERROR :
				log_file = stderr;
				fprintf( log_file, "No Lib-a-Lot error log file. " );
				break;
			case LOG_MYSQL_ERROR :
				log_file = stderr;
				fprintf( log_file, "No MySQL error log file. " );
				break;
			case LOG_ENGINE_ERROR :
				log_file = stderr;
				fprintf( log_file, "No engine error log file. " );
				break;
		}
	}

//*** Getting the time
	struct timeval time_value;
	gettimeofday( &time_value, NULL );
	struct tm* now_tm = localtime( &(time_value.tv_sec) );

//*** Printing the time in a buffer
	char time_buffer[64];
	strftime( time_buffer, sizeof( time_buffer ), "%Y-%m-%d %H:%M:%S", now_tm );
	fprintf( log_file, "%s.%06ld: ", time_buffer, time_value.tv_usec );

//*** Printing the message in the log file
	va_list arguments;
	va_start( arguments, format );
	vfprintf( log_file, format, arguments );
	va_end( arguments );

//*** End of line in the log file
	fprintf( log_file, "\n" );

//*** Flushing the content
	fflush( log_file );
}

void write_log_module( configuration_struct* configuration, unsigned int module_id, int flag, const char* format, ... ) {
	FILE* log_file = NULL;

//*** Get the correct file handle
	module* module_stuff = configuration->module_array[ module_id ];

	if( flag == LOG_MODULE_EVENT ) {
		log_file = module_stuff->log->event_file;
		if( log_file == NULL ) {
			log_file = stdout;
			fprintf( log_file, "No module event file. " );
		}
	}
	if( flag == LOG_MODULE_ERROR ) {
		log_file = module_stuff->log->error_file;
		if( log_file == NULL ) {
			log_file = stdout;
			fprintf( log_file, "No module error file. " );
		}
	}

//*** Getting the time
	struct timeval time_value;
	gettimeofday( &time_value, NULL );
	struct tm* now_tm = localtime( &(time_value.tv_sec) );

//*** Printing the time in a buffer
	char time_buffer[64];
	strftime( time_buffer, sizeof( time_buffer ), "%Y-%m-%d %H:%M:%S", now_tm );
	fprintf( log_file, "%s.%06ld: ", time_buffer, time_value.tv_usec );

//*** Printing the message in the log file
	va_list arguments;
	va_start( arguments, format );
	vfprintf( log_file, format, arguments );
	va_end( arguments );

//*** End of line in the log file
	fprintf( log_file, "\n" );

//*** Flushing the content
	fflush( log_file );
}
