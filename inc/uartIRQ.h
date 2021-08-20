/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/07/09
 *
 *===========================================================================*/

#ifndef RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_UARTIRQ_H_
#define RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_UARTIRQ_H_


#include "driver.h"



/*Prototipos de funciones publicas*/


bool_t txInterruptEnable( driver_t* );
bool_t rxInterruptEnable( driver_t* );
void onTxTimeOutCallback( TimerHandle_t );
void onRxTimeOutCallback( TimerHandle_t );

void send_block(char *block, driver_t *driver);


void free_block (driver_t* driver,char* block);



#endif /* RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_UARTIRQ_H_ */
