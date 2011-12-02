#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "phpserialize_ser2var.h"
#include "phpserialize_array.h"
#include "phpserialize_serialize.h"

static int phpserialize_atoi( const char *str ) {
	char *buf = (char*)str;

	while( *buf != 0 ) {
		if( isdigit( *buf++ ) == 0 )
			return -1;
	}

	return atoi( str );
}

int phpserialize_ser2var_bool( const char *str, char **next ) {
	int   res;
	char *ptr_next;
	char  tmp[2] = "";

	ptr_next = strchr( str, ';' );
	if( ptr_next == NULL ) {
		return -1;
	}

	if( ( ptr_next - str ) != 1 ) {
		return -1;
	}

	strncpy( tmp, str, 1 );
	res = phpserialize_atoi( tmp );

	if( ( res != TRUE ) && ( res != FALSE ) ) {
		return -1;
	}

	if( next != NULL) {
		(*next) = ptr_next + 1;
	}

	return res;
}


int phpserialize_ser2var_int( const char *str, char **next ) {
	int   res;
	char *ptr_next;
	char  tmp[50] = "";

	ptr_next = strchr( str, ';' );
	if( ptr_next == NULL ) {
		return -1;
	}

	strncpy( tmp, str, sizeof( char ) * ( ptr_next - str ) );
	res = phpserialize_atoi( tmp );

	if( next != NULL) {
		(*next) = ptr_next + 1;
	}

	return res;
}


char *phpserialize_ser2var_str( const char *str, char **next ) {
	int   len;
	int   i = 0;
	char *res;
	char *str_ptr;
	char  tmp[4096] = "";

	str_ptr = strchr( str, ':' );
	if( str_ptr == NULL ) {
		return NULL;
	}
	strncpy( tmp, str, sizeof( char ) * ( str_ptr - str ) );
	len = phpserialize_atoi( tmp );
	if( len == -1 ) {
		return NULL;
	}
	str_ptr += 3;

	res = calloc( sizeof( char ), len + 1 );

	for( i = 0; i < len; i++ ) {
		if( *str_ptr == '\\' ) {
			str_ptr++;
			switch( *str_ptr++ ) {
				case '\\':
					res[i] = '\\';
					break;
				case '"':
					res[i] = '"';
					break;
				default:
					free( res );
					return NULL;
			}
			continue;
		}
		res[i] = *str_ptr++;
	}

	if( next != NULL) {
		(*next) = str_ptr + 3;
	}

	return res;
}


phpserialize_array_t *phpserialize_ser2var_array( const char *str, char **next ) {
	char                 *str_ptr;
	char                 *str_ptr_1;
	int                   len;
	int                   i = 0;
	int                   id;
	char                  tmp[4096] = "";
	phpserialize_array_t *array;

	str_ptr = strchr( str, ':' );
	if( str_ptr == NULL ) {
		return NULL;
	}
	strncpy( tmp, str, sizeof( char ) * ( str_ptr - str ) );
	len = phpserialize_atoi( tmp );
	if( len == -1 ) {
		return NULL;
	}
	if( *str_ptr++ != ':' ) {
		return NULL;
	}
	if( *str_ptr++ != '{' ) {
		return NULL;
	}
	array = phpserialize_array_create( len );
	str_ptr_1 = str_ptr;
	while( i < len ) {
		id = phpserialize_array_push( array );
		array->names[id] = phpserialize_serialize_decode( str_ptr, &str_ptr_1, NAME );
		if( array->names[id] == NULL ) {
			phpserialize_array_destroy( &array );
			return NULL;
		}
		array->elements[id] = phpserialize_serialize_decode( str_ptr_1, &str_ptr, DATA );
		if( array->elements[id] == NULL ) {
			phpserialize_array_destroy( &array );
			return NULL;
		}
		i++;
	}
	if( *str_ptr++ != '}' ) {
		return NULL;
	}
	if( next != NULL ) {
		(*next) = str_ptr;
	}

	return array;
}
