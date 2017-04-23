#include "mysql_connection_common.h"

#include "configuration.h"
#include "log_record.h"
#include "sized_string.h"
#include <stdarg.h>

mysql_configuration* mysql_configuration_new( void ) {
	mysql_configuration* result = malloc( sizeof( mysql_configuration ) );
	if( result == NULL ) {
		exit( EXIT_FAILURE );
	}

	result->ip = NULL;
	result->port = NULL;
	result->database = NULL;
	result->login = NULL;
	result->password = NULL;

	return result;
}
void mysql_configuration_free( mysql_configuration* configuration ) {
	if( configuration != NULL ) {
		if( configuration->ip != NULL ) {
			free( configuration->ip );
		}

		if( configuration->port != NULL ) {
			free( configuration->port );
		}

		if( configuration->database != NULL ) {
			free( configuration->database );
		}

		if( configuration->login != NULL ) {
			free( configuration->login );
		}

		if( configuration->password != NULL ) {
			free( configuration->password );
		}

		free( configuration );
	}
}

void connect_database( configuration_struct* configuration, MYSQL* connection_mysql ) {
	my_init();
	mysql_init( connection_mysql );

	write_log( configuration, LOG_MYSQL_EVENT, "Connection to MySQL at %s %s %s %s",
																		configuration->mysql_server_configuration->ip,
																		configuration->mysql_server_configuration->login,
																		configuration->mysql_server_configuration->password,
																		configuration->mysql_server_configuration->database );

	MYSQL* attempt = mysql_real_connect( connection_mysql,
											configuration->mysql_server_configuration->ip,
											configuration->mysql_server_configuration->login,
											configuration->mysql_server_configuration->password,
											configuration->mysql_server_configuration->database,
											0, NULL, 0 );
	if( attempt != NULL ) {
		write_log( configuration, LOG_MYSQL_EVENT, "Successfully connected to the MySQL database" );
	}
	else {
		write_log( configuration, LOG_MYSQL_ERROR, "Error while connecting to the database: %s", mysql_error( connection_mysql ) );
	}
}

void execute_query( configuration_struct* configuration, MYSQL* database_connection, char* format, ... ) {
	char* begin = NULL;
	unsigned int count = 0;
	char* current = NULL;

	sized_string* result = sized_string_new();

	char c;
	char* s;
	unsigned long u;
	long l;
	double d;
	size_t z;

	char* quote_begin;
	unsigned int quote_count;
	char* quote_current;

	static char buffer[50];
	unsigned int length = 0;

	va_list arg;
	va_start( arg, format );

	for( current = format; *current != '\0'; ++current ) {
		begin = current;
		count = 0;

		while( *current != '\0' && *current != '%' ) {
			++count;
			++current;
		}

//*** Two different treatments: the '%' is discarded while the '\0' is kept.

		if( *current == '%' ) {
			result = sized_string_concatn( result, begin, count );

			++current;
			switch(*current) {
				case 'c':
					c = va_arg( arg, int );
					result = sized_string_concatn( result, &c, 1 );
					break;
				case 's':
					s = va_arg( arg, char* );
					result = sized_string_concat( result, s );
					break;
				case 'u':
					u = va_arg( arg, unsigned long );
					result = sized_string_concat( result, convert( u, 10 ) );
					break;
				case 'l':
					l = va_arg( arg, long );
					length = sprintf( buffer, "%li", l );
					result = sized_string_concatn( result, buffer, length );
					break;
				case 'f':
					d = va_arg( arg, double );
					length = sprintf( buffer, "%f", d );
					result = sized_string_concatn( result, buffer, length );
					break;
				case 'q':
					s = va_arg( arg, char* );
//*** Escape simple quotes
					if( s != NULL ) {
						quote_begin = s;
						quote_count = 0;
						quote_current = NULL;
						while( *quote_begin != '\0' ) {
							quote_current = quote_begin;
							quote_count = 0;
							while( *quote_current != '\0' && *quote_current != '\'' ) {
								++quote_count;
								++quote_current;
							}
							result = sized_string_concatn( result, quote_begin, quote_count );
							if( *quote_current == '\'' ) {
								result = sized_string_concat( result, "\\'" );
								++quote_current;
							}
							quote_begin = quote_current;
						}
					}
					else {
						result = sized_string_concat( result, "(null)" );
					}
					break;
				case 'Q':
					s = va_arg( arg, char* );
//*** Escape double quotes
					if( s != NULL ) {
						quote_begin = s;
						quote_count = 0;
						quote_current = NULL;
						while( *quote_begin != '\0' ) {
							quote_current = quote_begin;
							quote_count = 0;
							while( *quote_current != '\0' && *quote_current != '"' ) {
								++quote_count;
								++quote_current;
							}
							result = sized_string_concatn( result, quote_begin, quote_count );
							if( *quote_current == '"' ) {
								result = sized_string_concat( result, "\\\"" );
								++quote_current;
							}
							quote_begin = quote_current;
						}
					}
					else {
						result = sized_string_concat( result, "(null)" );
					}
					break;
				case 'z':
					z = va_arg( arg, size_t );
					length = sprintf( buffer, "%zd", z );
					result = sized_string_concatn( result, buffer, length );
					break;
				case '%':
					result = sized_string_concat( result, "%" );
					break;
			}
		}
		else {
			result = sized_string_concatn( result, begin, count + 1 );
			break;
		}
	}

	va_end(arg);

	write_log( configuration, LOG_MYSQL_EVENT, "Executing the query %s", result->string );

	switch( mysql_query( database_connection, result->string ) ) {
		case CR_COMMANDS_OUT_OF_SYNC:
			write_log( configuration, LOG_MYSQL_ERROR, "Error when executing %s, command out of sync: %u, %s\n", result->string, mysql_errno(database_connection), mysql_error(database_connection) );
			break;
		case CR_SERVER_GONE_ERROR:
			write_log( configuration, LOG_MYSQL_ERROR, "Error when executing %s, sever gone: %u, %s\n", result->string, mysql_errno(database_connection), mysql_error(database_connection) );
			break;
		case CR_SERVER_LOST:
			write_log( configuration, LOG_MYSQL_ERROR, "Error when executing %s, sever lost: %u, %s\n", result->string, mysql_errno(database_connection), mysql_error(database_connection) );
			break;
		case CR_UNKNOWN_ERROR:
			write_log( configuration, LOG_MYSQL_ERROR, "Error when executing %s, unknown error: %u, %s\n", result->string, mysql_errno(database_connection), mysql_error(database_connection) );
			break;
		default:
			write_log( configuration, LOG_MYSQL_EVENT, "Successfully executed the query %s", result->string );
			break;
	}

	sized_string_free( result );
}
char* convert( unsigned long num, int base ) {
	static char Representation[]="0123456789ABCDEF";
	static char buffer[50];
	char* ptr;

	ptr = &buffer[49];
	*ptr='\0';

	do {
		*--ptr = Representation[num%base];
		num /= base;
	} while( num != 0 );

	return ptr;
}

void parse_row( configuration_struct* configuration, MYSQL_ROW* row, unsigned long* lengths, char* format, ... ) {
	unsigned int count = 0;
	char* current = NULL;

	bool* bool_;

	signed char* signed_char_;
	char* char_;
	unsigned char* unsigned_char_;

	signed short int* signed_short_int_;
	unsigned short int* unsigned_short_int_;

	signed int* signed_int_;
	unsigned int* unsigned_int_;

	signed long int* signed_long_int_;
	unsigned long int* unsigned_long_int_;

	signed long long int* signed_long_long_int_;
	unsigned long long int* unsigned_long_long_int_;

	size_t* size_t_;

	float* float_;
	double* double_;
	long double* long_double_;

	MYSQL_TIME* mysql_time_;

	char** string_;

//	void* void_;

	size_t value_length = 255;
	size_t new_value_length = 0;
	char* value = malloc( sizeof( char ) * ( value_length + 1 ) );

	va_list arg;
	va_start( arg, format );

	int rank = 0;

	char current0;
	char current1;
	char current2;
	signed char local_signed_char_;

	for( current = format; *current != '\0'; ++current ) {
		count = 0;

		while( *current != '\0' && *current != '%' ) {
			++count;
			++current;
		}

		if( *current == '%' ) {
			++current;
			current0 = *current;
			current1 = *(current + 1);
			current2 = *(current + 2);

//*** Get the value from the column into a string
			new_value_length = lengths[rank];
			if( new_value_length > value_length ) {
				value = realloc( value, sizeof( char ) * ( new_value_length + 1 ) );
			}
			value_length = new_value_length;
			if( (*row)[rank] != NULL ) {
				strncpy( value, (*row)[rank], value_length );
				value[ value_length ] = '\0';

				if( current0 == '0' ) {
					bool_ = va_arg( arg, bool* );
					sscanf( value, "%hhd", &local_signed_char_ );
					*bool_ = ( local_signed_char_ == 0 );
				}
				else if( current0 == '1' ) {
					bool_ = va_arg( arg, bool* );
					sscanf( value, "%hhd", &local_signed_char_ );
					*bool_ = ( local_signed_char_ != 0 );
				}
				else if( current0 == 'h' && current1 == 'h' && current2 == 'd' ) {
					signed_char_ = va_arg( arg, signed char* );
					sscanf( value, "%hhd", signed_char_ );
				}
				else if( current0 == 'c' ) {
					char_ = va_arg( arg, char* );
					*char_ = value[0];
				}
				else if( current0 == 's' ) {
					string_ = va_arg( arg, char** );
					*string_ = malloc( sizeof( char ) * ( value_length + 1 ) );
					strncpy( *string_, value, value_length );
					(*string_)[ value_length ] = '\0';
				}
				else if( current0 == 'h' && current1 == 'h' && current2 == 'u' ) {
					unsigned_char_ = va_arg( arg, unsigned char* );
					sscanf( value, "%hhu", unsigned_char_ );
				}
				else if( current0 == 'h' && current1 == 'd' ) {
					signed_short_int_ = va_arg( arg, signed short int* );
					sscanf( value, "%hd", signed_short_int_ );
				}
				else if( current0 == 'h' && current1 == 'u' ) {
					unsigned_short_int_ = va_arg( arg, unsigned short int* );
					sscanf( value, "%hu", unsigned_short_int_ );
				}
				else if( current0 == 'd' ) {
					signed_int_ = va_arg( arg, signed int* );
					sscanf( value, "%d", signed_int_ );
				}
				else if( current0 == 'u' ) {
					unsigned_int_ = va_arg( arg, unsigned int* );
					sscanf( value, "%u", unsigned_int_ );
				}
				else if( current0 == 'l' && current1 == 'd' ) {
					signed_long_int_ = va_arg( arg, signed long int* );
					sscanf( value, "%ld", signed_long_int_ );
				}
				else if( current0 == 'l' && current1 == 'u' ) {
					unsigned_long_int_ = va_arg( arg, unsigned long int* );
					sscanf( value, "%lu", unsigned_long_int_ );
				}
				else if( current0 == 'l' && current1 == 'l' && current1 == 'd' ) {
					signed_long_long_int_ = va_arg( arg, signed long long int* );
					sscanf( value, "%lld", signed_long_long_int_ );
				}
				else if( current0 == 'l' && current1 == 'l' && current1 == 'u' ) {
					unsigned_long_long_int_ = va_arg( arg, unsigned long long int* );
					sscanf( value, "%llu", unsigned_long_long_int_ );
				}
				else if( current0 == 'z' ) {
					size_t_ = va_arg( arg, size_t* );
					sscanf( value, "%zd", size_t_ );
				}
				else if( current0 == 'f' ) {
					float_ = va_arg( arg, float* );
					sscanf( value, "%f", float_ );
				}
				else if( current0 == 'l' && current1 == 'f' ) {
					double_ = va_arg( arg, double* );
					sscanf( value, "%lf", double_ );
				}
				else if( current0 == 'L' && current1 == 'f' ) {
					long_double_ = va_arg( arg, long double* );
					sscanf( value, "%Lf", long_double_ );
				}
				else if( current0 == 'D' ) {
					mysql_time_ = va_arg( arg, MYSQL_TIME* );
					sscanf( value, "%04d-%02d-%02d %02d:%02d:%02d", &(mysql_time_->year), &(mysql_time_->month), &(mysql_time_->day),
																	&(mysql_time_->hour), &(mysql_time_->minute), &(mysql_time_->second) );
				}
				else {
					write_log( configuration, LOG_MYSQL_ERROR, "Error in %s: %s at %d, wrong format *%c*\n", __FILE__, __FUNCTION__, __LINE__, *current );
				}
			}
			else {
//				void_ = va_arg( arg, void** );
//				*void_ = NULL;
				if( current0 == 's' ) {
					string_ = va_arg( arg, char** );
					*string_ = NULL;
				}
				else if( current0 == 'z' ) {
					size_t_ = va_arg( arg, size_t* );
					*size_t_ = 0;
				}
				else {
					write_log( configuration, LOG_MYSQL_ERROR, "Error in %s: %s at %d, column is NULL while the format is not %%s\n", __FILE__, __FUNCTION__, __LINE__ );
				}
			}
		}
		else {
			break;
		}

		++rank;
	}

	free( value );

	va_end(arg);
}

void disconnect_database(MYSQL* connection_mysql) {
	mysql_close( connection_mysql );
}
