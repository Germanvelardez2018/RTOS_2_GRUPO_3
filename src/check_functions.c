/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/07/09
 *
 *===========================================================================*/
#include <string.h>
#include <stdlib.h>
#include "check_functions.h"
#include "sapi.h"

#include "crc8.h"


/*============================Declaracion de funciones privadas====================*/

static bool check_secuence(char* block);

static bool check_CRC(char* block);

static bool check_msg(char* block);

/*================================Funciones publicas==============================*/

bool check_block(char* block)
{
	// checkear secuencia

	if(	!check_secuence( block))
	{
		return false;
	}

	//checkear formato de mensaje valido

	if(!check_CRC(block))
	{
		return false;
	}

	//checkear CRC
   if(check_msg( block))
    {
	   return false;
	}

 return	true;
}


/*================================Funciones privadas================================*/

/*Secuencia debe ser numero*/
static bool check_secuence(char* block)
{
	bool res = true;  // por defecto

	char secuence[5];

	strncpy(secuence,block,4);
	secuence[4]='\0';

	//si secuence no es numero atoi devolvera cero
	int num = atoi(secuence);

	if(num == 0) //significa que atoi fallo o la secuencia es 0000?
	{
		res=false;
	}

	return res;
}




static bool check_CRC(char* block)
{
	bool res = true;
	int8_t size = strlen(block);

	/*
	 *   secuencia|C| mensaje| crc
	 *
	 * */

	//estraigo el CRC del mensaje y lo convierto
	char CRC[2];
	CRC[0]=block[size-2];
	CRC[1] = block [size -1];
	int crc_mensaje = 0;
	crc_mensaje = ASCII_to_int(CRC);
	//calculo el CRC yo mismo
	int8_t crc_calculado = crc8_calc(0,block,size);
	if( crc_mensaje != crc_calculado)
	{
		res = false;
	}
	return res;

}

static bool check_msg(char* block)
{
	bool res = true;


	/*FALTA IMPLEMENTACION */
    return res;

}

