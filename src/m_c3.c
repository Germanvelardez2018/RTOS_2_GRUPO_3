/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/07/09
 *
 *===========================================================================*/


/*Funciones de capa 2s*/

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "m_c3.h"

#include "string.h"
#include "stdlib.h"
#include "crc8.h"

#define N_QUEUE					5
#define Q_IN_ELEMENT_SIZE		sizeof(char*)
#define	Q_OUT_ELEMENT_SIZE		sizeof(char*)
static QueueHandle_t _queue_entrada;
static QueueHandle_t _queue_salida;









/*@brief inicio capa 3
 * inicio queues de entrada y salida
 * cargo tarea de capa 3
 *
 * */

void inicio_c3(void)
{

	_queue_entrada = xQueueCreate(N_QUEUE,Q_IN_ELEMENT_SIZE);
	//verificar que se creo queue satisfactoriamente

	_queue_salida = xQueueCreate(N_QUEUE,Q_OUT_ELEMENT_SIZE);
	//verificar que se creo queue satisfactoriamente



	/*Se carga la tarea de la capa 3*/

	xTaskCreate(task_c,                  // Funcion de la tarea a ejecutar
					(const char *) "task C3", // Nombre de la tarea como String amigable para el usuario
					configMINIMAL_STACK_SIZE * 6,   // Cantidad de stack de la tarea
					0,                    // Parametros de tarea
					tskIDLE_PRIORITY + 1,           // Prioridad de la tarea
					0                         // Puntero a la tarea creada en el sistema
					);


}


/*
 * Tarea que gestiona las responsabilidades de capa 3*/

void task_c(void* vargs)
{
	    char* new;
	    /*Estructura que separa el buffer en sus componentes*/
	    data_check data;

		while(1)
		{
			xQueueReceive(_queue_entrada,&new,portMAX_DELAY);

			  printf("[task_c]%s\n",new);

			  /*verifico validez del mensaje*/
			b_status status=  procesar_buffer(new,&data);


			if(status != b_error)
			{
				/*modifico formato del mensaje*/
				funcion_c3(new);

			}

			/*Por ultimo libero la memoria. O deberia esperar que se transmita?
			 * Use el mismo buffer para modificar el mensaje
			 * .Verificar*/
			 liberarMemoria(new);


		}
}





/*@brief tareas para enviar buffer char* a la queue de entrada
 *
 * */

void send_msg_to_c3(char* msg)
{
	xQueueSend(_queue_entrada,&msg,1000);
}



/*
 * @brief Verifica que el crc8 del buffer sea correcto
 *
 * */
b_status  check_crc8(char* buffer)
{
	b_status status = b_pass;

	char crc[2];
	uint8_t len = strlen(buffer);
	crc[0] = buffer[len -2];
	crc[1] = buffer[len-1];

	uint8_t crc_recibido = ASCII_to_int(crc);
	uint8_t crc_calculado = calculate_crc8(buffer,len-2);

	if(crc_recibido != crc_calculado)
	{
		status = b_error;
	}


	return status;

}



/*@brief recibe el buffer valido de capa 1 con el siguiente formato:
 *
 * |secuencia(4bytes)|tipo(1byte)|mensaje(variable)|crc(2bytes)
 *
 *
 *
 * */
b_status procesar_buffer(char* buffer, data_check* data)
{
	b_status status =b_pass;

	/* checkear el CRC8, recordar que el ultimo byte
	 * del buffer es 0\ y no va en el calculo
	 * */
	status = check_crc8(buffer);


	if(status == b_error)
	{
		return b_error;
	}


	int8_t size  = strlen(buffer);
	char	secuencia[4];
	strncpy(secuencia,buffer,4);
	int16_t n_secuencia = atoi(secuencia);
	// si error en conversion devuelve valor cero.
	// Que pasa si el valor es valido pero es string 0?
	if(n_secuencia == 0)
	{
		return b_error;
	}
	data->secuencia = n_secuencia;
	data->format    = buffer[4];
	//recordar que este msg_size tiene el crc al final
	data->msg_size  = buffer[5];
	//menos 4 secuencia 1 tipo y 2 crc
	data->msg_size  = size - 7;


	return status;
}


/* recibe la estructura data y cambia el formato del mensaje
 *
 * */

void funcion_c3(data_check* data){


    //pasar_a_formato generico
    //pasar_a_generico(buffer);

    switch(data->format)
    {
        case 'c':

           //cambiar_a_camel(buffer)
        break;

        case 'p':

         //cambiar_pascal(buffer)
        break;


        case 's':
        	break;

        //cambiar_snake_case(buffer);
        default:
        	break;
    }
}
