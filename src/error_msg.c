/*
 * ao_error.c
 *
 *  Created on: 14 ago. 2021
 *      Author: gvelardez
 */


#include "error_msg.h"




void  insert_error_msg_0(char* b)
{


	b[OFFSET_MSG]='E';
	int_to_ASCII(0, (b + OFFSET_MSG + 1));
	b[OFFSET_MSG+3]='\0';

}

void  insert_error_msg_1(char* b)
{


	b[OFFSET_MSG]='E';
	int_to_ASCII(1, (b + OFFSET_MSG + 1));
	b[OFFSET_MSG+3]='\0';

}
