/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/07/09
 *
 *===========================================================================*/


#include "driver.h"
#include "uartIRQ.h"
#include "crc8.h"
#include "check_functions.h"
#include "msg_format.h"
#include "error_msg.h"

#include <string.h>

#include "AO.h"




/*==================================Declaracion Defines============================*/

#define CHECK_LED 		LED1
#define AO_SIZE			3
#define N_ELEMENTS		5
#define C_POS			4

/*============================Declaracion de funciones privadas====================*/

 void c3_task(void* params);


/*================================Funciones publicas==============================*/




bool_t driver_init(driver_t* driver)
{

	bool_t res=true;

	/*Se inicializa el hardware del puerto UART con el baudrate seleccionado*/
	uartConfig(driver->uart,driver->baudrate);

	/*Se crean los timers que haran las funciones de time out de la transmicion y recepcion*/
	driver->flow.onTxTimeOut = xTimerCreate( "TX Time Out", PROTOCOL_TIMEOUT,pdFALSE, ( void* ) driver,( void* ) onTxTimeOutCallback );
	SIMPLE_ASSERT((driver->flow.onTxTimeOut));

	driver->flow.onRxTimeOut  = xTimerCreate( "RX Time Out", PROTOCOL_TIMEOUT,pdFALSE,(void*)driver,onRxTimeOutCallback );
	SIMPLE_ASSERT((driver->flow.onRxTimeOut));

	/* Se crea la cola para se�alizar la recepcion de un dato valido hacia la aplicacion.*/
	driver->onRxQueue = xQueueCreate( POOL_TOTAL_BLOCKS, sizeof( char* ) );
	SIMPLE_ASSERT((driver->onRxQueue));

	/* Se crea una cola donde van a ir los bloque que se tienen que mandar por UART*/
	driver->onTxQueue = xQueueCreate( POOL_TOTAL_BLOCKS, sizeof( char* ) );
	SIMPLE_ASSERT((driver->onTxQueue));

	/* Se reserva memoria para el memory pool*/
	driver->memory.p_memory_pool= ( char* ) pvPortMalloc( POOL_SIZE * sizeof( char ) );
	SIMPLE_ASSERT((driver->memory.p_memory_pool));

	/* Se inicializa el pool de memoria*/
	QMPool_init(&(driver->memory.pool),(void*)driver->memory.p_memory_pool,POOL_SIZE* sizeof(char),BLOCK_SIZE);

	driver-> flow.rxLen=0;
	driver-> flow.txLen =0;
	driver-> flow.tx_counter=0;

	xTaskCreate(c3_task,				// Funcion de la tarea a ejecutar
			(const char *) "modulo driver", 	// Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE * 8,   // Cantidad de stack de la tarea
			driver,                    	// Parametros de tarea
			tskIDLE_PRIORITY + 0,           // Prioridad de la tarea
			0);                         	// Puntero a la tarea creada en el sistema


	/* Se pide el primer bloque de memoria*/
	driver->flow.rxBlock = (char*) QMPool_get(&(driver->memory.pool),0);

	/*Se activan las interrupciones*/
	rxInterruptEnable( driver);

	/* Se habilitan todas las interrupciones de la UART seleccionada*/
	uartInterrupt( driver->uart, TRUE );


	return res;

}

/*
 * Controlar donde iria esta funcion
 * */


void add_crc_at_block(char* block)
{
	int8_t len = strlen(block);

	int8_t crc = crc8_calc(0,block,len);
	char CRC[2];
	int_to_ASCII(crc ,CRC);
	//agrego el crc

	block[len ]=CRC[0];
	block[len +1]=CRC[1];
	block[len +2]= '\0';
}




/*================================Funciones privadas================================*/


void c3_task(void* params)
{

	driver_t* driver = (driver_t*) params;

	// objetos activos para cada tipo de procesamiento posible
	ao_base_t ao_snake = { .state = AO_OFF };
	ao_base_t ao_camel = { .state = AO_OFF };
	ao_base_t ao_pascal = { .state = AO_OFF };

	ao_error_t   ao_error = {.ao_base.state = AO_OFF};


	//ARRAY para ordenarlos mejor
	ao_base_t* active_objects[AO_SIZE] = {
			&ao_snake,
			&ao_camel,
			&ao_pascal,
		 };

	callback_ao_t callbacks[AO_SIZE] =
	{
		  set_snake,
		  set_camel,
		  set_pascal,

	};

	char* block;
	uint8_t index = 0;


	while (TRUE)
	{
		gpioToggle(CHECK_LED);

		/* Se recibiran los bloques de datos mediante queue onRxQueue (se considera capa 2 o 3)
		 * Se espera a que venga un bloque por la cola*/
		xQueueReceive(driver->onRxQueue, &block, portMAX_DELAY);

		/*Se chequea el block, si da error se crea ao error*/
		errorCodes_t checkOk = BLOCK_OK;
		/*Se declara un objeto activo*/

		checkOk = check_block(block);

		if (checkOk != BLOCK_OK)
		{


			create_error_ao(&ao_error,driver,insert_error,checkOk,0);

			post_AO(&(ao_error.ao_base), block);

		}

		/*Si el bloque es correcto se le da formato*/
		else
		{


			char C = block[C_POS];
			/*Se define el objeto activo*/
			switch (C)
			{
			case FPASCAL:
				index = AO_PASCAL;
				break;
			case FCAMEL:
				index = AO_CAMEL;
				break;
			case FSNAKE:
				index = AO_SNAKE;
				break;
			default:
				break;
			}
			/*Se crea el objeto activo*/
			if (active_objects[index]->state == AO_OFF)
			{

				create_ao(active_objects[index], driver,callbacks[index], 0);

				post_AO(active_objects[index], block);


			}

		}

		gpioToggle(CHECK_LED);
	}
}


 void send_block(char* block,driver_t* driver)

{
	/*Antes de enviar el mensaje calcular CRC y agregarlo*/
	add_crc_at_block(block);
	/* Se envia a la cola de transmision el block a transmitir*/
	xQueueSend( driver->onTxQueue, &block, portMAX_DELAY );
	/* No se permite que se modifique txcounter*/
	taskENTER_CRITICAL();
	/* Si se esta enviando algo no se llama a la interrupcion para no interrumpir el delay*/
	if ( driver->flow.tx_counter == 0 ) //
	 {
	   txInterruptEnable( driver );
	 }
	taskEXIT_CRITICAL();
	uartSetPendingInterrupt( driver->uart );

}



