#ifndef PHPSERIALIZE_ARRAY_H
#define PHPSERIALIZE_ARRAY_H

#include "phpserialize_types.h"

phpserialize_array_t *phpserialize_array_create( int reserved );

int phpserialize_array_push( phpserialize_array_t *array );

void phpserialize_array_destroy( phpserialize_array_t **array );

#endif /* PHPSERIALIZE_ARRAY_H */
