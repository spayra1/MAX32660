/*******************************************************************************
 * Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 * $Date: 2018-12-12 10:03:31 -0600 (Wed, 12 Dec 2018) $
 * $Revision: 39788 $
 *
 ******************************************************************************/

/**
 * @file    	main.c
 * @brief   	I2C EEPROM communication
 * @details 	This example uses the I2C Master to read/write from/to memory.
 *  			The Master will use P0.8 and P0.9.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mxc_config.h"
#include "i2c.h"
#include "mxc_delay.h"
#include "eeprom_i2c.h"

/***** Definitions *****/
#define I2C_MASTER	    MXC_I2C0
#define I2C_MASTER_IDX	0

#define I2C_MEM_ADDR	(0x50<<1)
#define I2C_TIMEOUT       MXC_DELAY_MSEC(1)


/***** Globals *****/
i2c_req_t req;

volatile int i2c_flag;

uint8_t         sourceData[] = {0x1A, 0x2A, 0x4A, 0x8A,0x1A, 0x2A, 0x4A, 0x8A,0x1A, 0x2A, 0x4A, 0x8A,0x1A, 0x2A, 0x4A, 0x8A};
uint8_t         addressBuffer[] = {0xAB,0x10} ; //Put your address here
uint8_t         readBuffer[16];
uint8_t readByte;

// *****************************************************************************
int main(void)
{
    int error, i = 0;
    const sys_cfg_i2c_t sys_i2c_cfg = NULL; /* No system specific configuration needed. */

    printf("\n***** I2C EEPROM *****\n");
    printf("This example uses the I2C Master to read/write from/to the . For\n");
    printf("this example you must connect P0.3 to P0.9 (SDA) and P0.2 to P0.8 (SCL). The\n");
    printf("Master will use P0.8 and P0.9. The Slave will use P0.2 and P0.3. \n\n");

    //Setup the I2CM
    I2C_Shutdown(I2C_MASTER);
    if((error = I2C_Init(I2C_MASTER, I2C_STD_MODE, &sys_i2c_cfg)) != E_NO_ERROR) {
        printf("Error initializing I2C%d.  (Error code = %d)\n", I2C_MASTER_IDX, error);
        return 1;
    }
    NVIC_EnableIRQ(I2C0_IRQn);

	int r = 0;

	printf("writing a byte of data\t %d", addressBuffer);
	//Writes a byte of data to address specified
	r = I2C_ByteWrite(&addressBuffer,0x5B,sizeof(addressBuffer));

	//Reads a byte of data stored at the address specified
	int read = I2C_ByteRead(&addressBuffer,&readByte,sizeof(addressBuffer));
	printf("read a byte of data\t %d", read);

	//Write a specified number of data bytes beginning at the specified address
	r = I2C_BufferWrite(&addressBuffer,&sourceData,sizeof(addressBuffer),4);

	//Reads a specified number of data bytes beginning at the specified address
	I2C_BufferRead(&addressBuffer,&readBuffer,sizeof(addressBuffer),4);

   //stop here
	while (1) {
	   ; // Add your application code
	}
}