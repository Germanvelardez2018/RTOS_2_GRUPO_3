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



static void inline _init_task_ao(ao_base_t* obj)
{


	TaskCreate(driver_task,				// Funcion de la tarea a ejecutar
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
	 _init_task_ao(obj);



 }



 void send_block_ao(ao_base_t* obj, void* block);








