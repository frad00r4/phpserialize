#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "phpserialize_serialize.h"
#include "phpserialize_ser2var.h"


void phpserialize_serialize_destroy( phpserialize_var_t **unserialize ) {
	if( unserialize == NULL ) {
		return;
	}
	if( (*unserialize) == NULL ) {
		return;
	}

	switch( (*unserialize)->type ) {
		case _STR:
			if( (*unserialize)->data.p_str == NULL ) {
				break;
			}
			free( (*unserialize)->data.p_str );
			break;
		case _ARRAY:
			if( (*unserialize)->data.p_array == NULL ) {
				break;
			}
			phpserialize_array_destroy( &(*unserialize)->data.p_array );
			break;
	}
	free( (*unserialize) );
}


phpserialize_var_t *phpserialize_serialize_decode( const char *serialize, char **end, int flags ) {
	phpserialize_var_t *unserialize;
	char               *ptr;

	if( serialize == NULL ) {
		return NULL;
	}
	if( serialize[1] != ':' ) {
		return NULL;
	}
	unserialize = calloc( 1, sizeof( phpserialize_var_t ) );
	if( unserialize == NULL ) {
		return NULL;
	}

	unserialize->type = serialize[0];
	ptr = (char*)serialize + 2;

	switch( unserialize->type ) {
		case _STR:
			unserialize->data.p_str = phpserialize_ser2var_str( ptr, end );
			if( unserialize->data.p_str == NULL ) {
				phpserialize_serialize_destroy( &unserialize );
				return NULL;
			}
			break;
		case _INT:
			unserialize->data.p_int = phpserialize_ser2var_int( ptr, end );
			if( unserialize->data.p_int == -1 ) {
				phpserialize_serialize_destroy( &unserialize );
				return NULL;
			}
			break;
		case _ARRAY:
			unserialize->data.p_array = phpserialize_ser2var_array( ptr, end );
			if( unserialize->data.p_array == NULL ) {
				phpserialize_serialize_destroy( &unserialize );
				return NULL;
			}
			break;
		case _BOOL:
			unserialize->data.p_int = phpserialize_ser2var_bool( ptr, end );
			if( unserialize->data.p_int == -1 ) {
				phpserialize_serialize_destroy( &unserialize );
				return NULL;
			}
			break;
		default:
			free( unserialize );
			return NULL;
	}

	return unserialize;
}


int phpserialize_serialize_encode( phpserialize_var_t *unserialize, char *serialize ) {
	int res;

	switch( unserialize->type ) {
		case _INT:
			res = phpserialize_var2ser_int( unserialize, serialize );
			break;
		case _STR:
			res = phpserialize_var2ser_str( unserialize, serialize, 4096 );
			break;
		case _BOOL:
			res = phpserialize_var2ser_bool( unserialize, serialize );
			break;
		case _ARRAY:
			res = phpserialize_var2ser_array( unserialize, serialize );
			break;
		default:
			return -1;
	}
	if( res == -1 ) {
		return -1;
	}

	return res;
}
