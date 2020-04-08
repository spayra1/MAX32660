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
 * @brief   	I2C (Loopback) Example 
 * @details 	This example uses the I2C Master to read/write from/to the I2C Slave. For
 * 		        this example you must connect P0.3 to P0.9 (SDA) and P0.2 to P0.8 (SCL). The Master
 * 		        will use P0.8 and P0.9. The Slave will use P0.2 and P0.3.  
 *              
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
uint8_t         addressBuffer[] = {0xAB,0x10} ; //Put your address here
uint8_t         readBuffer[16];
uint8_t readByte;

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
	uint8_t reg;
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
       buflen = addlen+1;
   }
   else
       buflen = addlen;

   printf("Master writes data to Slave.\n");
   	if((rc = I2C_MasterWrite(I2C_MASTER, I2C_MEM_ADDR, writeBuffer, buflen, 0)) != E_NO_ERROR) { // with restart
   		printf("Error writing %d\n", rc);
   		return 1;
   	}
}

uint8_t I2C_ByteRead(uint8_t *dataAddress,uint8_t dataByte, uint8_t addlen)
{
	uint8_t rc;
	uint8_t reg;
    int check;

    //Write the address to the slave
    check = I2C_ByteWrite(dataAddress,NULL,addlen);

    //If not successful, return to function
    if(check == 1)
        return 1; // check for this in error


    printf("Master reads data from Slave.\n");
    	if((rc = I2C_MasterRead(I2C_MASTER, I2C_MEM_ADDR, dataAddress, addlen, 0)) != E_NO_ERROR) { // with restart
    		printf("Error writing %d\n", rc);
    	}
}

int I2C_BufferWrite(uint8_t *dataAddress, uint8_t *dataBuffer, uint8_t addlen, uint8_t buflen)
{
	uint8_t rc;
	uint8_t reg;
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

    printf("Master writes data to Slave.\n");
    	if((rc = I2C_MasterWrite(I2C_MASTER, I2C_MEM_ADDR, writeBuffer, buflen+addlen, 0)) != E_NO_ERROR) { // with restart
    		printf("Error writing %d\n", rc);
    		return 1;
    	}
}
void I2C_BufferRead(uint8_t *dataAddress, uint8_t *dataBuffer, uint8_t addlen,uint8_t buflen)
{
	uint8_t rc;
    int check = 0;

    //Write Address from where to read
    check = I2C_ByteWrite(dataAddress,NULL,addlen);

    //check if address write is successful
    if(check == 1)
        return; // see if this works

    //Set up for ACK polling
    timeOut = 0;

    printf("Master reads data from Slave.\n");
    	if((rc = I2C_MasterRead(I2C_MASTER, I2C_MEM_ADDR, dataBuffer, buflen, 0)) != E_NO_ERROR) { // with restart
    		printf("Error writing %d\n", rc);
    	}
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

    NVIC_EnableIRQ(I2C0_IRQn);


    printf("\n***** I2C Memory Example *****\n");
    printf("This example uses the I2C Master to read/write from the 24LC128 Memory chip.\n");

    //Writes a byte of data to address specified
    r = I2C_ByteWrite(&addressBuffer,0x5B,sizeof(addressBuffer));

    //Reads a byte of data stored at the address specified
    printf(I2C_ByteRead(&addressBuffer,&readByte,sizeof(addressBuffer)));

    //Write a specified number of data bytes beginning at the specified address
    r = I2C_BufferWrite(&addressBuffer,&sourceData,sizeof(addressBuffer),4);

    //Reads a specified number of data bytes beginning at the specified address
    I2C_BufferRead(&addressBuffer,&readBuffer,sizeof(addressBuffer),4);

}
