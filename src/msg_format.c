/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/07/09
 *
 *===========================================================================*/

#include "msg_format.h"
#include <string.h>
#include "sapi.h"
#include "ctype.h"

/*   Formato del mensaje
 *
 *  (  SEC[4BYTES]| C[1BYTE] | DATOS[NBYTES] | CRC[2BYTES] )
 *
 * */


/*============================Declaracion de funciones privadas====================*/




/*================================Funciones publicas===============================*/


 void set_pascal(char *block)
{
	uint8_t pos_end = strlen(block); //Posicion final del mensaje a cambiarle el formato
	uint8_t pos_read = START_DATA_POSITION;  //Posicion de lectura
	uint8_t pos_write = START_DATA_POSITION; //Posicion de escritura

	for (; pos_read < pos_end; pos_read++)
	{
		if (block[pos_read] == ' ' || block[pos_read] == '_')
		{
			pos_read++;
			block[pos_write] = toupper(block[pos_read]);
		}
		else if (pos_read == START_DATA_POSITION)
		{
			block[pos_write] = toupper(block[pos_read]);
		}
		else
		{
			block[pos_write] = block[pos_read];
		}
		pos_write++;
	}
	block[pos_write] = '\0'; //Se cierra el nuevo string
}

 void set_camel(char *block)
{
	uint8_t pos_end = strlen(block); //Posicion final del mensaje a cambiarle el formato
	uint8_t pos_read = START_DATA_POSITION;  //Posicion de lectura
	uint8_t pos_write = START_DATA_POSITION; //Posicion de escritura

	for (; pos_read < pos_end; pos_read++)
	{
		if (block[pos_read] == ' ' || block[pos_read] == '_')
		{
			pos_read++;
			block[pos_write] = toupper(block[pos_read]);
		}
		else if (pos_read == START_DATA_POSITION)
		{
			block[pos_write] = tolower(block[pos_read]);
		}
		else
		{
			block[pos_write] = block[pos_read];
		}
		pos_write++;
	}
	block[pos_write] = '\0'; //Se cierra el nuevo string
}

 void set_snake(char *block)
{
	char aux_block[FRAME_MAX_SIZE];
	uint8_t pos_end = strlen(block); //Posicion final del mensaje a cambiarle el formato
	uint8_t pos_read = START_DATA_POSITION;  //Posicion de lectura
	uint8_t pos_write = START_DATA_POSITION; //Posicion de escritura

	strcpy(aux_block, block); //Se deja una copia del string original para poder realizar la operacion

	for (; pos_read < pos_end; pos_read++)
	{
		if (aux_block[pos_read] == ' ')
		{
			block[pos_write] = '_';
		}
		else if (isupper(aux_block[pos_read]) && (pos_read != START_DATA_POSITION))
		{
			block[pos_write] = '_';
			pos_write++;
			block[pos_write] = tolower(aux_block[pos_read]);
		}
		else
		{
			block[pos_write] = tolower(aux_block[pos_read]);
		}
		pos_write++;
	}
	block[pos_write] = '\0'; //Se cierra el nuevo string
}

