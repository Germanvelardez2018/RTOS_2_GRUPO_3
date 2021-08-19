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
#include "error_msg.h"
#include "msg_format.h"
#include "crc8.h"

#include <ctype.h>

/*===============================Declaracion de definiciones=======================*/

#define SEC_END_POS 	3
#define OPCODE_POS 		4
#define DATA_START_POS 	5
#define SEC_LENGTH 		4
#define CRC_LENGTH		2
#define OPCODE_LENGTH	1
#define MAX_LETTERS		10
#define MAX_WORDS		15

/*============================Declaracion de funciones privadas====================*/

static bool check_secuence(char* block);

static bool check_msg(char* block);

static bool check_opcode(char* block);

/*================================Funciones publicas==============================*/


/*Funcion que realiza los chequeos del bloque antes de pasarlo para darle formato*/
errorCodes_t check_block(char* block)
{
	if(	!check_secuence(block))
	{
		return ERROR_INVALID_DATA;//ERROR_SECUENCE;
	}

/*	if(!check_CRC(block))
	{
		return ERROR_INVALID_DATA ;//ERROR_CRC
	}*/

   if(!check_msg( block))
    {
	   return ERROR_INVALID_DATA;
	}

   if(!check_opcode(block))
   {
	   return ERROR_INVALID_OPCODE;
   }

 return	BLOCK_OK;
}

bool check_CRC(char* block)
{
	bool res = true;
	int8_t size = strlen(block)  ;

	/*Se extrae el crc del mensaje y se pasa a un entero*/
	char CRC[2];
	CRC[0]=block[size-2];
	CRC[1] = block [size -1];



	int crcMensaje = 0;
	crcMensaje = ASCII_to_int(CRC);
	/*Se calcula el crc*/
	uint8_t crcCalculado = crc8_calc(0,block,size-2);

	if( crcMensaje != crcCalculado)
	{
		res = false;
	}

	return res;

}

/*================================Funciones privadas================================*/

/*Secuencia debe ser numero*/
static bool check_secuence(char* block)
{
	bool res = true;  // por defecto

	char secuence[SEC_LENGTH + 1];

	strncpy(secuence,block,SEC_LENGTH);
	secuence[SEC_LENGTH]='\0';


	//si secuence no es numero atoi devolvera cero
	int num = atoi(secuence);

	if(num == 0) //significa que atoi fallo o la secuencia es 0000?
	{
		// verificar secuencia valida 0000

			for(int i=0 ; i < SEC_LENGTH ;i++)
		{
			if(secuence[i] !='0')
			{
				return false;
			}
		}

	}

	return res;
}






static bool check_msg(char* block)
{
	uint8_t words = 0;
	uint8_t letters = 0;
	uint8_t length = strlen(block);
	uint8_t i;


	if(length <= (SEC_LENGTH + OPCODE_LENGTH + CRC_LENGTH))
	{
		return FALSE;
	}


	for (i=(OPCODE_POS + 1); i < (length - CRC_LENGTH); i++)
	{
		if (islower(block[i]))
		{
			letters++;
		}else if (isupper(block[i]))
		{
			if (letters == 0)
			{
				letters++;
			}else
			{
				words++;
			}
		}else if(block[i] == '_' || block[i] == ' ')
		{
			if(letters > 0 && letters <= MAX_LETTERS)
			{
				words++;
				letters = 0;
				if (words > MAX_WORDS)
				{
					return FALSE;
				}
			}else
			{
				return FALSE;
			}
		}else
		{
			return FALSE;
		}
	}
	if (letters == 0)
	{
		return FALSE;
	}
    return TRUE;

}

static bool check_opcode(char* block)
{
	if(block[OPCODE_POS] == FSNAKE)
	{
		return TRUE;
	}
	if(block[OPCODE_POS] == FCAMEL)
	{
		return TRUE;
	}
	if(block[OPCODE_POS] == FPASCAL)
	{
		return TRUE;
	}
	return FALSE;
}

