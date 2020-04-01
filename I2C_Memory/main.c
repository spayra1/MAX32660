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


/***** Globals *****/
i2c_req_t i2c_req;
i2c_cfg_t i2c_cfg;

#define BUFLENGTH 1 // this may need to be changed? not sure of the value
uint8_t rxbuf[BUFLENGTH];
uint8_t txbuf[BUFLENGTH];

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

uint8_t writeread(uint8_t memory_address, uint8_t data, int size) {
	uint8_t rc;
	uint8_t reg;
	reg = memory_address;

	printf("Master writes data to Slave.\n");
	if((rc = I2C_MasterWrite(MXC_I2C0, I2C_MEM_ADDR, reg, sizeof(reg), 1)) != E_NO_ERROR) { // with restart
		printf("Error writing %d\n", rc);
		while(1);
	}

	printf("Master reads data from Slave.\n");
	if((rc = I2C_MasterRead(MXC_I2C0, memory_address, data, size, 0)) != E_NO_ERROR) { // with a stop
		printf("Error reading %d\n", rc);
		while(1);
	}


  return (rc);
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

	//-- initialize i2c bus transaction ----------------------------------
	if((error=i2c_init(&i2c_cfg))!=E_NO_ERROR) {
		printf("[%d] i2c bus initialization error : %d\n", __LINE__, error);
	}

	//-- set lm75a configuration
	lm75a_cfg.cfgchar=0x10;

	//-- lm75a id string (0xA1) ------------------------------------------
	get_id(); 				//-- get board id ----------------
	/*
	lm75a_cfg.cfgchar = get_config();	//-- get current configuration ---
	printf("lm75a_cfg.cfgchar : 0x%02x\n", lm75a_cfg.cfgchar);
	*/


    printf("\n***** I2C Memory Example *****\n");
    printf("This example uses the I2C Master to read/write from the 24LC128 Memory chip.\n");


    //Setup the I2CM
    I2C_Shutdown(I2C_MASTER);
    if((error = I2C_Init(I2C_MASTER, I2C_STD_MODE, &sys_i2c_cfg)) != E_NO_ERROR) {
        printf("Error initializing I2C%d.  (Error code = %d)\n", I2C_MASTER_IDX, error);
        return 1;
    }
    NVIC_EnableIRQ(I2C0_IRQn);

    int memregister = 0x01;
    int size = sizeof(memregister);
    printf("%d", size);
    printf("Master I2C reads data to Memory.\n");
    int somesize = 4;
    int memread = writeread(memregister, txbuf, somesize);
    printf("%d", memread);

}
