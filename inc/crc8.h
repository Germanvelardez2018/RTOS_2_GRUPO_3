/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/07/09
 *
 *===========================================================================*/
#ifndef RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_CRC8_H_
#define RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_CRC8_H_



#include "sapi.h"




uint8_t ASCII_to_int(char* CRC8);

void  int_to_ASCII(uint8_t CRC8,char* crc8);


uint8_t calculate_crc8(char* buffer,uint8_t size);






#endif /* RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_CRC8_H_ */
