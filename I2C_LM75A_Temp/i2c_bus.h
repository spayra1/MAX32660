/*
 * i2c_bus.h
 *
 *  Created on: Jul 24, 2018
 *      Author: Cyberarea
 */
#ifndef I2C_BUS_H
#define I2C_BUS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "i2c.h"
#include "i2c_regs.h"

#define I2C_MASTER	0
#define I2C_SLAVE	1

typedef struct {
	uint8_t			i2c_role;	//-- MASTER or SLAVE
	mxc_i2c_regs_t	*i2c_regs;	//-- MXC_I2C0 or MXC_I2C1
	i2c_speed_t		i2c_speed;	//-- I2C_STD_MODE/FAST_MODE/FASTPLUS_MODE/HS_MODE
	uint8_t			i2c_addr;	//-- slave address
} 	i2c_cfg_t;

int i2c_init(i2c_cfg_t *);
int i2c_write(i2c_cfg_t *, uint8_t *, int, int);
int i2c_read(i2c_cfg_t *, uint8_t *, int, int);
int i2c_aread(i2c_cfg_t *, i2c_req_t *);

#endif /* I2C_BUS_H */
