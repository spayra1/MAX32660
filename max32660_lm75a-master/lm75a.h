/*
 * lm75a.h
 *
 *  Created on: Jul 26, 2018
 *      Author: Cyberarea
 */

#ifndef LM75A_H_
#define LM75A_H_

#include <stdint.h>
#include "i2c_bus.h"

#define LM75A_I2C_ADDRESS	(0x48 << 1)

#define LM75A_DEGREES_RESOLUTION	0.125;

//-- register map for LM75A -----------------------------------------------------
#define LM75A_REG_TEMP		0x00
#define LM75A_REG_CFG		0x01
#define	LM75A_REG_T_HYST	0x02	//-- default Thyst = 75 DegC
#define	LM75A_REG_T_OS		0x03	//-- default Tos   = 80 DegC
#define	LM75A_REG_ID		0x07	//-- not working good

//-- LM75A Configuration structure ----------------------------------------------
typedef struct {
	unsigned :3;	//-- reserved [7:5]
	unsigned FAULT_QUEUE: 2;	//-- FAULT_QUEUE[2]
	unsigned OSPOLARITY: 1;		//-- alert polarity (1:active high, 0:active low
	unsigned MODE:1;			//-- mode (Cmp/Int) (1:interrupt mode, 0: comparator)
	unsigned SHUTDOWN:1;		//-- shudown -> low poer shutdown mode
} 	lm75a_cfgbits_t;

typedef union {
	lm75a_cfgbits_t	cfgbits;
	unsigned char	cfgchar;
} 	lm75a_cfg_t;

//-- LM75A configuration structure ---------------------------------------------
lm75a_cfg_t lm75a_cfg;

//-- function prototypes --------------------------------------------------------
int 		get_id(void);
float 		get_temp(void);
int 		get_T_hyst(void);
int 		set_T_hyst(int8_t hyst_setpoint);
int			get_T_os(void);
int 		set_T_os(int8_t os_setpoint);
unsigned char get_config(void);

#endif /* LM75A_H_ */
