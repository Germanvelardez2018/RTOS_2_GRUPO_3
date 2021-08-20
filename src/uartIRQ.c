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

/*==================================Declaracion Defines============================*/

/*============================Declaracion de funciones privadas====================*/

static void _discard_block(driver_t *driver);

// Callback para la recepción
static void _onRxCallback(void *param);
// Callback para la transmicion
static void _onTxCallback(void *param);

static void _processing_byte(char new_byte, driver_t *driver);

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

void onRxTimeOutCallback(TimerHandle_t params)
{

	driver_t *driver = (driver_t *)pvTimerGetTimerID(params);

	// Inicio seccion critica
	taskENTER_CRITICAL();
	//Verificar que tengamos un bloque de mensaje cerrado, en caso contrario descartar
	//Tengo bloque cerrado y listo para enviar. Mandarlo por la queue
	if (driver->flow.state == FLOW_CLOSE)
	{

		xQueueSend(driver->onRxQueue, (void *)&(driver->flow.rxBlock), 0);
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
	else
	{	// Paso al time out y el bloque no esta cerrado, descartar
		//  printf("descarto buffer\n");
		_discard_block(driver);
	}

	taskEXIT_CRITICAL();
}

/*Funciones privadas*/

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
		//	printf("[isr]%c\n",new_byte);
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
		}
		else
		{
			_discard_block(driver);
		}

		break;

	default:
	{
		/*Se agregan bytes al mensaje*/
		driver->flow.rxBlock[driver->flow.rxLen] = new_byte;
		//aumento el rxLen
		(driver->flow.rxLen) = (driver->flow.rxLen) + 1;
	}
	}
}

static void _discard_block(driver_t *driver)
{
	ao_error_t ao_error = {.ao_base.state = AO_OFF};
	driver->flow.state = FLOW_NOT_INIT;
	driver->flow.rxLen = 0;
	create_error_ao(&ao_error, driver, insert_error, ERROR_INVALID_DATA, 0);

	post_AO(&(ao_error.ao_base), driver->flow.rxBlock);
}
