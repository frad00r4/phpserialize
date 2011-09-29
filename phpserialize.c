#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "phpserialize.h"


//===========================================================================

static phpserialize_array_t *phpserialize_ser2obj_array( const char *str, char **next );
static int phpserialize_ser2obj_bool( const char *str, char **next );
static int phpserialize_ser2obj_int( const char *str, char **next );
static char *phpserialize_ser2obj_str( const char *str, char **next );

static int phpserialize_obj2ser_bool( const serialize_t *ser, char *result );
static int phpserialize_obj2ser_int( const serialize_t *ser, char *result );
static int phpserialize_obj2ser_str( const serialize_t *ser, char *result, int size );
static int phpserialize_obj2ser_array( const serialize_t *ser, char *result );

static session_t *phpserialize_session_create( int count );
static int phpserialize_atoi( const char *str );

//===========================================================================

static int phpserialize_atoi( const char *str ) {
	char *buf = (char*)str;

	while( *buf != 0 ) {
		if( isdigit( *buf++ ) == 0 )
			return -1;
	}

	return atoi( str );
}


static int phpserialize_ser2obj_bool( const char *str, char **next ) {
	int  res;
	char tmp[4096] = "";

	(*next) = strchr( str, ';' );
	if( (*next) == NULL ) {
		return -1;
	}

	strncpy( tmp, str, sizeof( char ) * ( (*next) - str ) );
	res = phpserialize_atoi( tmp );

	if( ( res > 1 ) || ( res < 0 ) ) {
		return -1;
	}
	(*next)++;

	return res;
}


static int phpserialize_ser2obj_int( const char *str, char **next ) {
	int  res;
	char tmp[4096] = "";

	(*next) = strchr( str, ';' );
	if( (*next) == NULL ) {
		return -1;
	}

	strncpy( tmp, str, sizeof( char ) * ( (*next) - str ) );
	res = phpserialize_atoi( tmp );
	(*next)++;

	return res;
}


static char *phpserialize_ser2obj_str( const char *str, char **next ) {
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
	str_ptr += 3;
	(*next) = str_ptr;
	return res;
}


static phpserialize_array_t *phpserialize_ser2obj_array( const char *str, char **next ) {
	char                *str_ptr;
	char                *str_ptr_1;
	int                  len;
	int                  i = 0;
	char                 tmp[4096] = "";
	phpserialize_array_t *array;
	element_t           *element;


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
		element = phpserialize_array_push( array );
		switch( *str_ptr++ ) {
			case _STR:
				element->name_type = _STR;
				if( *str_ptr++ != ':' ) {
					phpserialize_array_destroy( &array );
					return NULL;
				}
				element->name.str = phpserialize_ser2obj_str( str_ptr, &str_ptr_1 );
				if( element->name.str == NULL ) {
					phpserialize_array_destroy( &array );
					return NULL;
				}
				break;
			case _INT:
				element->name_type = _INT;
				if( *str_ptr++ != ':' ) {
					phpserialize_array_destroy( &array );
					return NULL;
				}
				element->name.dig = phpserialize_ser2obj_int( str_ptr, &str_ptr_1 );
				if( element->name.dig == -1 ) {
					phpserialize_array_destroy( &array );
					return NULL;
				}
				break;
			default:
				phpserialize_array_destroy( &array );
				return NULL;
				break;
		}
		switch( *str_ptr_1++ ) {
			case _STR:
				element->type = _STR;
				if( *str_ptr_1++ != ':' ) {
					phpserialize_array_destroy( &array );
					return NULL;
				}
				element->data.str = phpserialize_ser2obj_str( str_ptr_1, &str_ptr );
				if( element->data.str == NULL ) {
					phpserialize_array_destroy( &array );
					return NULL;
				}
				break;
			case _INT:
				element->type = _INT;
				if( *str_ptr_1++ != ':' ) {
					phpserialize_array_destroy( &array );
					return NULL;
				}
				element->data.dig = phpserialize_ser2obj_int( str_ptr_1, &str_ptr );
				if( element->data.dig == -1 ) {
					phpserialize_array_destroy( &array );
					return NULL;
				}
				break;
			case _ARRAY:
				element->type = _ARRAY;
				if( *str_ptr_1++ != ':' ) {
					phpserialize_array_destroy( &array );
					return NULL;
				}
				element->data.array = phpserialize_ser2obj_array( str_ptr_1, &str_ptr );
				if( element->data.array == NULL ) {
					phpserialize_array_destroy( &array );
					return NULL;
				}
				break;
			case _BOOL:
				element->type = _BOOL;
				if( *str_ptr_1++ != ':' ) {
					phpserialize_array_destroy( &array );
					return NULL;
				}
				element->data.dig = phpserialize_ser2obj_bool( str_ptr_1, &str_ptr );
				if( element->data.dig == -1 ) {
					phpserialize_array_destroy( &array );
					return NULL;
				}
				break;
			default:
				phpserialize_array_destroy( &array );
				return NULL;
				break;
		}
		i++;
	}
	if( *str_ptr++ != '}' ) {
		return NULL;
	}
	(*next) = str_ptr;

	
	return array;
}


static int phpserialize_obj2ser_str( const serialize_t *ser, char *result, int size ) {
	char *result_ptr = (char*)result;
	int   len = strlen( ser->data.str );
	int   i;
	int   reserv = 0;

	if( ( result == NULL ) && ( ser->type != _STR ) ) {
		return -1;
	}
	for( i = 0; i < len; i++ ) {
		if( ( ser->data.str[i] == '\\' ) || ( ser->data.str[i] == '"' ) ) {
			reserv++;
		}
	}
	if( size < ( 15 + len + reserv ) ) {
		return -1;
	}
	result_ptr += sprintf( result, "s:%d:\\\"", len );
	for( i = 0; i < len; i++ ) {
		if( ser->data.str[i] == '\\' ) {
			*result_ptr++ = '\\';
			*result_ptr++ = '\\';
			continue;
		}
		if( ser->data.str[i] == '"' ) {
			*result_ptr++ = '\\';
			*result_ptr++ = '"';
			continue;
		}
		*result_ptr++ = ser->data.str[i];
	}
	sprintf( result_ptr, "\\\";" );

	return strlen( result );
}


static int phpserialize_obj2ser_int( const serialize_t *ser, char *result ) {
	if( ( result == NULL ) && ( ser->type != _INT ) ) {
		return -1;
	}
	sprintf( result, "i:%d;", ser->data.dig );

	return strlen( result );
}


static int phpserialize_obj2ser_bool( const serialize_t *ser, char *result ) {
	if( ( result == NULL ) && ( ser->type != _BOOL ) && ( ser->data.dig > 1 ) && ( ser->data.dig < 0 ) ) {
		return -1;
	}
	sprintf( result, "b:%d;", ser->data.dig );

	return 4;
}


static int phpserialize_obj2ser_array( const serialize_t *ser, char *result ) {
	char        *result_ptr = (char*)result;
	int          i;
	int          res;
	serialize_t  ser_name;

	if( ( result == NULL ) && ( ser->type != _ARRAY ) ) {
		return -1;
	}

	result_ptr += sprintf( result, "a:%d:{", ser->data.array->count );
	
	for( i = 0; i < ser->data.array->count; i++ ) {
		ser_name.data = ser->data.array->elements[i].name;
		ser_name.type = ser->data.array->elements[i].name_type;
		switch( ser->data.array->elements[i].name_type ) {
			case _INT:
				res = phpserialize_obj2ser_int( &ser_name, result_ptr );
				break;
			case _STR:
				res = phpserialize_obj2ser_str( &ser_name, result_ptr, 4096 );
				break;
			default:
				return -1;
		}
		if( res == -1 ) {
			return -1;
		}
		result_ptr += res;
		switch( ser->data.array->elements[i].type ) {
			case _INT:
				res = phpserialize_obj2ser_int( (serialize_t*)&ser->data.array->elements[i], result_ptr );
				break;
			case _STR:
				res = phpserialize_obj2ser_str( (serialize_t*)&ser->data.array->elements[i], result_ptr, 4096 );
				break;
			case _BOOL:
				res = phpserialize_obj2ser_bool( (serialize_t*)&ser->data.array->elements[i], result_ptr );
				break;
			case _ARRAY:
				res = phpserialize_obj2ser_array( (serialize_t*)&ser->data.array->elements[i], result_ptr );
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


phpserialize_array_t *phpserialize_array_create( int reserved ) {
	phpserialize_array_t *array;

	array = calloc( sizeof( phpserialize_array_t ), 1 );
	if( array == NULL ) {
		return NULL;
	}
	memset( array, 0, sizeof( phpserialize_array_t ) );
	array->reserved = reserved;
	array->count = 0;
	array->elements = calloc( sizeof( element_t ), reserved );
	return array;
}


element_t *phpserialize_array_push( phpserialize_array_t *array ) {
	element_t *element;
	
	if( array->reserved == array->count ) {
	//	element = calloc( sizeof( element_t ), array->reserved + 1 );
	//	memset( array->elements, 0, sizeof( element_t ) * ( array->reserved + 1 ) );
	//	memcpy( element, array->elements, sizeof( element_t ) * array->reserved );
	//	free( array->elements );
	//	array->reserved++;
	//	array->elements = element;
	//	element = &array->elements[array->count];
		return NULL;
	}
	element = &array->elements[array->count];
	array->count++;
	
	return element;
}


void phpserialize_array_destroy( phpserialize_array_t **array ) {
	int i;
	for( i = 0; i < (*array)->count; i++ ) {
		if( (*array)->elements[i].type = _ARRAY ) {
			phpserialize_array_destroy( &(*array)->elements[i].data.array );
			continue;
		}
		if( (*array)->elements[i].type = _STR ) {
			free( (*array)->elements[i].data.str );
			continue;
		}
	}
	free( (*array)->elements );
	free( *array );
}


static session_t *phpserialize_session_create( int count ) {
	session_t  *session;
	int         i;

	session = calloc( sizeof( session_t ), 1 );
	session->count = count;
	session->data = calloc( sizeof( serialize_t* ), count );
	session->name = calloc( sizeof( char* ), count );
	for( i = 0; i < count; i++ ) {
		session->data[i] = calloc( sizeof( serialize_t ), 1 );
	}

	return session;
}

void phpserialize_session_destroy( session_t **session ) {
	int i;

	for( i = 0; i < (*session)->count; i++ ) {
		if( (*session)->name[i] != NULL ) {
			free( (*session)->name[i] );
		}
		else {
			continue;
		}
		if( (*session)->data[i]->type == _STR ) {
			free( (*session)->data[i]->data.str );
			continue;
		}
		if( (*session)->data[i]->type == _ARRAY ) {
			phpserialize_array_destroy( &(*session)->data[i]->data.array );
			continue;
		}
	}
	free( (*session) );
}

session_t *phpserialize_session_decode( const char *session_str ) {
	int          count = -1;
	int          len;
	int          i;
	type_t       type;
	char        *session_str_ptr = (char*)session_str;
	session_t   *session;
	
	if( session_str == NULL ) {
		return NULL;
	}
	
	do {
		session_str_ptr = strchr( session_str_ptr, '|' );
		if( session_str_ptr != NULL ) {
			session_str_ptr++;
		}
		count++;
	} while( session_str_ptr != NULL);
	
	if( count == 0 ) {
		return NULL;
	}
	
	session = phpserialize_session_create( count );
	
	
	session_str_ptr = (char*)session_str;
	
	for( i = 0; i < count; i++ ) {
		/* set element name of session */
		session_str = strchr( session_str_ptr, '|' );
		len = session_str - session_str_ptr;
		session->name[i] = calloc( sizeof( char ), len + 1 );
		strncpy( session->name[i], session_str_ptr, len );

		/* set element serialize of session */
		session_str++;
		type = *session_str++;
		session->data[i]->type = type;
		if( *session_str++ != ':' ) {
			phpserialize_session_destroy( &session );
			return NULL;
		}
		switch( type ) {
			case _STR:
				session->data[i]->data.str = phpserialize_ser2obj_str( session_str, &session_str_ptr );
				if( session->data[i]->data.str == NULL ) {
					phpserialize_session_destroy( &session );
					return NULL;
				}
				break;
			case _INT:
				session->data[i]->data.dig = phpserialize_ser2obj_int( session_str, &session_str_ptr );
				if( session->data[i]->data.dig == -1 ) {
					phpserialize_session_destroy( &session );
					return NULL;
				}
				break;
			case _ARRAY:
				session->data[i]->data.array = phpserialize_ser2obj_array( session_str, &session_str_ptr );
				if( session->data[i]->data.array == NULL ) {
					phpserialize_session_destroy( &session );
					return NULL;
				}
				break;
			case _BOOL:
				session->data[i]->data.dig = phpserialize_ser2obj_bool( session_str, &session_str_ptr );
				if( session->data[i]->data.dig == -1 ) {
					phpserialize_session_destroy( &session );
					return NULL;
				}
				break;
			default:
				phpserialize_session_destroy( &session );
				return NULL;
				break;
		}
	}
	
	return session;
}

char *phpserialize_session_encode( session_t *session ) {
	char *str_session;
	char *str_session_ptr;
	int   i;
	int   res;
	
	if( session == NULL ) {
		return NULL;
	}
	str_session = calloc( sizeof( char ), 4096 );
	str_session_ptr = str_session;
	for( i = 0; i < session->count; i++ ) {
		str_session_ptr += sprintf( str_session_ptr, "%s|", session->name[i] );
		switch( session->data[i]->type ) {
			case _INT:
				res = phpserialize_obj2ser_int( session->data[i], str_session_ptr );
				break;
			case _STR:
				res = phpserialize_obj2ser_str( session->data[i], str_session_ptr, 4096 );
				break;
			case _BOOL:
				res = phpserialize_obj2ser_bool( session->data[i], str_session_ptr );
				break;
			case _ARRAY:
				res = phpserialize_obj2ser_array( session->data[i], str_session_ptr );
				break;
			default:
				free( str_session );
				return NULL;
		}
		if( res == -1 ) {
			free( str_session );
			return NULL;
		}
		str_session_ptr += res;
	}
	
	return str_session;
}
