/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/07/09
 *
 *===========================================================================*/

#include "uartIRQ.h"
#include "check_functions.h"
#include "msg_format.h"
#include "AO.h"
#include <string.h>
#include "crc8.h"
#include "error_msg.h"

/*==================================Declaracion Defines============================*/

/*==================================Variables Globales=============================*/



/*============================Declaracion de funciones privadas====================*/

static void _discard_block(driver_t *driver);


static void _onRxCallback(void *param);// Callback para la recepción

static void _onTxCallback(void *param);// Callback para la transmicion

static void _processing_byte(char new_byte, driver_t *driver);

void add_crc_at_block(char *block);

void send_block_from_ISR(char *block, driver_t *driver);

/*================================Funciones publicas==============================*/

bool_t txInterruptEnable(driver_t *driver)
{

	switch (driver->uart)
	{
	case UART_GPIO:
	case UART_USB:

	case UART_232:
	{
		uartCallbackSet(driver->uart, UART_TRANSMITER_FREE, _onTxCallback, (void *)driver);
		break;
	}
	default:
	{
		// No se ha seleccionado una UART correcta, por lo que retornamos false como un error.
		return (FALSE);
	}
	}
	return (TRUE);
}

bool_t rxInterruptEnable(driver_t *driver)
{

	switch (driver->uart)
	{
	case UART_GPIO:
	case UART_USB:

	case UART_232:
	{
		uartCallbackSet(driver->uart, UART_RECEIVE, _onRxCallback, (void *)driver);
		break;
	}
	default:
	{
		// No se ha seleccionado una UART correcta, por lo que retornamos false como un error.
		return (FALSE);
	}
	}

	return (TRUE);
}

void free_block(driver_t *driver, char *block)
{
	//sirve para liberar block que vienen de driver o block que viene por parametro
	if (block == NULL)
	{
		// Libero el bloque de memoria que ya fue trasmitido
		QMPool_put(&(driver->memory.pool), driver->flow.txBlock);
	}
	else
	{
		QMPool_put(&(driver->memory.pool), block);
	}
}

void onTxTimeOutCallback(TimerHandle_t params)
{
	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;

	// Obtenemos los datos de la UART seleccionada, aprovechando el campo reservado para el Timer ID.
	driver_t *driver = (driver_t *)pvTimerGetTimerID(params);

	//Inicio seccion critica
	taskENTER_CRITICAL();

	free_block(driver, NULL);

	driver->flow.tx_counter = 0; //Reinicio el contador de bytes transmitidos para la siguiente transmision
	taskEXIT_CRITICAL();		 // Fin de seccion critica
								 // Si hay mensajes esperando, vuelvo a prender la interrupcion y la disparo
	if (uxQueueMessagesWaiting(driver->onTxQueue))
	{
		txInterruptEnable(driver);
		uartSetPendingInterrupt(driver->uart);
	}
	else
	{ // Volvemos a encender la interrupcion de RX
		rxInterruptEnable(driver);
		uartSetPendingInterrupt(driver->uart);
	}
}




/*Para enviar un bloque cerrado con menos de 200 caracteres*/
static void _send_to_c3(driver_t* driver)
{

	BaseType_t xHigherPriorityTaskWoken;
	    /* We have not woken a task at the start of the ISR. */
	xHigherPriorityTaskWoken = pdFALSE;
	xQueueSendFromISR(driver->onRxQueue, &(driver->flow.rxBlock), &xHigherPriorityTaskWoken);
	driver->flow.state = FLOW_NOT_INIT;
	//Una vez que el bloque esta en la cola pido otro bloque para el siguiente paquete. Es responsabilidad de
	//la aplicacion liberar el bloque mediante una transmision o con la funcion putBuffer()
	driver->flow.rxBlock = (char *)QMPool_get(&(driver->memory.pool), 0); //pido un bloque del pool

							  //Chequeo si tengo un bloque de memoria, sino anulo la recepcion de paquetes
	if (driver->flow.rxBlock == NULL)
	{
		uartCallbackClr(driver->uart, UART_RECEIVE);

	}

}


void onRxTimeOutCallback(TimerHandle_t params)
{

	driver_t *driver = (driver_t *)pvTimerGetTimerID(params);

	// Inicio seccion critica
	taskENTER_CRITICAL();

	_discard_block(driver);


	taskEXIT_CRITICAL();
}


void send_block(char *block, driver_t *driver)

{
	/*Antes de enviar el mensaje calcular CRC y agregarlo*/
	add_crc_at_block(block);
	/* Se envia a la cola de transmision el block a transmitir*/
	xQueueSend(driver->onTxQueue, &block, portMAX_DELAY);
	/* No se permite que se modifique txcounter*/
	taskENTER_CRITICAL();
	/* Si se esta enviando algo no se llama a la interrupcion para no interrumpir el delay*/
	if (driver->flow.tx_counter == 0) //
	{
		txInterruptEnable(driver);
	}
	taskEXIT_CRITICAL();
	uartSetPendingInterrupt(driver->uart);
}




/*================================Funciones privadas================================*/

// Callback para la recepción
static void _onRxCallback(void *param)
{

	UBaseType_t uxSavedInterruptStatus;
	BaseType_t xTaskWokenByReceive = pdFALSE; //Variable para evaluar el cambio de contexto al finaliza la interrupcion

	driver_t *driver = (driver_t *)param;

	uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR(); // Protejo selectedUart

	//si llegan mas bytes de lo que entra en el buffer no los leo. Luego descarto el paquete en el timer de timeout
	if ((driver->flow.rxLen) < FRAME_MAX_SIZE)
	{
		// Obtenemos el byte de la UART seleccionada
		char new_byte = uartRxRead(driver->uart);
		_processing_byte(new_byte, (driver_t *)driver);
	}
	else
	{
		//si hay bytes de mas decarto mensaje?
		_discard_block(driver);
	}

	// Inicializamos el timer/ Las sucesivas llamadas reinician el timer
	xTimerStartFromISR(driver->flow.onRxTimeOut, &xTaskWokenByReceive);

	taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);

	//Hago la evaluacion de cambio de contexto necesario.
	if (xTaskWokenByReceive != pdFALSE)
	{
		taskYIELD();
	}
}
// Callback para la transmicion
void _onTxCallback(void *param)
{

	gpioToggle(LED2);
	driver_t *driver = (driver_t *)param;
	BaseType_t xTaskWokenByReceive = pdFALSE; //Variable para evaluar el cambio de contexto al finaliza la interrupcion
	UBaseType_t uxSavedInterruptStatus;

	//Armo una seccion crtitica para proteger las variables de txCounter
	uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();

	//	Si el contador esta en cero tengo que tomar un bloque de la cola, calcular CRC
	if (driver->flow.tx_counter == 0)
	{
		//	Tomo el bloque a transmitir de la cola
		xQueueReceiveFromISR(driver->onTxQueue, &(driver->flow.txBlock), &xTaskWokenByReceive);
		driver->flow.txLen = strlen(driver->flow.txBlock);
		uartTxWrite(driver->uart, '(');
	}
	//	Si no llegué al final de la cadena transmito el dato
	if (driver->flow.tx_counter < driver->flow.txLen)
	{
		uartTxWrite(driver->uart, driver->flow.txBlock[driver->flow.tx_counter]);
		driver->flow.tx_counter++;
	}

	//	Cuando termino de transmitir todos los bytes tengo que
	//	deshabilitar la interrupción e iniciar el timeout.
	if ((driver->flow.tx_counter) == (driver->flow.txLen))
	{
		uartTxWrite(driver->uart, ')');

		uartCallbackClr(driver->uart, UART_TRANSMITER_FREE);				//Deshabilito la interrupcion de TX
		xTimerStartFromISR(driver->flow.onTxTimeOut, &xTaskWokenByReceive); //Inicio el timer de Timeout
	}

	//Cierro la seccion critica
	taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);

	//Hago la evaluacion de cambio de cambio de contexto necesario.
	if (xTaskWokenByReceive != pdFALSE)
	{
		taskYIELD();
	}
}

static void _processing_byte(char new_byte, driver_t *driver)
{

	switch (new_byte)
	{

	case SOM:
		/*Inicio de mensaje*/
		driver->flow.rxLen = 0;
		driver->flow.state = FLOW_INIT;
		break;

	case EOM:
		/*Final de mensaje*/

		driver->flow.rxBlock[driver->flow.rxLen] = '\0';


		/*Se chequea el crc del bloque*/
		if (check_CRC(driver->flow.rxBlock))
		{
			driver->flow.state = FLOW_CLOSE;
			driver->flow.rxBlock[driver->flow.rxLen - CRC_SIZE] = '\0';

			//send to c3
			_send_to_c3(driver);
		}
		else
		{

			insert_error(driver->flow.rxBlock, ERROR_INVALID_DATA);
			send_block_from_ISR(driver->flow.rxBlock, driver);
		}

		break;

	default:
		{
			if (driver->flow.state == FLOW_INIT)
			{
				/*Se agregan bytes al mensaje*/
				driver->flow.rxBlock[driver->flow.rxLen] = new_byte;
				(driver->flow.rxLen) = (driver->flow.rxLen) + 1;
			}

		}
	}
}

static void _discard_block(driver_t *driver)
{

	driver->flow.state = FLOW_NOT_INIT;
	driver->flow.rxLen = 0;

}

void add_crc_at_block(char *block)
{
	int8_t len = strlen(block);

	int8_t crc = crc8_calc(0, block, len);
	char CRC[CRC_SIZE];
	int_to_ASCII(crc, CRC);
	//agrego el crc

	block[len] = CRC[0];
	block[len + 1] = CRC[1];
	block[len + 2] = '\0';
}

void send_block_from_ISR(char *block, driver_t *driver)

{
	BaseType_t xTaskWokenByReceive = pdFALSE;
	UBaseType_t uxSavedInterruptStatus;
	/*Antes de enviar el mensaje calcular CRC y agregarlo*/
	add_crc_at_block(block);
	/* Se envia a la cola de transmision el block a transmitir*/
	xQueueSendFromISR(driver->onTxQueue, &block, &xTaskWokenByReceive);
	/* No se permite que se modifique txcounter*/
	uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
	/* Si se esta enviando algo no se llama a la interrupcion para no interrumpir el delay*/
	if (driver->flow.tx_counter == 0) //
	{
		txInterruptEnable(driver);
	}
	taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus);
	uartSetPendingInterrupt(driver->uart);
}
