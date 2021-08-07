/*
 * msg_format.c
 *
 *  Created on: 28 jul. 2021
 *      Author: gvelardez
 */

#include "msg_format.h"
#include <string.h>
#include "sapi.h"
#include "ctype.h"

/*   Formato del mensaje
 *
 *  (  SEC[4BYTES]| C[1BYTE] | DATOS[NBYTES] | CRC[2BYTES] )
 *
 * */

static char detect_format(char *block, uint8_t pos_init, uint8_t pos_end);

static void set_pascal(char *block, uint8_t pos_init, uint8_t pos_end);

static void set_camel(char *block, uint8_t pos_init, uint8_t pos_end);

static void set_snake(char *block, uint8_t pos_init, uint8_t pos_end);


void change_format(char *block)
{

	
	char format = block[FORMAT_DESIGNATOR_POSITION];	// el byte que indica el formato que debe tener la salida

	uint8_t pos_init = START_DATA_POSITION;		//Posicion inicial del mensaje a cambiarle el formato

	uint8_t pos_end = strlen(block) - CRC_SIZE;		//Posicion final del mensaje a cambiarle el formato

	switch (format)
	{

	case FPASCAL:
		set_pascal(block, pos_init, pos_end);
		break;
	case FCAMEL:
		set_camel(block, pos_init, pos_end);
		break;
	case FSNAKE:


		set_snake(block, pos_init, pos_end);
		break;

	default:
		//formato invalido de C
		break;
	}

	//
}

/*Funciones privadas*/

/*@param block: el bloque del mensaje.
 *
 *
 * */

static void set_pascal(char *block, uint8_t pos_init, uint8_t pos_end)
{
	uint8_t pos_read = pos_init;		//Posicion de lectura
	uint8_t pos_write = pos_init;			//Posicion de escritura

	for (; pos_read < pos_end; pos_read++)
	{
		if (block[pos_read] == ' ' || block[pos_read] == '_')
		{
			pos_read++;
			block[pos_write] = toupper(block[pos_read]);
		}
		else if (pos_read == pos_init)
		{
			block[pos_write] = toupper(block[pos_read]);
			}
		else{
			block[pos_write] = block[pos_read];
		}
		pos_write++;
	}
	block[pos_write] = '\0'; //Se cierra el nuevo string
}



static void set_camel(char *block, uint8_t pos_init, uint8_t pos_end)
{
	uint8_t pos_read = pos_init;		//Posicion de lectura
	uint8_t pos_write = pos_init;			//Posicion de escritura

	for (; pos_read < pos_end; pos_read++)
	{
		if (block[pos_read] == ' ' || block[pos_read] == '_')
		{
			pos_read++;
			block[pos_write] = toupper(block[pos_read]);
		}
		else if (pos_read == pos_init)
		{
			block[pos_write] = tolower(block[pos_read]);
		}
		else{
			block[pos_write] = block[pos_read];
		}
		pos_write++;
	}
	block[pos_write] = '\0'; //Se cierra el nuevo string
}

static void set_snake(char *block, uint8_t pos_init, uint8_t pos_end)
{
	char	aux_block[FRAME_MAX_SIZE];
	uint8_t pos_read = pos_init;		//Posicion de lectura
	uint8_t pos_write = pos_init;			//Posicion de escritura

	strcpy (aux_block,block);		//Se deja una copia del string original para poder realizar la operacion

	for (; pos_read < pos_end; pos_read++)
	{
		if (aux_block[pos_read] == ' ')
		{
			block[pos_write] = '_';
		}
		else if (isupper(aux_block[pos_read]) && (pos_read != pos_init))
		{
			block[pos_write] = '_';
			pos_write++;
			block[pos_write] = tolower(aux_block[pos_read]);
		}
		else {
			block[pos_write] = tolower(aux_block[pos_read]);
		}
		pos_write++;
	}
	block[pos_write] = '\0'; //Se cierra el nuevo string

}
/* si todos los caracteres del mensaje son minusculas o '_', entonces el mensaje es snake_case
 * si el primer caracter del mensaje es Mayuscula, entonces es PascalCase
 * por descarte si no es PascalCase y no es snake_case, entonces es camelCase
 *
 *  A = 65        a = 97
 *  B = 66		  b = 98  y sigue
 *  el ' _ ' es 95
 * */

static bool isPascal(char *block, uint8_t pos_init)
{

	//si se cumple esta condiciones el primer elemento es mayuscula, entonces es pascalcase
	bool res = (block[pos_init] <= 97) ? true : false;

	return res;
}

static bool isSnake(char *block, uint8_t pos_init, uint8_t pos_end)
{
	for (uint8_t pos = pos_init; pos < pos_end; pos++)
	{
		if (block[pos] == '_')
		{
			return true;
		}
	}
	return false;
}
/*
 * Detecta el formato del mensaje:
 *
 * return : FCAMEL     'C'
 * 			FPASCAL	   'P'
 * 			FSNAKE     'S'
 * */

static char detect_format(char *block, uint8_t pos_init, uint8_t pos_end)
{
	char format = FCAMEL; //es CAMELCASE por descarte

	if (isPascal(block, pos_init))
	{
		format = FPASCAL;
	}
	else
	{
		if (isSnake(block, pos_init, pos_end))
		{
			format = FSNAKE;
		}
	}

	return format;
}
