#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "phpserialize.h"


int main() {
	char string[] = "string|s:22:\\\"\\\\`1234567890-=+\\\"_\\\\/*a'\\\";digit|i:1317209150;boolean|b:1;array1|a:2:{i:1;s:3:\\\"qqq\\\";s:5:\\\"test2\\\";i:222;}array2|a:2:{i:0;s:3:\\\"qqq\\\";i:1;i:222;}";
	session_t *test;
	char *res;
	
	printf( "%s\n", string );
	test = phpserialize_session_decode( string );
	if( test == NULL ) {
		printf( "FAIL\n" );
		return 0;
	}
	else {
		printf( "GOOD\n" );
		res = phpserialize_session_encode( test );
	}
	if( res == NULL ) {
		printf( "FAIL\n" );
		return 0;
	}
	else {
		printf( "GOOD\n" );
		printf( "%s\n", res );
	}
}
