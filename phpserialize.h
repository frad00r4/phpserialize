#ifndef PHPSERIALIZE_H
#define PHPSERIALIZE_H

#define FALSE 0
#define TRUE  1

typedef enum {
	_INT =   'i',
	_FLOAD = 'd',
	_STR =   's',
	_ARRAY = 'a',
	_BOOL =  'b',
	_NULL =  'N',
	_OBJ =   'O'
} type_t;

typedef struct phpserialize_array_s phpserialize_array_t;

typedef union {
	int                  dig;
	char                *str;
	phpserialize_array_t *array;
} data_u;

typedef struct {
	data_u   data;
	type_t   type;
	data_u   name;
	type_t   name_type;
} element_t;
	
typedef struct {
	data_u   data;
	type_t   type;
} serialize_t;

typedef struct {
	char       **name;
	serialize_t **data;
	unsigned int  count;
} session_t;

struct phpserialize_array_s {
	element_t    *elements;
	unsigned int  count;
	unsigned int  reserved;
};


phpserialize_array_t *phpserialize_array_create( int reserved );
element_t *phpserialize_array_push( phpserialize_array_t *array );
void phpserialize_array_destroy( phpserialize_array_t **array );


session_t *phpserialize_session_decode( const char *session );
char *phpserialize_session_encode( session_t *session );
void phpserialize_session_destroy( session_t **session );


#endif /* PHPSERIALIZE_H */