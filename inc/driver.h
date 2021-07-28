/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/07/09
 *
 *===========================================================================*/

#ifndef RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_DRIVER_H_
#define RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_DRIVER_H_


#include "sapi.h"
#include "qmpool.h"
#include "FreeRTOS.h"

#include "timers.h"

#include "queue.h"




#define SIMPLE_ASSERT( x )                                       \
    if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ) {; } }\




#define FRAME_MAX_SIZE				200
#define BLOCK_SIZE (FRAME_MAX_SIZE+1)
#define	PROTOCOL_TIMEOUT		pdMS_TO_TICKS(4) //4 ms de timeout por caracter
#define POOL_TOTAL_BLOCKS 		4//catidad de bloques en el pool de memoria
#define POOL_SIZE 				POOL_TOTAL_BLOCKS*BLOCK_SIZE //Tamaño total del pool



typedef enum
{
	FLOW_INIT,
	FLOW_NOT_INIT,
	FLOW_CLOSE
} state_flow;

typedef struct{

	  state_flow state;         //Estado del buffer
	  TimerHandle_t onTxTimeOut; //Timer para el timeout de la transmision
	  TimerHandle_t onRxTimeOut; //Timer para el timeout de la recepcion
	  char *txBlock; 			 //Bloque de memoria de la transmision en curso
	  char *rxBlock; 			//Bloque de memoria de la recepcion
	  uint8_t txLen; 			//longitud del paquete en transmision
	  uint8_t rxLen; 			//longitud del paquete recibido
	  uint8_t tx_counter;

}data_flow;


typedef struct
{

/*Puntero con memoria para el pool*/
 char* p_memory_pool;

 /*Estructura pool */
 QMPool	pool;

} memory_t;

typedef struct {
	/*Uart fisica a utilizar*/
	uartMap_t uart;

	/*Velocidad de transmision*/
	uint32_t baudrate;

	/*estructura con variables usadas en transmicion y recepcion*/
	QueueHandle_t onTxQueue; //cola de transmision (por esta cola llegan los bloques de memoria a transmitir)
	QueueHandle_t onRxQueue; //cola de recepcion (por esta cola llegan los bloques de memoria recibidos)
	/*Estructura para controlar el flujo de datos*/
	data_flow	flow;
	/*EStructura para la memoria*/
	memory_t memory;

} driver_t;




/*Prototipos de funciones publicas*/

bool_t driver_init(driver_t* driver);






/*Tarea principal del modulo*/


 void driver_manager(void* params);








#endif /* RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_DRIVER_H_ */
