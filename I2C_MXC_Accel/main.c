/*******************************************************************************
 * some source code Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
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
 * @brief   	I2C Accelerometer Script
 * @details 	This example uses the I2C Master to read/write from/to the MXC401xB accelerometer sensor
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
#include "lm75a.h"
#include "i2c_bus.h"

/***** Definitions *****/
#define I2C_MASTER	    MXC_I2C0
#define I2C_MASTER_IDX	0
#define I2C_MEM_ADDR	(0x50 << 1)
#define I2C_ACC_ADDR	(0x15 << 1)
#define I2C_TEMP_ADDR	(0x48<<1)
#define I2C_TIMEOUT     MXC_DELAY_MSEC(1)
#define PAGE_LIMIT		16


/***** Globals *****/
i2c_req_t i2c_req;
i2c_cfg_t i2c_cfg;

#define BUFLENGTH 32
uint8_t rxbuf[BUFLENGTH];
uint8_t txbuf[BUFLENGTH];
#define STORETEMPS 16

uint8_t         readBuffer[STORETEMPS];
uint8_t 		readByte;
uint8_t timeOut = 0;
int r = 0;
uint8_t address[] = {0xAB, 0x10}; // this is a random page in the EEPROM that we're choosing to write to

uint8_t devID_register = 0x0E; // register for device ID
uint8_t TOUT_register = 0x09;
uint8_t ACC_register0 = 0x00;
uint8_t XOUT_register = {0x04, 0x05};
uint8_t ACC_reg_addr_size = sizeof(TOUT_register);
uint8_t axis_reg_addr_size = sizeof(XOUT_register);
#define STOREACC 2
uint8_t accBuffer[STOREACC];

volatile int i2c_flag;
volatile int timer_flag;

#define CONT_LED_IDX 0
#define INTERVAL_TIME_CONT 3
#define CONT_TIMER	MXC_TMR0

/***** Functions *****/
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')
// printf("Leading text "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(byte));
// printf("m: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(m>>8), BYTE_TO_BINARY(m));
#define TWOBYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define TWOBYTE_TO_BINARY(byte)  \
  (byte & 0x8000 ? '1' : '0'), \
  (byte & 0x4000 ? '1' : '0'), \
  (byte & 0x2000 ? '1' : '0'), \
  (byte & 0x1000 ? '1' : '0'), \
  (byte & 0x800 ? '1' : '0'), \
  (byte & 0x400 ? '1' : '0'), \
  (byte & 0x200 ? '1' : '0'), \
  (byte & 0x100 ? '1' : '0'), \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

int I2C_ByteWrite(uint8_t *deviceAddress, uint8_t *dataAddress, uint8_t dataByte, uint8_t addlen)
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
   	if((rc = I2C_MasterWrite(I2C_MASTER, deviceAddress, buffPoint, buflen, 0)) != buflen) { // with restart
   		printf("\tError writing %d\n", rc);
   		return 1;
   	} else {
   		printf("\tSuccessfully wrote %d bytes (%d bytes address and %d bytes data)\n", buflen, addlen, (buflen-addlen));
   		return 0;
   	}
}

int I2C_ByteRead(uint8_t *deviceAddress, uint8_t *dataAddress, uint8_t dataByte, uint8_t addlen)
{
	uint8_t rc;
    int check;

    printf("MAX32660 reads data from EEPROM.\n");

    //Write the address to the slave
    printf("\tQuerying address: %X\n\t", deviceAddress);
    check = I2C_ByteWrite(deviceAddress, dataAddress,NULL,addlen);

    //If not successful, return to function
    if(check == 1) {
    	printf("Check not successful");
        return 1; // check for this in error
    } else {
		if((rc = I2C_MasterRead(I2C_MASTER, deviceAddress, readBuffer, 1, 0)) != 1) { // with restart
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

int I2C_BufferWrite(uint8_t *deviceAddress, uint8_t *dataAddress, uint8_t *dataBuffer, uint8_t addlen, uint8_t buflen)
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
	if((rc = I2C_MasterWrite(I2C_MASTER, deviceAddress, buffPoint, buflen+addlen, 0)) != (buflen+addlen)) { // with restart
		printf("\tError writing %d\n", rc);
		return 1;
	} else {
		printf("\tSuccessfully wrote %d bytes (%d bytes address and %d bytes data)\n", buflen, addlen, (buflen-addlen));
		return 0;
	}
}

int I2C_BufferRead(uint8_t *deviceAddress, uint8_t *dataAddress, uint8_t *dataBuffer, uint8_t addlen,uint8_t buflen)
{
	uint8_t rc;
    int check;

    printf("MAX32660 reads data from EEPROM.\n");

    //Write the address to the slave
    //printf("\tQuerying address:\n\t");
    check = I2C_ByteWrite(deviceAddress, dataAddress,NULL,addlen);

    //Set up for ACK polling
    timeOut = 0;

    //If not successful, return to function
	if(check == 1) {
		printf("Check not successful");
		return 1; // check for this in error
	} else {
		if((rc = I2C_MasterRead(I2C_MASTER, deviceAddress, dataBuffer, buflen, 0)) != buflen) { // with restart
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
        printf("0x%X\t", buffPoint[i]);
    }
    printf("\n");
    return;
}

void print_binary_buffer(uint8_t* buffPoint)
{
    int i;
    for(i = 0; i < 16; ++i) {
    	printf(""BYTE_TO_BINARY_PATTERN"\t", BYTE_TO_BINARY(buffPoint[i]));
    }
    printf("\n");
    return;
}

int acc_convert(uint8_t* buffPoint, int start, int end){
    int i;
    uint16_t value;
	uint16_t real_value;
	for(i = start; i < end+1; ++i) {
		printf(""BYTE_TO_BINARY_PATTERN"\t", BYTE_TO_BINARY(buffPoint[i]));
	}
	uint16_t MSB = (uint16_t)(buffPoint[start] << 8); printf("MSB: "TWOBYTE_TO_BINARY_PATTERN"\t", TWOBYTE_TO_BINARY(MSB));
	uint16_t LSB = (uint16_t)(buffPoint[end]); printf("LSB: "TWOBYTE_TO_BINARY_PATTERN"\t", TWOBYTE_TO_BINARY(LSB));
	value = MSB|LSB;
	value >>= 4;

	printf("value: "TWOBYTE_TO_BINARY_PATTERN"\n", TWOBYTE_TO_BINARY(value));
	if(value & 0x800) {
		//-- when sign bit is set, set upper unused bits, then 2's complement
		value |= 0xF800;
		value = ~value+1;
		real_value = (float) value * (-1);
	} else {
		real_value = (float) value;
	}
	return(real_value);
}

void decode_gravity(uint8_t* buffPoint){
	uint16_t Xval, Yval, Zval;
	Xval = acc_convert(buffPoint, 3, 4);
	Yval = acc_convert(buffPoint, 5, 6);
	Zval = acc_convert(buffPoint, 7, 8);
	printf("X value: %d\t", Xval);
	printf("Y value: %d\t", Yval);
    printf("Z value: %d\t", Zval);
    printf("\n");
    return;
}


void	measurement_loop(void)	//-- measurement loop
{
	float temp=0.0;

	uint8_t xBytes[2];
	uint8_t yBytes[2];
	uint8_t zBytes[2];
	uint8_t tempBytes[2];

	uint8_t printIndices[STORETEMPS];
	int increment = 0;

	//-- do nothing all measurement shall be done by timer interrupt
	while(1) {
		while(timer_flag != 1);	//-- wait until timer interrupt
		if(timer_flag==1) {
			timer_flag = 0;

//			I2C_BufferRead(I2C_ACC_ADDR,&devID_register,&readBuffer,sizeof(devID_register),STORETEMPS);
//			I2C_BufferRead(I2C_ACC_ADDR,&XOUT_register,&readBuffer,sizeof(XOUT_register),STORETEMPS);
			I2C_BufferRead(I2C_ACC_ADDR,&ACC_register0,&readBuffer,sizeof(ACC_register0),STORETEMPS);
			print_binary_buffer(&readBuffer);
			decode_gravity(&readBuffer);

//			uint8_t indexAddress[2]; indexAddress[0] = address[0]; indexAddress[1] = address[1];
//	        indexAddress[1] = address[1]+increment;
//
//			get_tempByte(tempBytes);
//			I2C_BufferWrite(&indexAddress,&tempBytes,sizeof(address),2);
//			temp = convert_temp(tempBytes);
//			printf("Measured temperature %f\n", temp);
//			I2C_BufferRead(&address,&readBuffer,sizeof(address),STORETEMPS);
//
//			for(int i = 0; i<STORETEMPS; i+=2){
//				if(increment-i >= 0){
//					printIndices[i] = readBuffer[increment - i];
//					printIndices[i+1] = readBuffer[increment - i + 1];
//				} else {
//					printIndices[i] = readBuffer[STORETEMPS + increment - i];
//					printIndices[i+1] = readBuffer[STORETEMPS + increment - i + 1];
//				}
//			}
//			for(int i = 0; i<STORETEMPS; i+=2){
//				tempBytes[0] = printIndices[i]; tempBytes[1] = printIndices[i+1];
//				if (!(tempBytes[0]==tempBytes[1] && tempBytes[1]==0)){
//					temp = convert_temp(tempBytes);
//					printf("temp %d seconds ago: %f\n", i/2, temp);
//				}
//			}
//
//
//			increment+=2;
//			if (increment>=STORETEMPS){
//				increment=0;
//			}
			printf("\n\n");
		}
	}
}

void ContinousTimer_Handler(void)
{
	//-- clear interrupt ----
	TMR_IntClear(MXC_TMR0);
	LED_Toggle(CONT_LED_IDX);
	timer_flag=1;
}

//-- ContinousTimer() ----------------------------------------------------------------
void ContinuousTimer(void)
{
	tmr_cfg_t	tmr;
	uint32_t 	period_ticks = PeripheralClock/4*INTERVAL_TIME_CONT;

	//-- initial led state is off
	LED_Off(CONT_LED_IDX);

	TMR_Disable(CONT_TIMER);			//-- Disable the timer
	TMR_Init(MXC_TMR0, TMR_PRES_4, 0);	//-- Set the prescale value

	tmr.mode = TMR_MODE_CONTINUOUS;
	tmr.cmp_cnt = period_ticks;
	tmr.pol = 0;
	TMR_Config(MXC_TMR0, &tmr);

	TMR_Enable(MXC_TMR0);

	printf("Continuous timer started and will be interrupted every one second\n");
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

// *****************************************************************************
//-- main(): init I2C communication and report temperature every designated seconds.
int main(void)
{
	int error;

	printf("\n\nMXC401xB Initialization Process started....\n\n");

	//-- setup continuous timer --------------------------------------------
	NVIC_SetVector(TMR0_IRQn, ContinousTimer_Handler);
	NVIC_EnableIRQ(TMR0_IRQn);
	ContinuousTimer();	//-- Install timer function

	//-- i2c bus establish info -------------------------------------------
	i2c_cfg.i2c_role 	= I2C_MASTER;
	i2c_cfg.i2c_regs	= MXC_I2C0;
	i2c_cfg.i2c_speed	= I2C_STD_MODE;		//-- 100MHz
	i2c_cfg.i2c_addr	= I2C_ACC_ADDR;	//-- i2c address

	//-- i2c request for asynchronous read --------------------------------
    i2c_req.addr 		= I2C_ACC_ADDR;
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

//	//-- set lm75a configuration
//	lm75a_cfg.cfgchar=0x10;
//
//	//-- lm75a id string (0xA1) ------------------------------------------
//	get_id(); 				//-- get board id ----------------
//	/*
//	lm75a_cfg.cfgchar = get_config();	//-- get current configuration ---
//	printf("lm75a_cfg.cfgchar : 0x%02x\n", lm75a_cfg.cfgchar);
//	*/
//
//	uint8_t blankData[STORETEMPS];
//		for (int i = 0; i < STORETEMPS; i++){
//			blankData[i]=0x00;
//		}
//
//    printf("\n\nWriting data buffer to address {0x%X,0x%X}\n", address[0],address[1]);
//    printf("Input buffer is: \t"); print_buffer(&blankData);
//    //Write a specified number of data bytes beginning at the specified address
//    I2C_BufferWrite(&address,&blankData,sizeof(address),STORETEMPS);
//    //Reads a specified number of data bytes beginning at the specified address
//    I2C_BufferRead(&address,&readBuffer,sizeof(address),STORETEMPS);

	//-- init lm75a ------------------------------------------------------
	measurement_loop();	//-- measurement loop

	return 0;
}
