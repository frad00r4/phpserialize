#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "phpserialize_array.h"
#include "phpserialize_serialize.h"


phpserialize_array_t *phpserialize_array_create( int reserved ) {
	phpserialize_array_t *array;
	int                   i;

	if( reserved < 1 ) {
		return NULL;
	}

	array = calloc( 1, sizeof( phpserialize_array_t ) );
	if( array == NULL ) {
		return NULL;
	}

	array->reserved = reserved;
	array->count = 0;
	array->elements = calloc( reserved, sizeof( phpserialize_var_t* ) );
	array->names = calloc( reserved, sizeof( phpserialize_var_t* ) );

	return array;
}


int phpserialize_array_push( phpserialize_array_t *array ) {
	if( array->reserved <= ( array->count ) ) {
		return -1;
	}
	return array->count++;
}


void phpserialize_array_destroy( phpserialize_array_t **array ) {
	int i;
	for( i = 0; i < (*array)->count; i++ ) {
		switch( (*array)->elements[i]->type ) {
			case _ARRAY:
				if( (*array)->elements[i]->data.p_array != NULL ) {
					phpserialize_array_destroy( &(*array)->elements[i]->data.p_array );
					break;
				}
			case _STR:
				if( (*array)->elements[i]->data.p_str != NULL ) {
					free( (*array)->elements[i]->data.p_str );
					break;
				}
		}
		switch( (*array)->names[i]->type ) {
			case _STR:
				if( (*array)->names[i]->data.p_str != NULL ) {
					free( (*array)->names[i]->data.p_str );
					break;
				}
		}
	}
	free( (*array)->elements );
	free( (*array)->names );
	free( *array );
}
