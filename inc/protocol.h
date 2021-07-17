/* Copyright 2020, Franco Bucafusco
 * All rights reserved.
 *
 * This file is part of sAPI Library.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "sapi.h"
#include "memPool.h"

#define 	SOM						'('		//Start of message character
#define 	EOM						')'		//End of message character
//#define		BAUDRATE				115200
#define		FRAME_MAX_SIZE  200
#define 	FRAME_MIN_SIZE	7

typedef struct {
	uartMap_t uart; 			//Number of UART USED
	uint32_t baudRate; 			//UART baudrate
} uartConfig_t;

typedef struct {
	uartMap_t uart; 			//Number of UART USED
	uint32_t baudRate; 			//UART baudrate
	char buffer[FRAME_MAX_SIZE];	//Temporal buffer for saving frame received by uart
	uint16_t index;					//Counter of characters received by uart
} protocolData_t;


void init_protocol(protocolData_t* protocol_config);
void protocol_x_init( protocolData_t * uartData );
void protocol_wait_frame();
void protocol_get_frame_ref(char** data, uint16_t* size);
void protocol_discard_frame( protocolData_t * uartData );
void wait_frame(void * pvParameters);

#endif
