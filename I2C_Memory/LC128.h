/*
 * 24LC128.h
 *
 *  Created on: April 1, 2020
 *      Author: Syamantak Payra
 */

#ifndef LCXXX_H_
#define LCXXX_H_

#include <stdint.h>
#include "i2c_bus.h"

#define LC128_I2C_ADDRESS	(0x50 << 1)

//-- register map for LM75A -----------------------------------------------------
#define LC128_REG_TEMP		0x00
#define LC128_REG_CFG		0x01
#define	LC128_REG_T_HYST	0x02	//-- default Thyst = 75 DegC
#define	LC128_REG_T_OS		0x03	//-- default Tos   = 80 DegC
#define	LC128_REG_ID		0x07	//-- not working good

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

#endif /* 24LCXXX_H_ */
