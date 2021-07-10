/*
 * gp.h
 *
 *  Created on: 9 jul. 2021
 *      Author: gvelardez
 */

#ifndef RTOS2_GESTOR_DE_PROTOLOS_SRC_GESTOR_PROTOCOLOS_GP_H_
#define RTOS2_GESTOR_DE_PROTOLOS_SRC_GESTOR_PROTOCOLOS_GP_H_


#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "sapi.h"
#include "gp.h"
#include "qmpool.h"



#define 	SOM						'('
#define 	EOM						')'
#define		MAX_LEN					200





/*Estructuras utilizadas en el modulo*/

/*Estructura con la configuracion del moludo*/


typedef	char*   gp_buffer ;






/*Maquina de estados para el gestor de protocolos*/
typedef enum
{
	/*El buffer esta vacio y aun no recibimos tramas*/
	IDLE,

	/*Se recibio el primer elemento de la trama*/
	START_MESSAGE,

	/*Se recibio otro elemento mas de la trama*/
	PROCESSING,

	/*Se recibio el elemto de fin de la trama*/
	END_MESSAGE
}
GP_STATE_MACHINE;







/*Estructura del gestor de protocolos*/

typedef struct
{
	/*MAQUINA DE ESTADOS*/
		GP_STATE_MACHINE 	state;

	/*El puerto que se va a utilizar*/
		uartMap_t uart;

		/*El baudrate del modulo*/
		uint16_t 	baudrate;

		/*El tiempo que espera que llegue un nuevo caracter en ms*/
		TickType_t 	time_out;


		/*Puntero a memoria para el pool*/
		gp_buffer	Pounter_memory;

		/*Size del pool de memoria*/
		uint16_t pool_size;

		/*uint16_t pool_block_size*/
		uint16_t pool_block_size;

		/*Pool de memoria*/
		QMPool    Pool_memory;
} obj_gp;



/*Funcion que inicializa el modulo para un puerto uart en particular
 * en vez de pasar
 *
 * */


void gp_init(obj_gp*  object);




/*Cambio de estado la maquina de estados asociada al obj_gp
 *
 * */

void change_state_machine(char input, GP_STATE_MACHINE   *state_machine);




#endif /* RTOS2_GESTOR_DE_PROTOLOS_SRC_GESTOR_PROTOCOLOS_GP_H_ */




