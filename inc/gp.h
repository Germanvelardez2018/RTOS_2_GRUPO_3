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
#define 	BLOCK_SIZE				20
#define 	N_BLOCKS_P_POOL			10
#define		POOL_SIZE				200
#define		BAUDRATE				115200




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





/*IDEA CONCEPTUAL
 *
 * TENEMOS UN POOL_SIZE DE 2000
 * Y UN BLOCK_SIZE 20, ENTONCES TENEMOS 50 BLOQUES DISPONIBLES
 *
 *
 *
 *
 *ejemplo
 *            !                    !
 *1)          HOLA MUNDO, MENSAJE
 *1)          DE PRUEBA           .
 *2)
 *3)
 *
 *
 *
 *
 * */





















/*Estructura del gestor de protocolos*/

typedef struct
{
	/*MAQUINA DE ESTADOS*/
		GP_STATE_MACHINE 	state;

		/*Uart*/
		uartMap_t uart;


		/*index*/

		//posicion del ultimo caracter en el blog
		uint8_t index;

		//index del pool de memoria utilizado
		uint8_t  b_index;

		//index del pool de memoria utilizado

		uint8_t p_index;


		/*El tiempo que espera que llegue un nuevo caracter en ms*/
		TickType_t 	time_out;


		/*10 Pools de memoria*/
		gp_buffer    BLOCKS_MEMORY[N_BLOCKS_P_POOL];


		QMPool POOL_MEMORY;
		/*array de punteros para bloques
		 * TENEMOS 10 PUNTEROS Y A MEDIDA
		 * QUE LO NECESITEMOS LLAMAREMOS A LA FUNCION get_block
		 *
		 * */

} obj_gp;



/*Funcion que inicializa el modulo para un puerto uart en particular
 * en vez de pasar
 *
 * */


void gp_init(obj_gp*  object,uartMap_t uart);




/*Cambio de estado la maquina de estados asociada al obj_gp
 *
 * */

void change_state_machine(char input, obj_gp*  object);




#endif /* RTOS2_GESTOR_DE_PROTOLOS_SRC_GESTOR_PROTOCOLOS_GP_H_ */




