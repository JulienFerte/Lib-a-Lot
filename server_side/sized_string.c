#include "sized_string.h"

#include "log_record.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

sized_string* sized_string_new( void ) {
	sized_string* result = (sized_string*) malloc( sizeof( sized_string ) );

	if( result == NULL ) {
//		write_log( LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: I can't allocate memory for a sized_string", __FUNCTION__, __FILE__, __LINE__ );
		return NULL;
	}

	result->length = 0;
	result->string = (char*) malloc( sizeof( char ) * ( result->length + 1 ) );

	if( result->string == NULL ) {
//		write_log( LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: I can't allocate memory for a char array of length %zu", __FUNCTION__, __FILE__, __LINE__, result->length );
		free( result );
		return NULL;
	}

	result->string[0] = '\0';

	return result;
}
sized_string* sized_string_from_string_length( const char* content, size_t length ) {
	sized_string* result = (sized_string*) malloc( sizeof( sized_string ) );

	if( result == NULL ) {
//		write_log( LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: I can't allocate memory for a sized_string", __FUNCTION__, __FILE__, __LINE__ );
		return NULL;
	}

	result->length = length;
	result->string = (char*) malloc( sizeof( char ) * ( result->length + 1 ) );

	if( result->string == NULL ) {
//		write_log( LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: I can't allocate memory for a char array of length %zu", __FUNCTION__, __FILE__, __LINE__, result->length );
		free( result );
		return NULL;
	}

	strncpy( result->string, content, length );
	result->string[result->length] = '\0';

	return result;
}
sized_string* sized_string_concat( sized_string* string, const char* word ) {
	size_t string_new_length = string->length + strlen( word );

	string->string = (char*) realloc( string->string, sizeof( char ) * ( string_new_length + 1 ) );
	if( string->string == NULL ) {
//		write_log( LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: I can't reallocate memory into a char array of length %zu", __FUNCTION__, __FILE__, __LINE__, string_new_length );
		exit( EXIT_FAILURE );
	}

	strcat( string->string, word );
	string->string[ string_new_length ] = '\0';
	string->length = string_new_length;

	return string;
}
sized_string* sized_string_concatn( sized_string* string, const char* word, size_t count ) {
	size_t string_new_length = string->length + count;

	string->string = (char*) realloc( string->string, sizeof( char ) * ( string_new_length + 1 ) );
	if( string->string == NULL ) {
//		write_log( LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: I can't reallocate memory into a char array of length %zu", __FUNCTION__, __FILE__, __LINE__, string_new_length );
		exit( EXIT_FAILURE );
	}

	strncat( string->string, word, count );
	string->string[ string_new_length ] = '\0';
	string->length = string_new_length;

	return string;
}
sized_string* sized_string_sized_concat( sized_string* lhs, sized_string* rhs ) {
	return sized_string_concatn( lhs, rhs->string, rhs->length );
}
sized_string* sized_string_concat_char( sized_string* string, char letter ) {
	size_t string_new_length = string->length + 1;

	string->string = (char*) realloc( string->string, sizeof( char ) * ( string_new_length + 1 ) );
	if( string->string == NULL ) {
//		write_log( LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: I can't reallocate memory into a char array of length %zu", __FUNCTION__, __FILE__, __LINE__, string_new_length );
		exit( EXIT_FAILURE );
	}

	string->string[ string_new_length - 1 ] = letter;
	string->string[ string_new_length ] = '\0';
	string->length = string_new_length;

	return string;
}
void sized_string_free( sized_string* element ) {
	free( element->string );
	free( element );
}
