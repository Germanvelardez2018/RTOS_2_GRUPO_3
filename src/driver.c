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

#define CHECK_LED 	LED1


/*============================Declaracion de funciones privadas====================*/





/*Recibe el bloque enviado por queue, debe verificar CRC8 y demas verificaciones
 *
 * Se usa la memoria dinamica del bloque recibido, se escribe el nuevo contenido y se vuelve a enviar para transmitir.
 * Una vez transmitido, liberar.
 * */


static void  insert_error_msg(char* b, int8_t x);

static void add_crc_at_block(char* block);





/*================================Funciones publicas==============================*/

#define AO_SIZE			4
#define N_ELEMENTS		4

typedef enum
{
	AO_SNAKE,
	AO_CAMEL,
	AO_PASCAL,
	AO_ERROR
}OBJECT_INDEX;

 void driver_task(void* params)
{
	char* block;
	driver_t* driver = (driver_t*) params;

	ao_base_t ao_snake = {.state = AO_OFF};
	ao_base_t ao_camel = {.state = AO_OFF};

	ao_base_t ao_pascal = {.state = AO_OFF};

	ao_base_t ao_error = {.state = AO_OFF};



	ao_base_t*  active_objects[AO_SIZE]={
				&ao_snake,
				&ao_camel,
				&ao_pascal,
				&ao_error
	};

	callback_ao_t  callbacks[AO_SIZE]={
			set_snake,
			set_camel,
			set_pascal,
			NULL

	};

	uint8_t index = 0;

	while(1)
	{
		gpioToggle(CHECK_LED);

		/* Se recibiran los bloques de datos mediante queue onRxQueue (se considera capa 2 o 3)
		 * Se espera a que venga un bloque por la cola*/
		xQueueReceive( driver->onRxQueue,&block,portMAX_DELAY );

		//checkeo el block, si error se crea ao error
			errorCodes_t checkOk = BLOCK_OK;
			//declaro un obj activo
			if(checkOk != BLOCK_OK)
			{
				//creo el obj activo de error y le paso la callback de error
				// 				insert_error_msg(buffer,checkOk);

			}

			// si no error de formato
			else
			{
				char C = block[N_ELEMENTS];
				switch(C)
				{
					case FPASCAL:
						index = AO_PASCAL;

						//creo objeto activo pascal. Antes de la creacion debo enviarle el block a la queue del objeto activo
						break;
					case FCAMEL:
						index = AO_CAMEL;

						//creo objeto activo  camel
						break;
					case FSNAKE:
						index = AO_SNAKE;

						//creo objeto activo snake
						break;
					default:
						break;
				}
				if(active_objects[index]->state ==AO_OFF)
				{
					create_ao(active_objects[index],callbacks[index],0);
				}
				 post_AO(active_objects[index],block);

			}



		gpioToggle(CHECK_LED);
	}
}





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

	/* Se pide un bloque del pool*/
	driver->flow.rxBlock = ( char* ) QMPool_get( &driver->memory.pool,0 );

	/* Se crea la cola para se�alizar la recepcion de un dato valido hacia la aplicacion.*/
	driver->onRxQueue = xQueueCreate( POOL_TOTAL_BLOCKS, sizeof( char* ) );

	SIMPLE_ASSERT((driver->onRxQueue));

	/* Se crea una cola donde van a ir los bloque que se tienen que mandar por UART
	 * La cola va a tener tantos elementos como bloques de memoria pueda tener el pool*/
	driver->onTxQueue = xQueueCreate( POOL_TOTAL_BLOCKS, sizeof( char* ) );

	SIMPLE_ASSERT((driver->onTxQueue));

	/* Se reserva memoria para el memory pool*/
	driver->memory.p_memory_pool= ( char* ) pvPortMalloc( POOL_SIZE * sizeof( char ) );

	SIMPLE_ASSERT((driver->memory.p_memory_pool));


	/* Se inicializa el pool de memoria*/
	QMPool_init(&(driver->memory.pool),(void*)driver->memory.p_memory_pool,POOL_SIZE* sizeof(char),BLOCK_SIZE);

	driver-> flow.rxLen=0;
	driver-> flow.txLen =0;
	driver->flow.tx_counter=0;


	/* Se pide el primer bloque de memoria*/
	driver->flow.rxBlock = (char*) QMPool_get(&(driver->memory.pool),0);

	/*Se activan las interrupciones*/
	rxInterruptEnable( driver);

	/* Se habilitan todas las interrupciones de la UART seleccionada*/
	uartInterrupt( driver->uart, TRUE );

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





static void  insert_error_msg(char* b, int8_t x)
{


	b[OFFSET_MSG]='E';
	int_to_ASCII(x, (b + OFFSET_MSG + 1));
	b[OFFSET_MSG+3]='\0';

}

static void add_crc_at_block(char* block)
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



