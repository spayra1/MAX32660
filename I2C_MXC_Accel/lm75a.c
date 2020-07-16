/*
 * lm75a.c
 *
 *  Created on: Jul 24, 2018
 *      Author: Cyberarea
 */

#include "lm75a.h"

int  	error;

extern uint8_t rxbuf[];
extern uint8_t txbuf[];

extern i2c_cfg_t i2c_cfg;
extern i2c_req_t i2c_req;
extern volatile int i2c_flag;

//-- get_id() ---------------------------------------------------------------
int 	get_id(void)
{
	uint8_t reg[2], data[2];

	reg[0]=LM75A_REG_ID;	//-- 0x07

	//-- write register value -----------------------------------------------
    if((error=i2c_write(&i2c_cfg, reg, 1, 0)) != 1) {
    	printf("[%d] Error master writing %d\n", __LINE__, error);
       	return error;
    }

    //-- read device id -----------------------------------------------------
    if((error = i2c_read(&i2c_cfg, data, 1, 0)) != 1) {
    	printf("[%d] Error reading %d\n", __LINE__, error);
    	return error;
    }

    //-- check if device id is 0xA1 -----------------------------------------
   	if(data[0] == 0xA1) printf("Get correct ID 0xA1\n");
   	printf("data[0]=0x%02x\n", data[0]);
   	return data[0];
}

//-- get_tempByte() ---------------------------------------------------------------
void 	get_tempByte(uint8_t* input)
{
	uint8_t  	reg[2], data[2];

	reg[0] = LM75A_REG_TEMP;	//-- 0x00

    if((error=i2c_write(&i2c_cfg, reg, 1, 0)) != 1) {
    	printf("[%d] Error master writing %d\n", __LINE__, error);
       	return;
    }

    if((error = i2c_read(&i2c_cfg, data, 2, 0)) != 2) {
    	printf("[%d] Error reading %d\n", __LINE__, error);
    	return;
    }

    *input = data[0]; *(input+1)=data[1];
}

//-- convert_temp() ---------------------------------------------------------------
float 	convert_temp(uint8_t* input)
{
	uint8_t data[2]; data[0] = *input; data[1] = *(input + 1);
	uint16_t	value, newvalue;
	float calc_value = 0.0;

    newvalue = (uint16_t) ((data[0] << 8) | data[1]);

    //printf("value = %u, 0x%x, data = 0x%02x, 0x%02x\n", newvalue, newvalue, data[0], data[1]);

    value = (uint16_t)newvalue;

    value >>= 5;
    if(value & 0x0100) {
    	//-- when sign bit is set, set upper unused bits, then 2's complement
    	value |= 0xF800;
    	value = ~value+1;
    	calc_value = (float) value * (-1) * LM75A_DEGREES_RESOLUTION;
    } else {
    	calc_value = (float) value * LM75A_DEGREES_RESOLUTION;
    }

    //printf("real = %x, value = %u\n", calc_value, value);
    return calc_value;
}


//-- get_temp() --------------------------------------------------------------
float 	get_temp()
{
	uint8_t  	reg[2], data[2];
	uint16_t	value;
	float real_value;

	reg[0] = LM75A_REG_TEMP;	//-- 0x00

    if((error=i2c_write(&i2c_cfg, reg, 1, 0)) != 1) {
    	printf("[%d] Error master writing %d\n", __LINE__, error);
       	return error;
    }

    if((error = i2c_read(&i2c_cfg, data, 2, 0)) != 2) {
    	printf("[%d] Error reading %d\n", __LINE__, error);
    	return error;
    }

    value = (uint16_t) ((data[0] << 8) | data[1]);

    // printf("value = %u, 0x%04x, data = 0x%02x, 0x%02x\n", value, value, data[0], data[1]);

    value >>= 5;
    if(value & 0x0100) {
    	//-- when sign bit is set, set upper unused bits, then 2's complement
    	value |= 0xF800;
    	value = ~value+1;
    	real_value = (float) value * (-1) * LM75A_DEGREES_RESOLUTION;
    } else {
    	real_value = (float) value * LM75A_DEGREES_RESOLUTION;
    }

    return real_value;
}

//-- get_T_hyst() -------------------------------------------------------------
int 	get_T_hyst()
{
	uint8_t reg[2];
	uint8_t data;
	int8_t	hyst;

	reg[0]=LM75A_REG_T_HYST;	//-- 0x02

	//-- write register value -----------------------------------------------
    if((error=i2c_write(&i2c_cfg, reg, 1, 0)) != 1) {
    	printf("[%d] Error master writing %d\n", __LINE__, error);
       	return error;
    }

    //-- now we read as the interrupt bring up callback function ------------
    i2c_req.tx_data=txbuf;
    i2c_req.rx_data=rxbuf;
   	if((error = i2c_aread(&i2c_cfg, &i2c_req)) != E_NO_ERROR) {
    	printf("[%d] Error starting async read %d\n", __LINE__, error);
    	return error;
    }

   	data = i2c_req.tx_data[0];

   	if(data & 0x80) { 	//-- check if it is signed
   		hyst = (~data+1)*(-1);
   	} else {
   		hyst = (int8_t) data;
   	}

   	return hyst;
}

//-- get _T_os() ---------------------------------------------------------------
int 	get_T_os()
{
	uint8_t reg[2];
	uint8_t data;
	int8_t	tos;

	reg[0]=LM75A_REG_T_OS;	//-- 0x03

	//-- write register value -----------------------------------------------
    if((error=i2c_write(&i2c_cfg, reg, 1, 0)) != 1) {
    	printf("[%d] Error master writing %d\n", __LINE__, error);
       	return error;
    }

    //-- now we read as the interrupt bring up callback function ------------
    i2c_req.tx_data=txbuf;
    i2c_req.rx_data=rxbuf;
   	if((error = i2c_aread(&i2c_cfg, &i2c_req)) != E_NO_ERROR) {
    	printf("[%d] Error starting async read %d\n", __LINE__, error);
    	return error;
    }

   	data = i2c_req.tx_data[0];
   	if(data & 0x80) {	//-- check if it is signed
   		tos = (~data+1)*(-1);
   	} else {
   		tos = (int8_t) data;
   	}

   	return tos;
}

//-- set_T_hyst() --------------------------------------------------------------
int 	set_T_hyst(int8_t hyst_setpoint)
{
	uint8_t reg[2];

	reg[0] = LM75A_REG_T_HYST;	//-- 0x02

    if((error=i2c_write(&i2c_cfg, reg, 1, 0)) != 1) {
    	printf("[%d] Error master writing %d\n", __LINE__, error);
       	return error;
    }
    if((error=i2c_write(&i2c_cfg, (uint8_t *)&hyst_setpoint, 1, 1)) != 1) {
    	printf("[%d] Error master writing %d\n", __LINE__, error);
       	return error;
    }
    return 1;
}

//-- set_T_os() ----------------------------------------------------------------
int 	set_T_os(int8_t os_setpoint)
{
	uint8_t reg[2];

	reg[0] = LM75A_REG_T_OS;	//-- 0x03

    if((error=i2c_write(&i2c_cfg, reg, 1, 0)) != 1) {
    	printf("[%d] Error master writing %d\n", __LINE__, error);
       	return error;
    }

    if((error=i2c_write(&i2c_cfg, (uint8_t*)&os_setpoint, 1, 1)) != 1) {
    	printf("[%d] Error master writing %d\n", __LINE__, error);
       	return error;
    }
    return 1;
}

//-- get_config(void) ----------------------------------------------------------
unsigned char get_config(void)
{
	uint8_t reg[2], data[2];

	reg[0]=LM75A_REG_CFG;	//-- 0x01

	//-- write register value -----------------------------------------------
    if((error=i2c_write(&i2c_cfg, reg, 1, 0)) != 1) {
    	printf("[%d] Error master writing %d\n", __LINE__, error);
       	return (unsigned char) error;
    }

    if((error = i2c_read(&i2c_cfg, data, 1, 0)) != 1) {
    	printf("[%d] Error reading %d\n", __LINE__, error);
       	return (unsigned char) error;
    }

   	return (unsigned char)data[0];

}

//-- end of program ------------------------------------------------------------
