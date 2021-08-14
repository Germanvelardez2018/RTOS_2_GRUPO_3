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

/*============================Declaracion de funciones privadas====================*/

static void event_handler(void* obj);




static void error_event_handler(void* obj);



/*================================Funciones publicas==============================*/



 void post_AO(ao_base_t* obj, char*  block)

{
	xQueueSend(obj->queue,&block,1);
}







bool_t create_ao(ao_base_t* obj, driver_t* driver, callback_ao_t action,uint8_t priorty)
{
	BaseType_t retValue = pdFALSE;
	obj->queue = xQueueCreate(N_QUEUE_AO, sizeof(char*));

	obj->driver = driver;

	//el handler es generico para todos los objetos de este estilo
	if(obj->queue != NULL)
	{
		//se asigna callback
		obj->action = action;
		retValue = xTaskCreate(event_handler,(const char*)"AO generico", configMINIMAL_STACK_SIZE*8, obj, tskIDLE_PRIORITY+priorty, NULL);


	}

	if(retValue != pdFALSE)
	{
		obj->state = AO_ON;
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}






bool_t create_error_ao(ao_error_t* error, driver_t* driver, error_callback_t action,errorCodes_t error_type,uint8_t priorty)
{
	BaseType_t retValue = pdFALSE;
	error->ao_base.queue = xQueueCreate(N_QUEUE_AO, sizeof(char*));
	error->type_error= error_type;
	error->ao_base.driver = driver;

	//el handler es generico para todos los objetos de este estilo
	if(error->ao_base.queue != NULL)
	{
		//se asigna callback
		error->e_callback = action;
		retValue = xTaskCreate(error_event_handler,(const char*)"AO error", configMINIMAL_STACK_SIZE*8, error, tskIDLE_PRIORITY+priorty, NULL);


	}

	if(retValue != pdFALSE)
	{
		error->ao_base.state = AO_ON;
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}




/*================================Funciones privadas================================*/


/*
 * event handler generico para todos los objetos activos definidos en este modulo
 * */

static void event_handler(void* obj)
{

	BaseType_t retQueue;

	ao_base_t* ao = (ao_base_t*) obj;


	while(1)
	{
		if(uxQueueMessagesWaiting(ao->queue))
		{
			retQueue = xQueueReceive(ao->queue,&(ao->message),0) ;// Evitar bloqueos

			if(retQueue) //lectura exitosa, entonces llamo callback
			{
				(*ao->action)((ao->message));
				send_block((ao->message),ao->driver);
			}
		}
		else
		{

			ao->state = AO_OFF;
			vQueueDelete(ao->queue);
			vTaskDelete(NULL);
		}

	}

}






static void error_event_handler(void* obj)
{

	BaseType_t retQueue;

	ao_error_t* error = (ao_error_t*) obj;


	while(1)
	{
		if(uxQueueMessagesWaiting(error->ao_base.queue))
		{
			retQueue = xQueueReceive(error->ao_base.queue,&(error->ao_base.message),0) ;// Evitar bloqueos

			if(retQueue) //lectura exitosa, entonces llamo callback
			{
				(error->e_callback)((error->ao_base.message),error->type_error);
				send_block((error->ao_base.message),error->ao_base.driver);
			}
		}
		else
		{

		error->ao_base.state = AO_OFF;
			vQueueDelete(error->ao_base.queue);
			vTaskDelete(NULL);
		}

	}

}












