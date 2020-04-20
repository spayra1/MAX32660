/*
 * i2c_check_flags.c
 *
 *  Created on: Jul 23, 2018
 *      Author: Cyberarea
 */
#include <stdio.h>
#include "i2c_check_flags.h"

//-- i2c0_status_check() ---------------------------------------------------------------------
void i2c0_status_check(void)
{
	unsigned int flag;
	mxc_i2c_regs_t *i2c0reg=MXC_I2C0;

	flag = i2c0reg->ctrl & I2C0_CTRL_MASK;
	printf("i2c0_status_check() at %d : i2c0reg->ctrl; 0x%08x\n", flag, __LINE__);
	if(flag & I2C0_CTRL_HSMODE)		printf("\tI2C0_CTRL_HSMODE\n");
	if(flag & I2C0_CTRL_SCL_PPM)	printf("\tI2C0_CTRL_SCL_PPM\n");
	if(flag & I2C0_CTRL_SCL_STRD)	printf("\tI2C0_CTRL_SCL_STRD\n");
	if(flag & I2C0_CTRL_READ)		printf("\tI2C0_CTRL_READ\n");
	if(flag & I2C0_CTRL_SWOE)		printf("\tI2C0_CTRL_SWOE\n");
	if(flag & I2C0_CTRL_SDA)		printf("\tI2C0_CTRL_SDA\n");
	if(flag & I2C0_CTRL_SCL)		printf("\tI2C0_CTRL_SCL\n");
	if(flag & I2C0_CTRL_SDAO)		printf("\tI2C0_CTRL_SDAO\n");
	if(flag & I2C0_CTRL_SCLO)		printf("\tI2C0_CTRL_SCLO\n");
	if(flag & I2C0_CTRL_ACK)		printf("\tI2C0_CTRL_ACK\n");
	if(flag & I2C0_CTRL_IRXM)		printf("\tI2C0_CTRL_IRXM\n");
	if(flag & I2C0_CTRL_GCEN)		printf("\tI2C0_CTRL_GCEN\n");
	if(flag & I2C0_CTRL_MST)		printf("\tI2C0_CTRL_MST\n");
	if(flag & I2C0_CTRL_I2CEN)		printf("\tI2C0_CTRL_I2CEN\n");

	flag = i2c0reg->status & I2C0_STATUS_MASK;
	printf("i2c0_status_check() at %d : i2c0reg->status; 0x%08x\n", flag, __LINE__);
	if(flag & I2C0_STATUS_CKMD) 	printf("\tI2C0_STATUS_CKMD\n");
	if(flag & I2C0_STATUS_TXF) 		printf("\tI2C0_STATUS_TXF\n");
	if(flag & I2C0_STATUS_TXE) 		printf("\tI2C0_STATUS_TXE\n");
	if(flag & I2C0_STATUS_RXF) 		printf("\tI2C0_STATUS_RXF\n");
	if(flag & I2C0_STATUS_RXE) 		printf("\tI2C0_STATUS_RXE\n");
	if(flag & I2C0_STATUS_BUSY) 	printf("\tI2C0_STATUS_BUSY\n");

	flag = i2c0reg->int_fl0 & I2C0_INTFL0_MASK;	//-- clear unused portion[31:16]
	printf("i2c0_status_check() at %d  : i2c0reg->int_fl0; 0x%08x\n", flag, __LINE__);
	if(flag & I2C0_INTFL0_TXLOI) 	printf("\tI2C0_INTFL0_TXLOI\n");
	if(flag & I2C0_INTFL0_STOPERI) 	printf("\tI2C0_INTFL0_STOPERI\n");
	if(flag & I2C0_INTFL0_STRTERI) 	printf("\tI2C0_INTFL0_STRTERI\n");
	if(flag & I2C0_INTFL0_DNRERI) 	printf("\tI2C0_INTFL0_DNRERI\n");
	if(flag & I2C0_INTFL0_DATERI) 	printf("\tI2C0_INTFL0_DATERI\n");
	if(flag & I2C0_INTFL0_ADRERI) 	printf("\tI2C0_INTFL0_ADRERI\n");
	if(flag & I2C0_INTFL0_TOERI) 	printf("\tI2C0_INTFL0_TOERI\n");
	if(flag & I2C0_INTFL0_ARBERI) 	printf("\tI2C0_INTFL0_ARBERI\n");
	if(flag & I2C0_INTFL0_ADRACKI) 	printf("\tI2C0_INTFL0_ADRACKI\n");
	if(flag & I2C0_INTFL0_STOPI) 	printf("\tI2C0_INTFL0_STOPI\n");
	if(flag & I2C0_INTFL0_TXTHI) 	printf("\tI2C0_INTFL0_TXTHI\n");
	if(flag & I2C0_INTFL0_RXTHI) 	printf("\tI2C0_INTFL0_RXTHI\n");
	if(flag & I2C0_INTFL0_AMI) 		printf("\tI2C0_INTFL0_AMI\n");
	if(flag & I2C0_INTFL0_GCI) 		printf("\tI2C0_INTFL0_GCI\n");
	if(flag & I2C0_INTFL0_IRXMI) 	printf("\tI2C0_INTFL0_IRXMI\n");
	if(flag & I2C0_INTFL0_DONEI) 	printf("\tI2C0_INTFL0_DONEI\n");
}
//-- i2c0_interrupt_check() ------------------------------------------------------------------
void i2c0_interrupt_check(void)
{
	unsigned int flag;
	mxc_i2c_regs_t *i2c0reg=MXC_I2C0;

	flag = i2c0reg->int_en0 & I2C0_INTEN0_MASK;	//-- clear unused portion[31:16]
	printf("i2c0_interrupt_check() at %d :i2c0reg->int_en0; 0x%08x\n", flag, __LINE__);
	if(flag & I2C0_INTEN0_TXLOIE) 	printf("\tI2C0_INTEN0_TXLOIE\n");
	if(flag & I2C0_INTEN0_STOPERIE) printf("\tI2C0_INTEN0_STOPERIE\n");
	if(flag & I2C0_INTEN0_STRTERIE) printf("\tI2C0_INTEN0_STRTERIE\n");
	if(flag & I2C0_INTEN0_DNRERIE) 	printf("\tI2C0_INTEN0_DNRERIE\n");
	if(flag & I2C0_INTEN0_DATERIE) 	printf("\tI2C0_INTEN0_DATERIE\n");
	if(flag & I2C0_INTEN0_ADRERIE) 	printf("\tI2C0_INTEN0_ADRERIE\n");
	if(flag & I2C0_INTEN0_TOERIE) 	printf("\tI2C0_INTEN0_TOERIE\n");
	if(flag & I2C0_INTEN0_ARBERIE) 	printf("\tI2C0_INTEN0_ARBERIE\n");
	if(flag & I2C0_INTEN0_ADRACKIE) printf("\tI2C0_INTEN0_ADRACKIE\n");
	if(flag & I2C0_INTEN0_STOPIE) 	printf("\tI2C0_INTEN0_STOPIE\n");
	if(flag & I2C0_INTEN0_TXTHIE) 	printf("\tI2C0_INTEN0_TXTHIE\n");
	if(flag & I2C0_INTEN0_RXTHIE) 	printf("\tI2C0_INTEN0_RXTHIE\n");
	if(flag & I2C0_INTEN0_AMIE) 	printf("\tI2C0_INTEN0_AMIE\n");
	if(flag & I2C0_INTEN0_GCIE) 	printf("\tI2C0_INTEN0_GCIE\n");
	if(flag & I2C0_INTEN0_IRXMIE) 	printf("\tI2C0_INTEN0_IRXMIE\n");
	if(flag & I2C0_INTEN0_DONEIE) 	printf("\tI2C0_INTEN0_DONEIE\n");
}
//-- end of program --------------------------------------------------------------------------
