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
   change_state_machine(input,obj);




}















static void _init_state_machine(obj_gp*  object)
{
	/*El sistema siempre empieza en estado IDLE*/
	object->state = IDLE;
	/*Escribir la logica de lo que debe pasar*/
	//inicia el buffer
	object->index = 0;

}



static void _init_frame_state(obj_gp*  object)
{
	/*Se detecto el inicio de una nueva trama*/
	object->state = START_MESSAGE;
	/*Escribir la logica de lo que debe pasar*/

	//funcion para pedir un buffer al pool

	//guardar el caracter en el buffer

	//subir index una posicion
	object->index +=1;
}


static void _end_frame_state(obj_gp*  object)
{
	/*Se detecto el caracter de finalizacion*/
	object->state = END_MESSAGE;
	/*Escribir la logica de lo que debe pasar*/

	//mandar a queue_print el buffer


}


static void	 _processing_input(obj_gp*  object)
{
	/*Se detecto un caracter cualquiera.*/
	 object->state = PROCESSING;
	/*Escribir la logica de lo que debe pasar*/

		//guardar el caracter en el buffer

	 //subir index una posicion
	 	object->index +=1;
}







/*Funciones publicas*/




/*Inicio el modulo*/



void gp_init(obj_gp*  object)
{

		/*Se inicia la uart*/
	   uartConfig(object->uart,object->baudrate );

	   /*Index del mensaje inicia en zero*/
	   _init_state_machine(object);
	   // uart el paso de parametros para pasar la maquina de estado
	   uartCallbackSet(UART_USB, UART_RECEIVE, _ISR_Processing,(void*)object);
	   // Habilito todas las interrupciones de UART_USB
	   uartInterrupt(UART_USB, true);
	   /*Iniciamos el pool de memoria*/

	   object->Pounter_memory = ( gp_buffer ) pvPortMalloc( object->pool_size * sizeof( char ) );

	   QMPool_init( &(object->Pool_memory), &(object->Pounter_memory),object->pool_size,object->pool_block_size ); //Tamanio del segmento de memoria reservado


}






void change_state_machine(char input_char,obj_gp*  object)
{
	switch(input_char)
	{
			// si llego (
		case SOM:
			 _init_frame_state(object);
			break;

			//si llego )
		case EOM:
			_end_frame_state(object);
			break;
			//si llego otro caracter
		default:
			_processing_input(object);
			break;

	}
}





/*Pero puede que nos convenga o nos pidan que hagamos un modulo por cada capa de abstraccion*/



