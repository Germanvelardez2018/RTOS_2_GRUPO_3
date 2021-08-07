/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/08/06
 *
 *===========================================================================*/

#ifndef RTOS2_REPOSITORIO_OFICIAL_GIT_RESUELTO_RTOS_2_GRUPO_3_INC_ERROR_H_
#define RTOS2_REPOSITORIO_OFICIAL_GIT_RESUELTO_RTOS_2_GRUPO_3_INC_ERROR_H_


#define _INT_TO_CHAR(x)       (char)((char)x + '0')
#define OFFSET_MSG		    	5





typedef enum
{
	ERROR_INVALID_DATA,
	ERROR_INVALID_OPCODE,
	ERROR_SYSTEM,
	ERROR_CRC,
	ERROR_SECUENCE,
	BLOCK_OK
}errorCodes_t;

#endif /* RTOS2_REPOSITORIO_OFICIAL_GIT_RESUELTO_RTOS_2_GRUPO_3_INC_ERROR_H_ */
