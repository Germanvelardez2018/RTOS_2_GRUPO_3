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
#include "driver.h"


#define N_QUEUE_AO	2





typedef enum
{
	AO_ON,
	AO_OFF
} AO_state;




typedef void   (*callback_ao_t)(char*);



typedef struct
{
    char*         message;
	QueueHandle_t  queue;      // cola de mensajes usada para enviar los blocks de los mensaje
	callback_ao_t action;     //callback
	AO_state		state;
	driver_t*     driver;   //NECESARIO PARA conectarse con UART
}
ao_base_t;



void post_AO(ao_base_t* obj, char* block);





bool_t create_ao(ao_base_t* obj,driver_t* driver, callback_ao_t action,uint8_t priorty);


 void send_block_ao(ao_base_t* obj, void* block);

 void post_AO(ao_base_t* obj, char* block);


#endif /* RTOS2_VERSION_FINAL_RTOS_INC_AO_H_ */
