/*
 * si1133.h
 *
 *  Created on: Oct 5, 2021
 *      Author: tanle
 */
//***********************************************************************************
// Include files
//***********************************************************************************

#ifndef HEADER_FILES_SI1133_H_
#define HEADER_FILES_SI1133_H_

/* System include statements */


/* Silicon Labs include statements */
#include <stdbool.h>

/* The developer's include statements */
#include "i2c.h"
#include "brd_config.h"
#include "HW_delay.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define PART_ID   0x00

#define STANDARD_VALUE 51


#define RESPONSE0     0x11
#define INPUT0        0x0A
#define COMMAND_REG   0x0B
#define ADC_CONFIG0   0x02
#define WHITE         0b01011
#define PARAM_SET     0b10000000
#define CHAN0_PREP    0b000001
#define CHAN_LIST     0x01
#define FORCE         0x11
#define HOSTOUT0      0x13
#define HOSTOUT1      0x14
#define HOSTOUT2      0x15
#define RESET_CMD_CTR 0x00

#define NULL_CB       0


//***********************************************************************************
// global variables
//***********************************************************************************




//***********************************************************************************
// function prototypes
//***********************************************************************************
void Si1133_i2c_open();

void Si1133_read(uint32_t bytes_expected,uint32_t register_address, uint32_t call_back);

void Si1133_write(uint32_t bytes_expected,uint32_t register_address, uint32_t call_back);

void Si1133_force_sense();

void Si1133_request_result();

uint32_t result_read();

#endif /* HEADER_FILES_SI1133_H_ */
