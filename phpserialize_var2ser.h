#ifndef PHPSERIALIZE_VAR2SER_H
#define PHPSERIALIZE_VAR2SER_H

#include "phpserialize_types.h"

int phpserialize_var2ser_bool( const phpserialize_var_t *ser, char *result );

int phpserialize_var2ser_int( const phpserialize_var_t *ser, char *result );

int phpserialize_var2ser_str( const phpserialize_var_t *ser, char *result, int size );

int phpserialize_var2ser_array( const phpserialize_var_t *ser, char *result );

#endif /* PHPSERIALIZE_VAR2SER_H */
