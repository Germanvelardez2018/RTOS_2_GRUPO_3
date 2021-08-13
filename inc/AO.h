/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/08/12
 *
 *===========================================================================*/

#ifndef RTOS2_VERSION_FINAL_RTOS_INC_AO_H_
#define RTOS2_VERSION_FINAL_RTOS_INC_AO_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "sapi.h"
#define N_QUEUE_AO	5


typedef void   (*call_back_ao_t)(char*);



typedef struct
{
    void*          content;     //puntero usado para recibir datos de la queue
    TaskHandle_t   task_name;   //Referencia a la task del objeto activo
	QueueHandle_t  queue;      // cola de mensajes usada para enviar los blocks de los mensaje
	QueueHandle_t  output;     // cola donde se envia el bloque procesado
	call_back_ao_t action;     //callback
	bool_t         dead_flag;  //flag que determina si el objeto se ejecuta una sola vez y muere
	uint8_t 	   priority;   //

}
ao_base_t;




 void create_ao(ao_base_t* obj, call_back_ao_t action, bool_t dead, QueueHandle_t output,uint8_t priorty);



 void send_block_ao(ao_base_t* obj, void* block);




#endif /* RTOS2_VERSION_FINAL_RTOS_INC_AO_H_ */
