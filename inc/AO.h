/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/08/12
 *
 *===========================================================================*/

#ifndef RTOS2_VERSION_FINAL_RTOS_INC_AO_H_
#define RTOS2_VERSION_FINAL_RTOS_INC_AO_H_

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "sapi.h"
#define N_QUEUE_AO	5



//queda mas piola

typedef char* msg_t;

typedef enum
{
	AO_ON,
	AO_OFF
} AO_state;














typedef void   (*call_back_ao_t)(msg_t);



typedef struct
{
    void*          content;     //puntero usado para recibir datos de la queue
	QueueHandle_t  queue;      // cola de mensajes usada para enviar los blocks de los mensaje
	call_back_ao_t action;     //callback
	AO_state		state;
	uint8_t 	   priority;   //

}
ao_base_t;





void event_dispacher(msg_t block);


bool_t create_ao(ao_base_t* obj, call_back_ao_t action,uint8_t priorty);


 void send_block_ao(ao_base_t* obj, void* block);




#endif /* RTOS2_VERSION_FINAL_RTOS_INC_AO_H_ */
