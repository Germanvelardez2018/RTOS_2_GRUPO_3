/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/07/09
 *
 *===========================================================================*/
#ifndef M_C3_H_
#define M_C3_H_


#include "sapi.h"


/*@brief buffer estatus

 *
 * */
typedef enum {
	b_error,
	b_pass

} b_status;



typedef struct
{
	char* 	 msg;
	uint16_t secuencia;
	uint8_t  msg_size;
	char 	 format;


}
data_check;



void inicio_c3(void);



b_status procesar_buffer(char* buffer, data_check* data);


void funcion_c3(data_check* data);


b_status   check_crc8(char* buffer);


void task_c(void* vargs);

void send_msg_to_c3(char* msg);




#endif /* RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_M_C3_H_ */
