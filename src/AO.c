/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/08/12
 *
 *===========================================================================*/

#include "AO.h"
#include "driver.h"
#include "msg_format.h"
#include "sapi.h"
#include "error_msg.h"

#include "uartIRQ.h"//!BORRAR!

/*============================Declaracion de funciones privadas====================*/

static void event_handler(void *obj);


/*================================Funciones publicas==============================*/

void post_AO(ao_base_t *obj, char *block)

{
	xQueueSend(obj->queue, &block, 1);
}

bool_t create_ao(ao_base_t *obj, driver_t *driver, callback_ao_t action, uint8_t priorty)
{
	BaseType_t retValue = pdFALSE;

	if (obj->state == AO_ON)
	{
		return retValue;
	}

	obj->queue = xQueueCreate(N_QUEUE_AO, sizeof(char *));

	obj->driver = driver;

	//Se crea la tarea asociada al objeto y se asigna su callback.
	if (obj->queue != NULL)
	{
		//se asigna callback
		obj->action = action;
		retValue = xTaskCreate(event_handler, (const char *)"AO generico", configMINIMAL_STACK_SIZE * 8, obj, tskIDLE_PRIORITY + priorty, obj->task);
	}

	if (retValue != pdFALSE)
	{
		obj->state = AO_ON;
		return retValue;
	}
	else
	{
		return retValue;
	}
}

/*================================Funciones privadas================================*/

/*
 * event handler generico para todos los objetos activos definidos en este modulo
 * */

static void event_handler(void *obj)
{
	ao_base_t *ao = (ao_base_t *)obj;

	while (1)
	{
		if (xQueueReceive(ao->queue, &(ao->message), 0))
		{
			(*ao->action)((ao->message));
			xQueueSend(ao->returnQueue, &ao->message,0);
		}
		else
		{
			ao->state = AO_OFF;
			vQueueDelete(ao->queue);			//En caso de no tener mas mensajes en la cola, el objeto se suicida.
			vTaskDelete(NULL);
		}
	}
}

