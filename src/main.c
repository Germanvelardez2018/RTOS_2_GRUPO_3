/*=============================================================================
 * Author: German Velardez <germanvelardez16@gmail.com>
 * Date: 2021/07/09
 * Version: 1.0
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "main.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "sapi.h"





#define N_QUEUE 10


/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/


static QueueHandle_t queue_print;









void init_print_manager(void);
void print_manager(void *taskParmPtr);






void myTask(void *vargs)
{
	while(1)
	{
		gpioToggle(LEDR);
		vTaskDelay(1000);
	}
}





int main( void )
{
   boardInit();

   init_print_manager();

   vTaskStartScheduler(); // Initialize scheduler

   while( true ); // If reach heare it means that the scheduler could not start

   // YOU NEVER REACH HERE, because this program runs directly or on a
   // microcontroller and is not called by any Operating System, as in the 
   // case of a PC program.
   return 0;
}



void print_manager(void *taskParmPtr)
{
    char *mensaje = NULL;

    while (1)
    {
        xQueueReceive(queue_print, &mensaje, portMAX_DELAY);
        taskENTER_CRITICAL();
        printf("%s", mensaje);
        taskEXIT_CRITICAL();
    }
}


void init_print_manager(void)
{

    BaseType_t res;

    // Creo tarea unica de impresion
    res = xTaskCreate(
        print_manager,                 // Funcion de la tarea a ejecutar
        (const char *)"print_manager", // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE * 2,  // Cantidad de stack de la tarea
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
