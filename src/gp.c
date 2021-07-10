/*
 * gp.c
 *
 *  Created on: 9 jul. 2021
 *      Author: gvelardez
 */





#include "gp.h"
#include "sapi.h"

/*Definiciones*/


/*Variables privadas (global e static)*/







/*Funciones privadas (static)
 * Toda aquella funcion que no tenga sentido llamar desde fuera de este modulo,
 * por ejemplo funciones intermedias o de soporte de otras funciones
 *
 * */











/*CALLBACK de la interrupcion*/

static void _ISR_Processing( void *vargs )
{


	obj_gp *obj = (obj_gp*)vargs ;


   char input = uartRxRead( obj->uart);

   /*Funcion que cambia de estado la maquina*/
   change_state_machine(input,&(obj->state));




}















static void _init_state_machine(GP_STATE_MACHINE  *state_machine)
{
	/*El sistema siempre empieza en estado IDLE*/
	(*state_machine) = IDLE;
}



static void _init_frame_state(GP_STATE_MACHINE  *state_machine)
{
	/*Se detecto el inicio de una nueva trama*/
	(*state_machine) = START_MESSAGE;
}


static void _end_frame_state(GP_STATE_MACHINE   *state_machine)
{
	/*Se detecto el caracter de finalizacion*/
	(*state_machine) = END_MESSAGE;

}


static void	 _processing_input(GP_STATE_MACHINE   *state_machine)
{
	(*state_machine) = PROCESSING;
}







/*Funciones publicas*/




/*Inicio el modulo*/



void gp_init(obj_gp*  object)
{

		/*Se inicia la uart*/
	   uartConfig(object->uart,object->baudrate );

	   // uart el paso de parametros para pasar la maquina de estado
	   uartCallbackSet(UART_USB, UART_RECEIVE, _ISR_Processing,(void*)object);
	   // Habilito todas las interrupciones de UART_USB
	   uartInterrupt(UART_USB, true);
	   /*Iniciamos el pool de memoria*/

	   object->Pounter_memory = ( gp_buffer ) pvPortMalloc( object->pool_size * sizeof( char ) );

	   QMPool_init( &(object->Pool_memory), &(object->Pounter_memory),object->pool_size,object->pool_block_size ); //Tamanio del segmento de memoria reservado


}






void change_state_machine(char input_char,GP_STATE_MACHINE *state_machine)
{
	switch(input_char)
	{

		case SOM:
			 _init_frame_state(state_machine);
			break;

		case EOM:
			_end_frame_state(state_machine);
			break;

		default:
			_processing_input(state_machine);
			break;

	}
}





/*Pero puede que nos convenga o nos pidan que hagamos un modulo por cada capa de abstraccion*/



