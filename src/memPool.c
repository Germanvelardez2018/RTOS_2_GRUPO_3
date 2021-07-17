/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/07/09
 *
 *===========================================================================*/


#include "memPool.h"
#include "string.h"

/*==========================[variables globales]=============================*/

mensaje_t pool_ptr;
QMPool pool_manager;
/*====================[Prototipos de funciones privadas]=====================*/

void imprimir(char* mensaje);

/*====================[Declaracion de funciones publicas]====================*/


void poolInit(void)
{


	pool_ptr = ( mensaje_t ) pvPortMalloc( POOL_SIZE * sizeof( mensaje_t ) );

	if ( pool_ptr == NULL )
	{
		gpioWrite( LEDR, ON );
		imprimir( "Error al crear el pool" );
		while(TRUE);
	}

	QMPool_init( &pool_manager, (mensaje_t) pool_ptr, POOL_SIZE,
			BLOCK_SIZE);
}


mensaje_t guardarMensaje(mensaje_t mensaje)
{
	mensaje_t mensaje_pool = ( mensaje_t ) QMPool_get( &pool_manager, 0 );

	if (mensaje_pool != NULL)
	{
		strcpy(mensaje_pool, mensaje);
		return mensaje_pool;
	}
	else
	{
		return NULL;
	}
}

void liberarMemoria(mensaje_t mensaje)
{
	QMPool_put( &pool_manager, mensaje );
}


/*====================Declaracion de funciones privadas================*/


void imprimir(char* mensaje)
{
	uartWriteString( UART_USB, mensaje );
}
