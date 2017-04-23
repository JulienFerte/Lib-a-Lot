#include "c_header.h"

char* char_array_copy( const char* string ) {
	size_t result_length = strlen( string );
	char* result = (char*) malloc( sizeof( char ) * ( result_length + 1 ) );
	strcpy( result, string );
	result[ result_length ] = '\0';
	return result;
}
char* char_array_ncopy( const char* string, size_t length ) {
	char* result = (char*) malloc( sizeof( char ) * ( length + 1 ) );
	strncpy( result, string, length );
	result[ length ] = '\0';
	return result;
}
