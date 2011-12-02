#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "phpserialize_serialize.h"
#include "phpserialize_var2ser.h"


int phpserialize_var2ser_str( const phpserialize_var_t *ser, char *result, int size ) {
	char *result_ptr = (char*)result;
	int   len = strlen( ser->data.p_str );
	int   i;
	int   reserv = 0;

	if( ( result == NULL ) && ( ser->type != _STR ) ) {
		return -1;
	}
	for( i = 0; i < len; i++ ) {
		if( ( ser->data.p_str[i] == '\\' ) || ( ser->data.p_str[i] == '"' ) ) {
			reserv++;
		}
	}
	if( size < ( 15 + len + reserv ) ) {
		return -1;
	}
	result_ptr += sprintf( result, "s:%d:\\\"", len );
	for( i = 0; i < len; i++ ) {
		if( ser->data.p_str[i] == '\\' ) {
			*result_ptr++ = '\\';
			*result_ptr++ = '\\';
			continue;
		}
		if( ser->data.p_str[i] == '"' ) {
			*result_ptr++ = '\\';
			*result_ptr++ = '"';
			continue;
		}
		*result_ptr++ = ser->data.p_str[i];
	}
	sprintf( result_ptr, "\\\";" );

	return strlen( result );
}


int phpserialize_var2ser_int( const phpserialize_var_t *ser, char *result ) {
	if( ( result == NULL ) && ( ser->type != _INT ) ) {
		return -1;
	}
	sprintf( result, "i:%d;", ser->data.p_int );

	return strlen( result );
}


int phpserialize_var2ser_bool( const phpserialize_var_t *ser, char *result ) {
	if( ( result == NULL ) && ( ser->type != _BOOL ) && ( ser->data.p_int > 1 ) && ( ser->data.p_int < 0 ) ) {
		return -1;
	}
	sprintf( result, "b:%d;", ser->data.p_int );

	return 4;
}


int phpserialize_var2ser_array( const phpserialize_var_t *ser, char *result ) {
	char        *result_ptr = (char*)result;
	int          i;
	int          res;

	if( ( result == NULL ) && ( ser->type != _ARRAY ) ) {
		return -1;
	}

	result_ptr += sprintf( result, "a:%d:{", ser->data.p_array->count );
	
	for( i = 0; i < ser->data.p_array->count; i++ ) {
		switch( ser->data.p_array->names[i]->type ) {
			case _INT:
				res = phpserialize_var2ser_int( ser->data.p_array->names[i], result_ptr );
				break;
			case _STR:
				res = phpserialize_var2ser_str( ser->data.p_array->names[i], result_ptr, 4096 );
				break;
			default:
				return -1;
		}
		if( res == -1 ) {
			return -1;
		}
		result_ptr += res;
		switch( ser->data.p_array->elements[i]->type ) {
			case _INT:
				res = phpserialize_var2ser_int( ser->data.p_array->elements[i], result_ptr );
				break;
			case _STR:
				res = phpserialize_var2ser_str( ser->data.p_array->elements[i], result_ptr, 4096 );
				break;
			case _BOOL:
				res = phpserialize_var2ser_bool( ser->data.p_array->elements[i], result_ptr );
				break;
			case _ARRAY:
				res = phpserialize_var2ser_array( ser->data.p_array->elements[i], result_ptr );
				break;
			default:
				return -1;
		}
		if( res == -1 ) {
			return -1;
		}
		result_ptr += res;
	}
	
	sprintf( result_ptr, "}" );

	return strlen( result );
}
