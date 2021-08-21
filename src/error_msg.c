/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/07/09
 *
 *===========================================================================*/


#include "error_msg.h"
#include "crc8.h"


/*===========================Funciones publicas==============================*/



void insert_error(char* block,errorCodes_t type)
{
	block[OFFSET_MSG]='E';
	int_to_ASCII((uint8_t)type, (block + OFFSET_MSG + 1));
	block[OFFSET_MSG+3]='\0';

}


