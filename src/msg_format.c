/*
 * msg_format.c
 *
 *  Created on: 28 jul. 2021
 *      Author: gvelardez
 */



#include "msg_format.h"

#include "sapi.h"


/*   Formato del mensaje
 *
 *  (  SEC[4BYTES]| C[1BYTE] | DATOS[NBYTES] | CRC[2BYTES] )
 *
 * */

static char detect_format(char* block);


static void set_pascal(char* block,uint8_t pos_init, uint8_t pos_end);


static void set_camel(char* block,uint8_t pos_init, uint8_t pos_end);


static void set_snake(char* block,uint8_t pos_init, uint8_t pos_end);




static void set_format(char format,char* block);

void change_format(char* block)
{

	// el byte que indica el formato que debe tener la salida
	char C = block[4];

	//



}


static void set_format(char format,char* block)
{

	//el string que necesitamos modificar empieza en pos=5
	uint8_t pos_init= 5;

	//el mensaje termina dos bytes antes del valor de len

	uint8_t pos_end = strlen(block)-2;

	switch(format)
	{


	case FPASCAL:
				set_pascal(block,pos_init,pos_end );
				break;
	case FCAMEL:
				set_camel(block,pos_init,pos_end);
				break;
	case FSNAKE:
				set_snake(block,pos_init,pos_end);
				break;

	default:
				break;

	}

}




/*Funciones privadas*/

/*@param block: el bloque del mensaje.
 *
 *
 * */
static void set_pascal(char* block,uint8_t pos_init, uint8_t pos_end)
{



}


static void set_camel(char* block,uint8_t pos_init, uint8_t pos_end)
{
	//el string que necesitamos modificar empieza en pos=5

}


static void set_snake(char* block,uint8_t pos_init, uint8_t pos_end)
{
	//el string que necesitamos modificar empieza en pos=5


}
/* si todos los caracteres del mensaje son minusculas o '_', entonces el mensaje es snake_case
 * si el primer caracter del mensaje es Mayuscula, entonces es PascalCase
 * por descarte si no es PascalCase y no es snake_case es camelCase
 *
 *  A = 65        a = 97
 *  B = 66		  b = 98  y sigue
 *  el ' _ ' es 95
 * */




static bool isPascal(char* block,uint8_t pos_init)
{


	//si se cumple esta condiciones el primer elemento es mayuscula, entonces es pascalcase
	bool res = (block[pos_init]<= 97)?true:false;


	return res;
}
static bool isSnake(char* block,uint8_t pos_init, uint8_t pos_end)
{
	for(uint8_t pos = pos_init; pos< pos_end; pos ++)
	{
		if(block[pos] == '_')
		{
			return true;
		}
	}
	return false;
}


static char detect_format(char* block)
{
	/*COmpletar*/


	return 'c';
}
