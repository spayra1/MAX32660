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

/***** Definitions *****/
#define I2C_MASTER	    MXC_I2C0
#define I2C_MASTER_IDX	0

#define I2C_SLAVE	    MXC_I2C1
#define I2C_SLAVE_IDX	1
#define I2C_SLAVE_ADDR	(0x51<<1)
#define I2C_TIMEOUT       MXC_DELAY_MSEC(1)


/***** Globals *****/
i2c_req_t req;
static uint8_t txdata[16];
static uint8_t rxdata[16];

volatile int i2c_flag;
volatile int i2c_flag1;

/***** Functions *****/
//Slave interrupt handler
void I2C1_IRQHandler(void) 
{
    I2C_Handler(I2C_SLAVE);
    return;
}

//Master interrupt handler
void I2C0_IRQHandler(void)
{
    I2C_Handler(I2C_MASTER);
    return;
}

//I2C callback 
void i2c_callback(i2c_req_t *req, int error)
{
    i2c_flag = error;
    return;
}

//Prints out human-friendly format to read txdata and rxdata
void print_data(void)
{
    int i;
    printf("txdata: ");
    for(i = 0; i < 16; ++i) {
        printf("%d\t", txdata[i]);
    }

    printf("\nrxdata: ");
    for(i = 0; i < 16; ++i) {
        printf("%d\t", rxdata[i]);
    }

    printf("\n");

    return;
}

//Compare txdata and rxdata to see if they are the same
void verify(void)
{
    int i, fails = 0;
    for(i = 0; i < 16; ++i) {
        if(txdata[i] != rxdata[i]) {
            ++fails;
	    }
    }
    if(fails > 0) {
        printf("Fail.\n");
    } else {
        printf("Pass.\n");
    }

    return;
}

// *****************************************************************************
int main(void)
{
    int error, i = 0;
    const sys_cfg_i2c_t sys_i2c_cfg = NULL; /* No system specific configuration needed. */

    printf("\n***** I2C Loopback Example *****\n");
    printf("This example uses the I2C Master to read/write from/to the I2C Slave. For\n");
    printf("this example you must connect P0.3 to P0.9 (SDA) and P0.2 to P0.8 (SCL). The\n");
    printf("Master will use P0.8 and P0.9. The Slave will use P0.2 and P0.3. \n\n");

    //Setup the I2CM
    I2C_Shutdown(I2C_MASTER);
    if((error = I2C_Init(I2C_MASTER, I2C_STD_MODE, &sys_i2c_cfg)) != E_NO_ERROR) {
        printf("Error initializing I2C%d.  (Error code = %d)\n", I2C_MASTER_IDX, error);
        return 1;
    }
    NVIC_EnableIRQ(I2C0_IRQn);

    //Setup the I2CS
    I2C_Shutdown(I2C_SLAVE);
    if((error = I2C_Init(I2C_SLAVE, I2C_STD_MODE, &sys_i2c_cfg)) != E_NO_ERROR) {
        printf("Error initializing I2C%d.  (Error code = %d)\n", I2C_SLAVE_IDX, error);
        return 1;
    }
    NVIC_EnableIRQ(I2C1_IRQn);	

    //MASTER WRITE SLAVE READ **********************************************
    printf("Master write, Slave read ... \n");
    //Initialize test data
    for(i = 0; i < 16; i++) {
        txdata[i] = i;
        rxdata[i] = 0;
    }

    printf("Initial test data:\n");
    print_data();

    //Prepare SlaveAsync
    req.addr = I2C_SLAVE_ADDR;
    req.tx_data = txdata;
    req.tx_len = 16;
    req.rx_data = rxdata;
    req.rx_len = 16;
    req.restart = 0;
    req.callback = i2c_callback;

    i2c_flag = 1;
    if((error = I2C_SlaveAsync(MXC_I2C1, &req)) != E_NO_ERROR) {
        printf("Error starting async read %d\n", error);
        while(1);
    }

// SCANNER CODE IS HERE. changed I2C_SLAVE_ADDR to i << 1, and != 16 to == 16, Error message %d\n, error

    for (int i = 0; i < 128; i++)
    {
//    printf("Master writes data to Slave.\n");
    printf("%x\n", i);
    if(I2C_MasterWrite(MXC_I2C0, i << 1, txdata, 16, 0) == 16) {
        printf("Address at %x\n", i);
        //while(1);
    }
    }

    //Wait for callback
    mxc_delay_start(I2C_TIMEOUT);
    do {
        if (mxc_delay_check() == E_NO_ERROR) {
            I2C_AbortAsync(&req);
        }
    } while (i2c_flag == 1);

    //Check callback flag
    if(i2c_flag != E_NO_ERROR) {
        printf("Error with i2c_callback %d\n", i2c_flag);
    }

    printf("Checking test data:\n");
    print_data();
    verify();

    //Initialize new test data
    for(i = 0; i < 16; i++) {
        txdata[i] = 15-i;
        rxdata[i] = 0;
    }
    
	//Reset tx and rx data ptrs.  Previous call to I2C_SlaveAsync modifies them.
    req.tx_data = txdata;
    req.rx_data = rxdata;

    //SLAVE WRITE MASTER READ **********************************************
    printf("Slave write, Master read ... \n");
    printf("New test data:\n");
    print_data();

    i2c_flag = 1;
    if((error = I2C_SlaveAsync(MXC_I2C1, &req)) != E_NO_ERROR) {
        printf("Error starting async read %d\n", error);
        while(1);
    }

    printf("Master reads data from Slave.\n");
    if((error = I2C_MasterRead(MXC_I2C0, I2C_SLAVE_ADDR, rxdata, 16, 0)) != 16) {
        printf("Error reading%d\n", error);
        while(1);
    }

    //Wait for callback
    mxc_delay_start(I2C_TIMEOUT);
    do {
        if (mxc_delay_check() == E_NO_ERROR) {
            I2C_AbortAsync(&req);
        }
    } while (i2c_flag == 1);

    //Check callback flag
    if(i2c_flag != E_NO_ERROR) {
        printf("Error with i2c_callback %d\n", i2c_flag);
        while(1);
    }

    printf("Checking test data:\n");
    print_data();
    verify();


    printf("\nExample complete.\n");
    while(1)
    return 0;
}
