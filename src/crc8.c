/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/07/09
 *
 *===========================================================================*/
#include "crc8.h"


/*
 *
 * ANCII-------------------->DECIMAL
 * 	A = 10		     		   41
 *  B						   42
 *  C						   43
 *  D						   44
 *  E                          45
 *  F = 15                     46
 *si char es menor que 40 entonces es un numero de 0 a 9
 *--------------si -------------------------
 *  0						   30
 *  1						   31
 *  2                          32
 *  9						   39
 * */





/*
 * @brief pasamos un char[2] a un int8_t:
 * 	Ejemplo:      "02" -> 0x02
 *
 * @param     CRC8 son 2 char
 * */

uint8_t ASCII_to_int(char* CRC8)
{
	uint8_t crc8;

	uint8_t d_1 = (((uint8_t)CRC8[0])<40)?((uint8_t)CRC8[0]) -30 :((uint8_t)CRC8[0]) -31 ;
	uint8_t d_2 = (((uint8_t)CRC8[1])<40)?((uint8_t)CRC8[1]) -30 :((uint8_t)CRC8[1]) -31 ;

	crc8 = d_1 * 16 + d_2;
	return crc8;
}



/*
 * @brief pasamos un int8_t a un char[2]
 * 	Ejemplo:      0x02 -> "02"
 *
 * @param     CRC8 es un int8_t
 * */

void  int_to_ASCII(uint8_t CRC8,char* crc8)
{
	//falta implementar, sera necesario cuando quiera transmitir datos
}



/*
 * @brief calculamos crc8
 * @source https://www.it-swarm-es.com/es/c/crc-definitivo-para-c/1070869379/
 * */

uint8_t calculate_crc8(char* data,uint8_t size)
{
	uint8_t crc;
    if (data == NULL)
        return 0;
    crc = ~crc & 0xff;
    while (size--)
    {
        crc ^= *data++;
        for (uint8_t k = 0; k < 8; k++)
            crc = crc & 1 ? (crc >> 1) ^ 0xb2 : crc >> 1;
    }
    return crc ^ 0xff;
}



