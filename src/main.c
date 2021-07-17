/*=============================================================================
 * Copyright (c) 2021
 * All rights reserved.
 * License: Free
 * Date: 2021/10/03
 * Version: v1.2
 *===========================================================================*/

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "task.h"
//#include "sapi.h"

#include "semphr.h"
#include "protocol.h"





int main(void)
{

	/*Se define la uart a utilizar*/

	static protocolData_t uart_ONE = { .uart = UART_USB, .baudRate = 115200, .index=0 };


	/* Inicializar la placa */

	boardConfig();

	/*Iniciamos el modulo protocol*/

	init_protocol(&uart_ONE);



	vTaskStartScheduler();

	return 0;
}
