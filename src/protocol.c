/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: Free
 * Date: 2021/10/03
 * Version: v1.2
 *===========================================================================*/

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "protocol.h"
#include "semphr.h"
#include "task.h"


/*Capa 3 */
#include "m_c3.h"

SemaphoreHandle_t new_frame_signal;
SemaphoreHandle_t mutex;





/*@brief Inicia el modulo,configurando periferico y cargando la tarea
 * @param Se envia una estructura con parametros de configuracion
 *
 */
void init_protocol(protocolData_t* protocol_config)
{
	/*Funcion para inicializar el pool de memoria*/
		poolInit();

		/*Se crea la tarea para recibir tramas (se le debe pasar por parametro la uartConfig_t a utilizar)*/

		xTaskCreate(wait_frame,                  // Funcion de la tarea a ejecutar
				(const char *) "wait_frame", // Nombre de la tarea como String amigable para el usuario
				configMINIMAL_STACK_SIZE * 12,   // Cantidad de stack de la tarea
				protocol_config,                    // Parametros de tarea
				tskIDLE_PRIORITY + 1,           // Prioridad de la tarea
				0                         // Puntero a la tarea creada en el sistema
				);

}





/**
 @brief Manejador del evento de recepcion de 1 byte via UART
 @param noUsado
 */
void protocol_rx_event(void * pvParameters)
{
	protocolData_t * uartData;

	uartData = (protocolData_t*) pvParameters;

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	/* leemos el caracter recibido */
	char c = uartRxRead(uartData->uart);

	BaseType_t signaled = xSemaphoreTakeFromISR(mutex,
			&xHigherPriorityTaskWoken);

	if (signaled)
	{
		if ( FRAME_MAX_SIZE - 1 == uartData->index)	//Se reinicia por haber alcanzado el maximo largo de caracteres permitido.
		{
			/* reinicio el paquete */
			uartData->index = 0;
		}

		if (c == SOM)						//Se leyo un Start Of Message
		{
			if (uartData->index != 0)
			{
				/* fuerzo el arranque del frame (descarto lo anterior)*/
				uartData->index = 0;
			}

			uartData->index++;
		}
		else if (c == EOM)					//Se leyo un End Of Message
		{
			/* solo cierro el fin de frame si al menos alcanzo la longitud minima del frame.*/
			if (uartData->index >= FRAME_MIN_SIZE)
			{
				/* se termino el paquete - se termina con un '\0' */
				uartData->buffer[(uartData->index) - 1] = '\0';

				/* incremento el indice */
				uartData->index++;

				/* Deshabilito todas las interrupciones de UART_USB */
				uartCallbackClr(uartData->uart, UART_RECEIVE);

				/* señalizo a la aplicacion */
				xSemaphoreGiveFromISR(new_frame_signal,
						&xHigherPriorityTaskWoken);
			}
			else
			{
				/* reinicio el paquete */
				uartData->index = 0;
			}
		}
		else
		{
			/* se guarda el dato si al menos se recibio un start.*/
			if (uartData->index >= 1)
			{
				/* guardo el dato */
				uartData->buffer[uartData->index - 1] = c;

				/* incremento el indice */
				uartData->index++;
			}
			else
			{
				/* no hago nada, descarto el byte */
			}
		}

		xSemaphoreGiveFromISR(mutex, &xHigherPriorityTaskWoken);
	}

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 @brief Inicializa el stack

 @param uartConfig_t
 */
void protocol_x_init(protocolData_t * uartData)
{
	/* CONFIGURO EL DRIVER */

	/* Inicializar la UART_USB junto con las interrupciones de Tx y Rx */
	uartConfig(uartData->uart, uartData->baudRate);

	/* Seteo un callback al evento de recepcion y habilito su interrupcion */
	uartCallbackSet(uartData->uart, UART_RECEIVE, protocol_rx_event, uartData);

	/* Habilito todas las interrupciones de UART_USB */
	uartInterrupt(uartData->uart, true);

	/* CONFIGURO LA PARTE LOGICA */
	new_frame_signal = xSemaphoreCreateBinary();
	mutex = xSemaphoreCreateMutex();
}

/**
 @brief Espera indefinidamente por un paquete.
 */
void protocol_wait_frame()
{
	xSemaphoreTake(new_frame_signal, portMAX_DELAY);
	xSemaphoreTake(mutex, 0);
}

/**
 @brief Una vez procesado el frame, descarta el paquete y permite una nueva recepcion.
 */
void protocol_discard_frame(protocolData_t * uartData)
{
	/* indico que se puede inciar un paquete nuevo */
	uartData->index = 0;

	/* libero la seccion critica, para que el handler permita ejecutarse */
	xSemaphoreGive(mutex);

	/* limpio cualquier interrupcion que hay ocurrido durante el procesamiento */
	uartClearPendingInterrupt(uartData->uart);

	/* habilito isr rx  de UART_USB */
	uartCallbackSet(uartData->uart, UART_RECEIVE, protocol_rx_event, uartData);
}

void wait_frame(void * pvParameters)
{
	inicio_c3();
	protocolData_t uartData;
	mensaje_t mensaje;

	uartData = *(protocolData_t*) pvParameters;

	protocol_x_init(&uartData);	//Se inicializa la comunicacion por UART y las interrupciones

	while ( TRUE)
	{
		protocol_wait_frame();		//Espera a que se reciba un frame completo.

		/*Guardar dato en memoria dinamica*/
		mensaje = guardarMensaje(uartData.buffer);

		if (mensaje != NULL)
		{
			/* para el printf */
		//	printf("[protocol.c]%s \n", mensaje);
			send_msg_to_c3(mensaje);

			protocol_discard_frame(&uartData); //Habilita nuevamente las interrupciones para poder seguir recibiendo mensajes


			//libero la memoria en task_c
			//liberarMemoria(mensaje);			//Se libera la memoria pedida
		}
		else
		{
			printf("Out of memory \n");
		}

		/* hago un blink para que se vea */
		gpioToggle(LEDB);
		vTaskDelay(25 / portTICK_RATE_MS);
		gpioToggle(LEDB);

	}
}

