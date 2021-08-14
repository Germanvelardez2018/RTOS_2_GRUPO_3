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



static ao_task(void* params)
{
	//recibe un objeto activo
	ao_base_t *obj = (ao_base_t*)params;


	  // Cuando hay un evento, lo procesamos.
	    while( TRUE )
	    {
	        // Verifico si hay elementos para procesar en la cola. Si los hay, los proceso.
	        if( uxQueueMessagesWaiting( obj->queue ) )
	        {
	            // Hago una lectura de la cola.
	            BaseType_t res = xQueueReceive( obj->queue, &(obj->content), portMAX_DELAY );

	            // Si la lectura fue exitosa, proceso el dato.
	            if( res )
	            {
	                // Llamamos al callback correspondiente en base al comando que se le pas�.
	                ( obj->action )( &(obj->content) );


	              // content tiene el bloque con el formato ya cambiado
	              // se  envia a queue out
	            	xQueueSend( *(obj->output), &(obj->content), 0 );


	            }
	        }

	        // Caso contrario, la cola est� vac�a, lo que significa que debo eliminar la tarea.
	        else
	        {


	            // Borramos la cola del objeto activo.
	            vQueueDelete( obj->queue );

	            // Y finalmente tenemos que eliminar la tarea asociada (suicidio).
	            vTaskDelete( NULL );

	        }
	    }





}


static void inline _init_task_ao(ao_base_t* obj)
{


	TaskCreate(ao_task,				// Funcion de la tarea a ejecutar
				(const char *) "active object", 	// Nombre de la tarea como String amigable para el usuario
				configMINIMAL_STACK_SIZE * 4,   // Cantidad de stack de la tarea
				obj,                    	// Parametros de tarea
				tskIDLE_PRIORITY + obj->priority,           // Prioridad de la tarea
				obj->task_name);                         	// Puntero a la tarea creada en el sistema

		//verificar resultado




}




 void create_ao(ao_base_t* obj, call_back_ao_t action, bool_t dead, QueueHandle_t output,uint8_t priorty)
 {
	 // asignacion
	 obj->action = action;
	 //inicio la queue
	 obj->queue = xQueueCreate(N_QUEUE_AO, sizeof(char*));

	 //referencio a queue TX de UART
	 obj->output = output;

	 //
	 _init_task_ao(obj);



 }

// esta funcion se usa en el task_driver---> que deberia transformarse en dispacher event

 void send_block_ao(ao_base_t* obj, void* block);








