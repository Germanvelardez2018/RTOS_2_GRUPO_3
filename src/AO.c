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



void event_dispacher(msg_t block)
{
	}


static void event_handler(ao_base_t* obj)
{

	BaseType_t retQueue;
	msg_t block;
	ao_base_t* ao = (ao_base_t*) obj;
	while(1)
	{
		if(uxQueueMessagesWaiting(ao->queue))
		{
			retQueue = xQueueSend(ao->queue,&block,0) ;// Evitar bloqueos

			if(retQueue) //lectura exitosa, entonces llamo callback
			{
				(ao->action)(&block);
			}
		}
		else
		{
			//
			ao->state = AO_OFF;
			vQueueDelete(ao->queue);
			vTaskDelete(NULL);
		}

	}

}



bool_t create_ao(ao_base_t* obj, call_back_ao_t action,uint8_t priorty)
{
	BaseType_t retValue = pdFALSE;

	obj->queue = xQueueCreate(N_QUEUE_AO, sizeof(msg_t));

	//el handler es generico para todos los objetos de este estilo
	if(obj->queue != NULL)
	{
		//se asigna callback
		obj->action = action;


		retValue = xTaskCreate(event_handler,(const char*)"AO generico", configMINIMAL_STACK_SIZE*2, obj, tskIDLE_PRIORITY+priorty, NULL);
	}

	if(retValue == pdFALSE)
	{
		obj->state = AO_ON;

		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

















