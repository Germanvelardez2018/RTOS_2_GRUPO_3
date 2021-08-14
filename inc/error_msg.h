/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/08/06
 *
 *===========================================================================*/

#ifndef RTOS2_REPOSITORIO_OFICIAL_GIT_RESUELTO_RTOS_2_GRUPO_3_INC_ERROR_MSG_H_
#define RTOS2_REPOSITORIO_OFICIAL_GIT_RESUELTO_RTOS_2_GRUPO_3_INC_ERROR_MSG_H_


/*=========================Declaracion de definiciones=======================*/

#define _INT_TO_CHAR(x)       (char)((char)x + '0')
#define OFFSET_MSG		    	4


/*=====================Declaracion de tipos de datos=========================*/


typedef enum
{
	ERROR_INVALID_OPCODE,
	ERROR_INVALID_DATA,
	ERROR_SYSTEM,
	ERROR_CRC,
	ERROR_SECUENCE,
	BLOCK_OK
}errorCodes_t;


/*=========================Declaracion de funciones publicas========================*/

void  insert_error_msg_0(char* b);

void  insert_error_msg_1(char* b);




#endif /* RTOS2_REPOSITORIO_OFICIAL_GIT_RESUELTO_RTOS_2_GRUPO_3_INC_ERROR_MSG_H_ */




