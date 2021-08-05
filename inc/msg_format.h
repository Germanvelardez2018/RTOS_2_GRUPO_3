/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/07/09
 *
 *===========================================================================*/

#ifndef RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_MSG_FORMAT_H_
#define RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_MSG_FORMAT_H_




#define 	SOM						'('		//Start of message character
#define 	EOM						')'		//End of message character
//#define		BAUDRATE				115200
#define		FRAME_MAX_SIZE  200
#define 	FRAME_MIN_SIZE	7

#define     CRC_SIZE    2
#define     SECUENCE_SIZE   4
#define     FORMAT_DESIGNATOR_POSITION  SECUENCE_SIZE       //Posición del buffer en donde se encuentra el caracter que indica el formato al cual convertir los datos (puede ser 'P', 'C' o 'S')
#define     START_DATA_POSITION         5
//FORMAT

#define FPASCAL			'P'
#define	FCAMEL			'C'
#define	FSNAKE			'S'



void change_format(char* block);




#endif /* RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_MSG_FORMAT_H_ */
