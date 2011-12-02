#ifndef PHPSERIALIZE_SERIALIZE_H
#define PHPSERIALIZE_SERIALIZE_H

#include "phpserialize_types.h"

phpserialize_var_t *phpserialize_serialize_decode( const char *serialize, char **end, int flags );

int phpserialize_serialize_encode( phpserialize_var_t *unserialize, char *serialize );

void phpserialize_serialize_destroy( phpserialize_var_t **unserialize );

#endif /* PHPSERIALIZE_SERIALIZE_H */
