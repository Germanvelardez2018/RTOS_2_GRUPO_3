/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/07/09
 *
 *===========================================================================*/


#ifndef POSGRADO_RTOS_2_GRUPO_3_INC_MEMPOOL_H_
#define POSGRADO_RTOS_2_GRUPO_3_INC_MEMPOOL_H_

/*===============================[Inclusiones]===============================*/


#include "qmpool.h"
#include "sapi.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include <stdio.h>

/*=============================[Definiciones]================================*/

#define POOL_SIZE 1000 //tama�o del pool de memoria
#define BLOCK_SIZE 200   //tama�o de cada bloque

/*=========================[Definciones y Macros]============================*/

typedef char* mensaje_t;

/*====================[Declaracion de funciones publicas]====================*/

/*Funcion para inicializar el pool de memoria*/
void poolInit();

/*Funcion para guardar un mensaje en un bloque del pool. Si devuelve NULL
 * significa que no hay mas memoria.*/
mensaje_t guardarMensaje(mensaje_t mensaje);

void liberarMemoria(mensaje_t mensaje);





#endif /* POSGRADO_RTOS_2_GRUPO_3_INC_MEMPOOL_H_ */
