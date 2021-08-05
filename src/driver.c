/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/07/09
 *
 *===========================================================================*/


#include "driver.h"
#include "uartIRQ.h"

#include "check_functions.h"
#include "msg_format.h"

/*==================================Declaracion Defines============================*/

#define CHECK_LED 	LED1


/*============================Declaracion de funciones privadas====================*/





/*Recibe el bloque enviado por queue, debe verificar CRC8 y demas verificaciones
 *
 * Se usa la memoria dinamica del bloque recibido, se escribe el nuevo contenido y se vuelve a enviar para transmitir.
 * Una vez transmitido, liberar.
 * */

static void send_block(char* block,driver_t* driver);


/*================================Funciones publicas==============================*/

 void driver_Task(void* params)
{
	char* buffer;
	driver_t* driver = (driver_t*) params;

	while(1)
	{
		gpioToggle(CHECK_LED);

		/*Se recibiran los bloques de datos mediante queue onRxQueue (se considera capa 2 o 3)*/
		xQueueReceive( driver->onRxQueue,&buffer,portMAX_DELAY ); //Se espera a que venga un bloque por la cola
		bool check_ok = false;
		if(buffer !=NULL) //No DEBERIA RECIBIR NULL,pero conviene validar
		{
			/*checkeo formato, secuencia y CRC*/
			check_ok = check_block(buffer);

			if(check_ok)
			{
				/*
				 * Se envia el bloque a transmision
				 * Solo se cambia formato si el contenido del block es valido. Se libera bloque en la funcion
				 */
				change_format(buffer);

				send_block(buffer,driver);
			}

			else
			{
				/*Si el formato no es valido, se descarta el mensaje y libera la memoria*/
				free_block (driver,buffer);
			}
		}
		gpioToggle(CHECK_LED);
	}
}





bool_t driver_init(driver_t* driver)
{
	bool_t res=true;

	/*Iniciamos el hardware del puerto UART con el baudrate seleccionado*/

	uartConfig(driver->uart,driver->baudrate);

	/*Creamos los timers que haran las funciones de time out de la transmicion y recepcion*/

	driver->flow.onTxTimeOut = xTimerCreate( "TX Time Out", PROTOCOL_TIMEOUT,pdFALSE, ( void* ) driver,( void* ) onTxTimeOutCallback );

	SIMPLE_ASSERT((driver->flow.onTxTimeOut));

	driver->flow.onRxTimeOut  = xTimerCreate( "RX Time Out", PROTOCOL_TIMEOUT,pdFALSE,(void*)driver,onRxTimeOutCallback );

	SIMPLE_ASSERT((driver->flow.onRxTimeOut));

	/*
	 * Se pide un bloque del pool
	 */
	driver->flow.rxBlock = ( char* ) QMPool_get( &driver->memory.pool,0 );
	// Creamos la cola para se�alizar la recepcion de un dato valido hacia la aplicacion.
	driver->onRxQueue = xQueueCreate( POOL_TOTAL_BLOCKS, sizeof( char* ) );

	SIMPLE_ASSERT((driver->onRxQueue));

	/*
	 * Creo una cola donde van a ir los bloque que tengo que mandar por UART
	 * La cola va a tener tantos elementos como bloques de memoria pueda tener el pool
	 */
	driver->onTxQueue = xQueueCreate( POOL_TOTAL_BLOCKS, sizeof( char* ) );

	SIMPLE_ASSERT((driver->onTxQueue));


	/*Reservo memoria para el memory pool*/
	driver->memory.p_memory_pool= ( char* ) pvPortMalloc( POOL_SIZE * sizeof( char ) );

	SIMPLE_ASSERT((driver->memory.p_memory_pool));


	/*Inicio el pool de memoria*/

	QMPool_init(&(driver->memory.pool),(void*)driver->memory.p_memory_pool,POOL_SIZE* sizeof(char),BLOCK_SIZE);

	driver-> flow.rxLen=0;
	driver-> flow.txLen =0;
	driver->flow.tx_counter=0;


	/*Pido el primer bloque de memoria*/
	driver->flow.rxBlock = (char*) QMPool_get(&(driver->memory.pool),0);

	//Activo interrupciones

	rxInterruptEnable( driver);

	// Habilitamos todas las interrupciones de la UART seleccionada.
	uartInterrupt( driver->uart, TRUE );
	printf("finalizo configuracion\n");
	return res;

}

/*
 * Funcion testigo
 */

void led_task(void* params)
{

	while(1)
	{
		gpioToggle(LED3);
		vTaskDelay( 200 );

	}

}






/*================================Funciones privadas================================*/



static void send_block(char* block,driver_t* driver)
{
	xQueueSend( driver->onTxQueue, &block, portMAX_DELAY ); //Envio a la cola de transmision el blocke a transmitir
	taskENTER_CRITICAL();  //no permito que se modifique txcounter
	if ( driver->flow.tx_counter == 0 ) //si se esta enviando algo no llamo a la interrupcion para no interrumpir el delay
	 {
	   txInterruptEnable( driver );
	 }
	taskEXIT_CRITICAL();
	uartSetPendingInterrupt( driver->uart );

}



