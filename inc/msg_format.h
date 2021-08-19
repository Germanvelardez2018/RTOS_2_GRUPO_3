/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/07/09
 *
 *===========================================================================*/

#ifndef RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_MSG_FORMAT_H_
#define RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_MSG_FORMAT_H_

/*==================================Declaracion Defines============================*/

#define SOM '(' //Start of message character
#define EOM ')' //End of message character

#define FRAME_MAX_SIZE 200
#define FRAME_MIN_SIZE 7

#define CRC_SIZE 2
#define SECUENCE_SIZE 4
#define FORMAT_DESIGNATOR_POSITION SECUENCE_SIZE //Posición del buffer en donde se encuentra el caracter que indica el formato al cual convertir los datos (puede ser 'P', 'C' o 'S')
#define START_DATA_POSITION 5

//Format cases

#define FPASCAL 'P'
#define FCAMEL 'C'
#define FSNAKE 'S'

/*==========================Declaracion de funciones publicas=====================*/

void set_pascal(char *block);

void set_camel(char *block);

void set_snake(char *block);

#endif /* RTOS2_REPOSITORIO_OFICIAL_REPOSITORIO_GIT_INC_MSG_FORMAT_H_ */
