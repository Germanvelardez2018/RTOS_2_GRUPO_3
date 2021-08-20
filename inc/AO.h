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
#include "error_msg.h"
#define N_QUEUE_AO 2

typedef enum
{
	AO_OFF,
	AO_ON
} AO_state;

typedef void (*callback_ao_t)(char *);

typedef void (*error_callback_t)(char *, errorCodes_t);

typedef struct
{
	char *message;
	QueueHandle_t queue;  // cola de mensajes usada para enviar los blocks de los mensaje
	callback_ao_t action; //callback
	AO_state state;
	driver_t *driver; //NECESARIO PARA conectarse con UART
	TaskHandle_t* task; // Puntero para referenciar la tarea asociada al objeto activo
} ao_base_t;

typedef struct
{
	error_callback_t e_callback;
	errorCodes_t type_error;
	ao_base_t ao_base;
} ao_error_t;

void post_AO(ao_base_t *obj, char *block);

bool_t create_ao(ao_base_t *obj, driver_t *driver, callback_ao_t action, uint8_t priorty);
bool_t create_error_ao(ao_error_t *error, driver_t *driver, error_callback_t action, errorCodes_t error_type, uint8_t priorty);

void send_block_ao(ao_base_t *obj, void *block);

void post_AO(ao_base_t *obj, char *block);

#endif /* RTOS2_VERSION_FINAL_RTOS_INC_AO_H_ */
