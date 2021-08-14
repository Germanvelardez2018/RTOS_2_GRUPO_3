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



 void post_AO(ao_base_t* obj, char* block)
{
	xQueueSend(obj->queue,&block,0);
}



void event_dispacher(msg_t block)
{


	//checkeo el block, si error se crea ao error
	errorCodes_t checkOk = BLOCK_OK;
	//declaro un obj activo
	if(checkOk != BLOCK_OK)
	{
		//creo el obj activo de error y le paso la callback de error
		// 				insert_error_msg(buffer,checkOk);

	}

	// si no error de formato
	else
	{
			char C = block[4];
		switch(C)
		{
		case FPASCAL:


			//creo objeto activo pascal. Antes de la creacion debo enviarle el block a la queue del objeto activo
			break;
		case FCAMEL:
			//creo objeto activo  camel
			break;
		case FSNAKE:
			//creo objeto activo snake
			break;
		}
	}




}


static void event_handler(void* obj)
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
				(ao->action)(block);
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



bool_t create_ao(ao_base_t* obj, callback_ao_t action,uint8_t priorty)
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

















