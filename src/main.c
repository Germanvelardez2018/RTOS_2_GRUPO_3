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

#include "driver.h"




void led_task(void* params)
{

	while(1)
	{
		gpioToggle(LED3);
		vTaskDelay( 200 );

	}

}


int main(void)
{

	/*Se define la uart a utilizar*/

//	static protocolData_t uart_ONE = { .uart = UART_USB, .baudRate = 115200, .index=0 };


	/* Inicializar la placa */

	boardConfig();

	/*Iniciamos el modulo protocol*/


	static driver_t driver={.uart = UART_USB,
							.baudrate = 115200
							} ;

	driver_init(&driver);


	xTaskCreate(driver_manager,                  // Funcion de la tarea a ejecutar
					(const char *) "wait_frame", // Nombre de la tarea como String amigable para el usuario
					configMINIMAL_STACK_SIZE * 4,   // Cantidad de stack de la tarea
					&driver,                    // Parametros de tarea
					tskIDLE_PRIORITY + 1,           // Prioridad de la tarea
					0                         // Puntero a la tarea creada en el sistema
	);


	xTaskCreate(led_task,                  // Funcion de la tarea a ejecutar
					(const char *) "led", // Nombre de la tarea como String amigable para el usuario
					configMINIMAL_STACK_SIZE * 4,   // Cantidad de stack de la tarea
					0,                    // Parametros de tarea
					tskIDLE_PRIORITY + 1,           // Prioridad de la tarea
					0                         // Puntero a la tarea creada en el sistema
	);


//	init_protocol(&uart_ONE);



	vTaskStartScheduler();

	return 0;
}
