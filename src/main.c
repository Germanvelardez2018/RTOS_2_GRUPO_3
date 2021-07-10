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
#include "gp.h"











int main( void )
{
   boardInit();


   obj_gp protocolo ;

   gp_init(&protocolo,UART_USB);



   vTaskStartScheduler(); // Initialize scheduler

   while( true ); // If reach heare it means that the scheduler could not start

   // YOU NEVER REACH HERE, because this program runs directly or on a
   // microcontroller and is not called by any Operating System, as in the 
   // case of a PC program.
   return 0;
}





