#ifndef SIZED_STRING_H
#define SIZED_STRING_H

#include <stddef.h>

typedef struct {
	char* string;
	size_t length;
} sized_string;

sized_string* sized_string_new( void );
sized_string* sized_string_from_string_length( const char*, size_t );

sized_string* sized_string_concat( sized_string*, const char* );
sized_string* sized_string_concatn( sized_string*, const char*, size_t );

sized_string* sized_string_sized_concat( sized_string*, sized_string* );

sized_string* sized_string_concat_char( sized_string*, char );

void sized_string_free( sized_string* );

#endif
