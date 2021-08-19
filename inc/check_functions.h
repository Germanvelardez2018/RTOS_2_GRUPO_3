/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/07/09
 *
 *===========================================================================*/

#ifndef RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_CHECK_FUNCTIONS_H_
#define RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_CHECK_FUNCTIONS_H_


/*En este modulo estan todas las funciones que sirver para:
 * 		verificar CRC
 * 		verificar caracteres validos
 * 		generar nuevos CRC
 *
 *
 *
 * */

#include "driver.h"
#include "error_msg.h"

/*==================================Declaracion Defines============================*/




/*================================Funciones publicas==============================*/


errorCodes_t check_block(char* block);

bool check_CRC(char* block);






#endif /* RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_CHECK_FUNCTIONS_H_ */
