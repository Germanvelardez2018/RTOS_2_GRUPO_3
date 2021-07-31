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

static void set_format(char format, char *block);

static char mayus_to_min(char mayus);
static char min_to_mayus(char min);

static char mayus_to_min(char mayus)
{

	char min;

	if (isupper(mayus))
	{
		min = mayus - 'A' + 'a';
	}
	else
	{
		return mayus;
	}

	return min;
}

static char min_to_mayus(char min)
{

	char mayus;

	if (islower(min))
	{
		mayus = min - 'a' + 'A';
	}
	else
	{
		return min;
	}

	return mayus;
}

void change_format(char *block)
{

	// el byte que indica el formato que debe tener la salida
	char C = block[4];

	set_format(C, block);
	//
}

static void set_format(char format, char *block)
{

	//el string que necesitamos modificar empieza en pos=5
	uint8_t pos_init = 5;

	//el mensaje termina dos bytes antes del valor de len

	uint8_t pos_end = strlen(block) - 2;

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
}

/*Funciones privadas*/

/*@param block: el bloque del mensaje.
 *
 *
 * */
static void set_pascal(char *block, uint8_t pos_init, uint8_t pos_end)
{
	uint8_t pos_prim = pos_init;
	uint8_t pos_sec = pos_init;

	for (; pos_prim < pos_end; pos_prim++)
	{
		if (block[pos_prim] == ' ' || block[pos_prim] == '_')
		{
			pos_prim++;
			block[pos_sec] = min_to_mayus(block[pos_prim]);
		}
		else if (pos_prim == 1)
		{
			block[pos_sec] = min_to_mayus(block[pos_prim]);
		}
		pos_sec++;
	}
	block[pos_sec] = '\0'; //Se cierra el nuevo string
}

static void set_camel(char *block, uint8_t pos_init, uint8_t pos_end)
{
	uint8_t pos_prim = pos_init;
	uint8_t pos_sec = pos_init;

	for (; pos_prim < pos_end; pos_prim++)
	{
		if (block[pos_prim] == ' ' || block[pos_prim] == '_')
		{
			pos_prim++;
			block[pos_sec] = min_to_mayus(block[pos_prim]);
		}
		else if (pos_prim == 1)
		{
			block[pos_sec] = mayus_to_min(block[pos_prim]);
		}
		pos_sec++;
	}
	block[pos_sec] = '\0'; //Se cierra el nuevo string
}

static void set_snake(char *block, uint8_t pos_init, uint8_t pos_end)
{
	uint8_t pos_prim = pos_init;
	uint8_t pos_sec = pos_init;

	for (; pos_prim < pos_end; pos_prim++)
	{
		if (block[pos_prim] == ' ')
		{
			block[pos_sec] = '_';
		}
		else if (isupper(block[pos_prim]))
		{
			block[pos_sec] = mayus_to_min(block[pos_prim]);
		}else {
			block[pos_sec] = mayus_to_min(block[pos_sec]);
		}
		pos_sec++;
	}
	block[pos_sec] = '\0'; //Se cierra el nuevo string

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
