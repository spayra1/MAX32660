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
 * @brief   	I2C Memory Script
 * @author		Syamantak Payra
 * @date		April 19, 2020
 * @details 	This script uses the I2C Master on the MAX32660 to read/write from/to the 24LC128 EEPROM.
 * 				Warning: page limits not tested
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mxc_config.h"
#include "i2c.h"
#include "mxc_delay.h"

#include "mxc_sys.h"
#include "nvic_table.h"
#include "tmr.h"
#include "tmr_utils.h"
#include "gpio.h"
#include "led.h"
#include "i2c_bus.h"

/***** Definitions *****/
#define I2C_MASTER	    MXC_I2C0
#define I2C_MASTER_IDX	0
#define I2C_MEM_ADDR	(0x50 << 1)
#define I2C_TIMEOUT     MXC_DELAY_MSEC(1)
#define PAGE_LIMIT		16



/***** Globals *****/
i2c_req_t i2c_req;
i2c_cfg_t i2c_cfg;

#define BUFLENGTH 1 // this may need to be changed? not sure of the value
uint8_t rxbuf[BUFLENGTH];
uint8_t txbuf[BUFLENGTH];

uint8_t         sourceData[] = {0x1A, 0x2A, 0x4A, 0x8A,0x1A, 0x2A, 0x4A, 0x8A,0x1A, 0x2A, 0x4A, 0x8A,0x1A, 0x2A, 0x4A, 0x8A};
uint8_t         readBuffer[16];
uint8_t 		readByte;

int r = 0;

volatile int i2c_flag;

/***** Functions *****/

//Master interrupt handler
void I2C0_IRQHandler(void)
{
    I2C_Handler(I2C_MASTER);
    return;
}

//I2C callback 
void i2c_callback(i2c_req_t *i2c_req, int error)
{
    i2c_flag = error;
    return;
}

uint8_t timeOut = 0;

int I2C_ByteWrite(uint8_t *dataAddress, uint8_t dataByte, uint8_t addlen)
{
	uint8_t rc;
	uint8_t writeBuffer[PAGE_LIMIT+3];
	uint8_t buflen;

	//Copy address bytes to the write buffer so it can be sent first
    for(int i = 0; i < addlen; i++)
    {
        writeBuffer[i] = dataAddress[i];
    }

   //Check if this is an address write or a data write.
   if(dataByte != NULL)
   {
       writeBuffer[addlen] = dataByte;
       buflen = addlen+1; // add one byte (dataByte) to buffer length
   }
   else
       buflen = addlen;

   const uint8_t* buffPoint = &writeBuffer;

   printf("MAX32660 writes data to EEPROM.\n");
   	if((rc = I2C_MasterWrite(I2C_MASTER, I2C_MEM_ADDR, buffPoint, buflen, 0)) != buflen) { // with restart
   		printf("\tError writing %d\n", rc);
   		return 1;
   	} else {
   		printf("\tSuccessfully wrote %d bytes (%d bytes address and %d bytes data)\n", buflen, addlen, (buflen-addlen));
   		return 0;
   	}
}

int I2C_ByteRead(uint8_t *dataAddress,uint8_t dataByte, uint8_t addlen)
{
	uint8_t rc;
    int check;

    printf("MAX32660 reads data from EEPROM.\n");

    //Write the address to the slave
    printf("\tQuerying address:\n\t");
    check = I2C_ByteWrite(dataAddress,NULL,addlen);

    //If not successful, return to function
    if(check == 1) {
    	printf("Check not successful");
        return 1; // check for this in error
    } else {
		if((rc = I2C_MasterRead(I2C_MASTER, I2C_MEM_ADDR, readBuffer, 1, 0)) != 1) { // with restart
			printf("Error reading %d\n", rc);
			return 1;
		} else {
			readByte = readBuffer[0];
			printf("\tSuccessfully read data.\n");
			printf("Data byte read was 0x%X (%d)\n",readByte,readByte);
			return 0;
		}
    }
}

// BufferWrite and BufferRead not ironed out yet
int I2C_BufferWrite(uint8_t *dataAddress, uint8_t *dataBuffer, uint8_t addlen, uint8_t buflen)
{
	uint8_t rc;
    uint8_t writeBuffer[PAGE_LIMIT+3];

    //Set Address as the bytes to be written first
    for(int i = 0; i < addlen; i++)
    {
        writeBuffer[i] = dataAddress[i];
    }

    //Ensure that the page limit is not breached so as to avoid overwriting other data
    if(buflen > PAGE_LIMIT)
        buflen = PAGE_LIMIT;

    //Copy data bytes to write buffer
    for(int j = 0; j < buflen; j++)
    {
        writeBuffer[addlen+j] = dataBuffer[j];
    }
    //Set up for ACK polling
    timeOut = 0;

    const uint8_t* buffPoint = &writeBuffer;

    printf("MAX32660 writes data to EEPROM.\n");
	if((rc = I2C_MasterWrite(I2C_MASTER, I2C_MEM_ADDR, buffPoint, buflen+addlen, 0)) != (buflen+addlen)) { // with restart
		printf("\tError writing %d\n", rc);
		return 1;
	} else {
		printf("\tSuccessfully wrote %d bytes (%d bytes address and %d bytes data)\n", buflen, addlen, (buflen-addlen));
		return 0;
	}
}

int I2C_BufferRead(uint8_t *dataAddress, uint8_t *dataBuffer, uint8_t addlen,uint8_t buflen)
{
	uint8_t rc;
    int check;

    printf("MAX32660 reads data from EEPROM.\n");

    //Write the address to the slave
    printf("\tQuerying address:\n\t");
    check = I2C_ByteWrite(dataAddress,NULL,addlen);

    //Set up for ACK polling
    timeOut = 0;

    //If not successful, return to function
	if(check == 1) {
		printf("Check not successful");
		return 1; // check for this in error
	} else {
		if((rc = I2C_MasterRead(I2C_MASTER, I2C_MEM_ADDR, dataBuffer, buflen, 0)) != buflen) { // with restart
			printf("Error reading %d\n", rc);
			return 1;
		} else {
			printf("\tSuccessfully read data.\nRead buffer is:\t\t");
			print_buffer(&readBuffer);
			return 0;
		}
	}


}

void print_buffer(uint8_t* buffPoint)
{
    int i;
    for(i = 0; i < 16; ++i) {
        printf("%d\t", buffPoint[i]);
    }
    printf("\n");
    return;
}

// *****************************************************************************
int main(void)
{
	int error;

	//-- i2c bus establish info -------------------------------------------
	i2c_cfg.i2c_role 	= I2C_MASTER;
	i2c_cfg.i2c_regs	= MXC_I2C0;
	i2c_cfg.i2c_speed	= I2C_STD_MODE;		//-- 100MHz
	i2c_cfg.i2c_addr	= I2C_MEM_ADDR;	//-- i2c address

	//-- i2c request for asynchronous read --------------------------------
    i2c_req.addr 		= I2C_MEM_ADDR;
    i2c_req.tx_data 	= txbuf;
    i2c_req.tx_len 		= 32;
    i2c_req.rx_data 	= rxbuf;
    i2c_req.rx_len 		= 32;
    i2c_req.restart 	= 0;
    i2c_req.callback 	= i2c_callback;	//-- registering i2c_callback function

    //Setup the I2CM
    I2C_Shutdown(I2C_MASTER);
    if((error = I2C_Init(I2C_MASTER, I2C_STD_MODE, &i2c_cfg)) != E_NO_ERROR) {
        printf("Error initializing I2C%d.  (Error code = %d)\n", I2C_MASTER_IDX, error);
        return 1;
    }
    NVIC_EnableIRQ(I2C0_IRQn);


    printf("\n***** I2C Memory Script *****\n");
    printf("This example uses the MAX32660 I2C Master to read/write from the 24LC128 EEPROM Memory Chip.\n");

    uint8_t input = 0x5B;
    uint8_t addressBuffer[] = {0xAB, 0x10};
    printf("\n\nWriting one byte (0x%X, equivalent to %d) at address {0x%X,0x%X}:\n", input, input, addressBuffer[0],addressBuffer[1]);
    //Writes a byte of data to address specified
    r = I2C_ByteWrite(&addressBuffer,input,sizeof(addressBuffer));
    //Reads a byte of data stored at the address specified
    r = I2C_ByteRead(&addressBuffer,&readByte,sizeof(addressBuffer));

    printf("\n\nWriting data buffer to address {0x%X,0x%X}\n", input, input, addressBuffer[0],addressBuffer[1]);
    printf("Input buffer is: \t"); print_buffer(&sourceData);
    //Write a specified number of data bytes beginning at the specified address
    I2C_BufferWrite(&addressBuffer,&sourceData,sizeof(addressBuffer),16);
    //Reads a specified number of data bytes beginning at the specified address
    I2C_BufferRead(&addressBuffer,&readBuffer,sizeof(addressBuffer),16);

}
