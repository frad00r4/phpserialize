#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "phpserialize_session.h"
#include "phpserialize_array.h"
#include "phpserialize_serialize.h"
#include "phpserialize_var2ser.h"


phpserialize_session_t *phpserialize_session_create( int count ) {
	phpserialize_session_t  *session;
	int         i;

	session = calloc( sizeof( phpserialize_session_t ), 1 );
	session->count = count;
	session->data = calloc( sizeof( phpserialize_var_t* ), count );
	session->name = calloc( sizeof( char* ), count );
	for( i = 0; i < count; i++ ) {
		session->data[i] = calloc( sizeof( phpserialize_var_t ), 1 );
	}

	return session;
}


void phpserialize_session_destroy( phpserialize_session_t **session ) {
	int i;

	for( i = 0; i < (*session)->count; i++ ) {
		if( (*session)->name[i] != NULL ) {
			free( (*session)->name[i] );
		}
		else {
			continue;
		}
		if( (*session)->data[i]->type == _STR ) {
			free( (*session)->data[i]->data.p_str );
			continue;
		}
		if( (*session)->data[i]->type == _ARRAY ) {
			phpserialize_array_destroy( &(*session)->data[i]->data.p_array );
			continue;
		}
	}
	free( (*session) );
}


phpserialize_session_t *phpserialize_session_decode( const char *session_str ) {
	int          count = -1;
	int          len;
	int          i;
	type_t       type;
	char        *session_str_ptr = (char*)session_str;
	phpserialize_session_t   *session;
	
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
		session->name[i] = calloc( len + 1, sizeof( char ) );
		strncpy( session->name[i], session_str_ptr, len );

		/* set element serialize of session */
		session_str++;
		session->data[i] = phpserialize_serialize_decode( session_str, &session_str_ptr, DATA );
		if( session->data[i] == NULL ) {
			phpserialize_session_destroy( &session );
			return NULL;
		}
	}
	return session;
}


char *phpserialize_session_encode( phpserialize_session_t *session ) {
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
		res = phpserialize_serialize_encode( session->data[i], str_session_ptr );
		if( res == -1 ) {
			free( str_session );
			return NULL;
		}
		str_session_ptr += res;
	}
	
	return str_session;
}
