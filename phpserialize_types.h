#ifndef PHPSERIALIZE_TYPES_H
#define PHPSERIALIZE_TYPES_H

#define FALSE 0
#define TRUE  1

#define E_INT   0x10000000
#define E_FLOAT 0x01000000
#define E_STR   0x00100000
#define E_ARRAY 0x00010000
#define E_BOOL  0x00001000
#define E_NULL  0x00000100
#define E_OBJ   0x00000010

#define NAME E_INT | E_STR
#define DATA E_INT | E_STR | E_ARRAY | E_BOOL

typedef enum {
	_INT   = 'i',
	_FLOAD = 'd',
	_STR   = 's',
	_ARRAY = 'a',
	_BOOL  = 'b',
	_NULL  = 'N',
	_OBJ   = 'O'
} type_t;

typedef struct phpserialize_array_s phpserialize_array_t;

typedef union {
	int                   p_int;
	float                 p_fload;
	char                 *p_str;
	phpserialize_array_t *p_array;
} data_u;

typedef struct {
	data_u data;
	type_t type;
} phpserialize_var_t;

typedef struct {
	char               **name;
	phpserialize_var_t **data;
	unsigned int         count;
} phpserialize_session_t;

struct phpserialize_array_s {
	phpserialize_var_t **elements;
	phpserialize_var_t **names;
	unsigned int         count;
	unsigned int         reserved;
};


#endif /* PHPSERIALIZE_TYPES_H */
