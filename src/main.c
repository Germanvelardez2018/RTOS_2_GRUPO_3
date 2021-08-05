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

//#include "semphr.h"
//#include "protocol.h"

#include "driver.h"

/*==================================Declaracion Defines============================*/

#define UART_BAUDRATE 115200


/*===============================Programa Principal=============================*/


int main(void)
{

	/* Inicializar la placa */

	boardConfig();

	/*Iniciamos el modulo protocol*/


	static driver_t driver={.uart = UART_USB,
							.baudrate = UART_BAUDRATE
							} ;

	driver_init(&driver);


	xTaskCreate(driver_task,				// Funcion de la tarea a ejecutar
			(const char *) "wait_frame", 	// Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE * 4,   // Cantidad de stack de la tarea
			&driver,                    	// Parametros de tarea
			tskIDLE_PRIORITY + 1,           // Prioridad de la tarea
			0);                         	// Puntero a la tarea creada en el sistema



	xTaskCreate(led_task,					// Funcion de la tarea a ejecutar
			(const char *) "led", 			// Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE * 4,   // Cantidad de stack de la tarea
			0,                    			// Parametros de tarea
			tskIDLE_PRIORITY + 1,           // Prioridad de la tarea
			0);                        		// Puntero a la tarea creada en el sistema




	vTaskStartScheduler();

	return 0;
}
