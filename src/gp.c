/*
 * gp.c
 *
 *  Created on: 9 jul. 2021
 *      Author: gvelardez
 */





#include "gp.h"





#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"


#define N_QUEUE 10

//definida en main, mejorar depues
static   QueueHandle_t queue_print;


/*Definiciones*/







/*Variables privadas (global e static)*/







/*Funciones privadas (static)
 * Toda aquella funcion que no tenga sentido llamar desde fuera de este modulo,
 * por ejemplo funciones intermedias o de soporte de otras funciones
 *
 * */








static void print_manager(void *taskParmPtr)
{
    char *mensaje = NULL;

    while (1)
    {
        xQueueReceive(queue_print, &mensaje, portMAX_DELAY);

        printf("%s\n", mensaje);
    }
}
static void init_print_manager(void)
{

    BaseType_t res;



    // Creo tarea unica de impresion
    res = xTaskCreate(
        print_manager,                 // Funcion de la tarea a ejecutar
        (const char *)"print_manager", // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE * 8,  // Cantidad de stack de la tarea
        0,                             // Parametros de tarea
        tskIDLE_PRIORITY + 1,          // Prioridad de la tarea
        0                              // Puntero a la tarea creada en el sistema
    );

    // Gestion de errores
    configASSERT(res == pdPASS);

    // Crear cola
    queue_print = xQueueCreate(N_QUEUE, sizeof(char *));

    // Gestion de errores de colas
    configASSERT(queue_print != NULL);
}




/*CALLBACK de la interrupcion*/

static void _ISR_Processing( void *vargs )
{


	obj_gp *obj = (obj_gp*)vargs ;


   char input = uartRxRead( obj->uart);

   /*Funcion que cambia de estado la maquina*/
   change_state_machine(input,obj);


}




//funciones para gestionar memoria dinamica

// pido un bloque mas
static void  get_new_block(obj_gp* obj)
{
	if( obj->index == 0)
	{
		//index de bloque 0
		obj->b_index = 0;
	}
	else
	{
		obj->b_index +=1;
		// cuando paso a otro bloque index debe volver a cero
		obj->index = 0;

	}
	obj->BLOCKS_MEMORY[obj->b_index] = (gp_buffer)QMPool_get(&( obj->POOL_MEMORY), 0 );
}



/*libero los bloques*/
void free_blocks(obj_gp* object)
{
 //pensarla con tiempo
	for(int8_t i=0; i<=(object->b_index); i++)
	{
		QMPool_put( &(object->POOL_MEMORY), object->BLOCKS_MEMORY[i] );
	}

}



void save_char(obj_gp* obj,char c)
{

	//GUARDO EL CHAR DONDE CORRESPONDE

	obj->BLOCKS_MEMORY[obj->b_index][obj->index] = c;
	//despues de guardar el char debo, index ++ y pasar de block si es necesario
	obj->index+=1;
	if(obj->index >=BLOCK_SIZE )
	{
		//pasar al siguiente block
		get_new_block(obj);
	}
}










static void _init_state_machine(obj_gp*  object)
{
	/*El sistema siempre empieza en estado IDLE*/
	object->state = IDLE;
	/*Escribir la logica de lo que debe pasar*/
	//inicia el buffer
	object->index = 0;


	//pido la memoria para todo el pool
	gp_buffer memory = ( gp_buffer ) pvPortMalloc( POOL_SIZE * sizeof( char ) );


	//Inicio el pool

	//en caso de mensaje de 200 char, se necesita agregar caracter 0\ en la ultima posicion por eso size 201
	QMPool_init(&(object->POOL_MEMORY),  memory,(POOL_SIZE+1)*sizeof( gp_buffer ),BLOCK_SIZE ); //Tamanio del segmento de memoria reservado

}



static void _init_frame_state(obj_gp*  object)
{
	/*Se detecto el inicio de una nueva trama*/
	object->state = START_MESSAGE;
	/*Escribir la logica de lo que debe pasar*/

	//descarto todo el mensaje y empiezo de nuevo, deberia devolver los bloques pedidos
	object->index = 0;
	free_blocks(object);


}


static void _end_frame_state(obj_gp*  object)
{
	/*Se detecto el caracter de finalizacion*/
	object->state = END_MESSAGE;
	/*Escribir la logica de lo que debe pasar*/


	//agrego caracter nulo al final de la string
	save_char(object,0);


	//mandar a queue_print el buffer
	xQueueSend(queue_print,&(object->BLOCKS_MEMORY[0][0]),portMAX_DELAY);


}


static void	 _processing_input(obj_gp*  object,char c)
{
	/*Se detecto un caracter cualquiera.*/
	 object->state = PROCESSING;
	/*Escribir la logica de lo que debe pasar*/

	//guardar el caracter en el buffer
	save_char(object,c);

}







/*Funciones publicas*/




/*Inicio el modulo*/

void gp_init(obj_gp*  object,uartMap_t uart)
{


	printf("iniciamos el modulo \n");
	//inicio tarea impresion
	init_print_manager();

	object->uart = uart;
		/*Se inicia la uart*/
	uartConfig(uart,BAUDRATE );

	uartCallbackSet(uart, UART_RECEIVE, _ISR_Processing,(void*)object);
	   // Habilito todas las interrupciones de UART_USB
	uartInterrupt(UART_USB, true);


	 //inicia la maquina de estados
    _init_state_machine(object);



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
			_processing_input(object,input_char);
			break;

	}
}





/*Pero puede que nos convenga o nos pidan que hagamos un modulo por cada capa de abstraccion*/



