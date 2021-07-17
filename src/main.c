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





int main(void) {

	/*Se define la uart a utilizar*/

	static protocolData_t uart_ONE = { .uart = UART_USB, .baudRate = 115200, .index=0 };


	/* Inicializar la placa */

	boardConfig();

	/*Funcion para inicializar el pool de memoria*/
	poolInit();

	/*Se crea la tarea para recibir tramas (se le debe pasar por parametro la uartConfig_t a utilizar)*/

	xTaskCreate(wait_frame,                  // Funcion de la tarea a ejecutar
			(const char *) "wait_frame", // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE * 2,   // Cantidad de stack de la tarea
			&uart_ONE,                    // Parametros de tarea
			tskIDLE_PRIORITY + 1,           // Prioridad de la tarea
			0                         // Puntero a la tarea creada en el sistema
			);

	vTaskStartScheduler();

	return 0;
}
