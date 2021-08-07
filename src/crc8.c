/*=============================================================================
 * Authors: German Velardez <germanvelardez16@gmail.com>
 *          Federico Meghinasso <fmeghi@gmail.com>
 *          Matias Meghinasso <meghinasso@gmail.com>
 * Date: 2021/07/09
 *
 *===========================================================================*/
#include "crc8.h"


/*============================Declaracion Defines============================*/

#define ASCII_A_POS 		65
#define ASCCI_NUM_TO_INT 	48
#define ASCII_LETTER_TO_INT 55
#define BASE_DECIMAL 		10
#define BASE_HEX 			16

/*============================Declaracion Funciones Privadas================*/

uint8_t char_to_int(char hex);

char int_to_char(int numero);






/*=============================Declaracion Funciones Publicas================*/


uint8_t ASCII_to_int(char* stringCRC)
{

	return ((char_to_int(stringCRC[1])*BASE_DECIMAL) + char_to_int(stringCRC[0])) ;
}


uint8_t char_to_int(char hex)
{
	if (((uint8_t)hex) < ASCII_A_POS)
	{
		return (((uint8_t)hex) - ASCCI_NUM_TO_INT);
	}else
	{
		return (((uint8_t)hex) - ASCII_LETTER_TO_INT);
	}
}



void  int_to_ASCII(uint8_t intCRC,char* stringCRC)
{
	stringCRC[0] = int_to_char(intCRC / 16);
	stringCRC[1] = int_to_char(intCRC % 16);
}

char int_to_char(int numero)
{
	if (numero < BASE_DECIMAL)
	{
		return((char)(numero + ASCCI_NUM_TO_INT));
	}else
	{
		return((char)(numero + ASCII_LETTER_TO_INT));
	}
}


static uint8_t crc8_small_table[16] = {
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15,
    0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d
};

uint8_t crc8_init(void)
{
    return 0xff;
}

uint8_t crc8_calc(uint8_t val, void *buf, int cnt)
{
	int i;
	uint8_t *p = buf;

	for (i = 0; i < cnt; i++) {
		val ^= p[i];
		val = (val << 4) ^ crc8_small_table[val >> 4];
		val = (val << 4) ^ crc8_small_table[val >> 4];
	}
	return val;
}



