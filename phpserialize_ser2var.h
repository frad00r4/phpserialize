#ifndef PHPSERIALIZE_SER2VAR_H
#define PHPSERIALIZE_SER2VAR_H

#include "phpserialize_types.h"

phpserialize_array_t *phpserialize_ser2var_array( const char *str, char **next );

int phpserialize_ser2var_bool( const char *str, char **next );

int phpserialize_ser2var_int( const char *str, char **next );

char *phpserialize_ser2var_str( const char *str, char **next );

#endif /* PHPSERIALIZE_SER2VAR_H */
