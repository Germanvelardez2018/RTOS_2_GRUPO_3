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
			(const char *) "modulo driver", 	// Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE * 8,   // Cantidad de stack de la tarea
			&driver,                    	// Parametros de tarea
			tskIDLE_PRIORITY + 0,           // Prioridad de la tarea
			0);                         	// Puntero a la tarea creada en el sistema





	vTaskStartScheduler();

	return 0;
}
