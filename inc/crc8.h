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


/*=============================Declaracion Funciones Publicas================*/

/*
 * Funcion para pasar dos char que representan a un numero en hexa
 * a un valor entero.
 */
uint8_t ASCII_to_int(char* stringCRC);
/*
 * Funcion para pasar un entero de dos caracteres a ASCII
 */
void  int_to_ASCII(uint8_t intCRC,char* stringCRC);


uint8_t crc8_init(void);

/*
 * Funcion para calcular el crc8
 */
uint8_t crc8_calc(uint8_t val, void *buf, int cnt);






#endif /* RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_CRC8_H_ */
