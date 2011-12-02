#ifndef PHPSERIALIZE_SESSION_H
#define PHPSERIALIZE_SESSION_H

#include "phpserialize_types.h"

phpserialize_session_t *phpserialize_session_create( int count );

phpserialize_session_t *phpserialize_session_decode( const char *session );

char *phpserialize_session_encode( phpserialize_session_t *session );

void phpserialize_session_destroy( phpserialize_session_t **session );

#endif /* PHPSERIALIZE_SESSION_H */
