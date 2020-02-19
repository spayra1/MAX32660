/*
 * i2c_bus.c
 *
 *  Created on: Jul 24, 2018
 *      Author: Cyberarea
 */
#include "i2c_bus.h"

int 			errorno;

//-- I2C bus (MASTER) initialization --------------------------------------------------
int i2c_init(i2c_cfg_t *cfg)
{
    const sys_cfg_i2c_t sys_i2c_cfg = NULL; /* No system specific configuration needed. */

    I2C_Shutdown(cfg->i2c_regs);	//-- shutdown first and then initialize the module for i2c
    if((errorno = I2C_Init(cfg->i2c_regs, cfg->i2c_speed, &sys_i2c_cfg)) != E_NO_ERROR) {	//-- 100MHz
        printf("%d) Error initializing I2C bus.  (Error code = %d)\n", __LINE__, errorno);
        return errorno;
    }

    if(cfg->i2c_regs == MXC_I2C0)	//-- I2C controller 0 or 1
    	NVIC_EnableIRQ(I2C0_IRQn);	//-- enable interrupts for I2C0
    else
    	NVIC_EnableIRQ(I2C1_IRQn);	//-- enable interrupts for I2C1

    return E_NO_ERROR;
}

//-- i2c_write() ---------------------------------------------------------------------
int i2c_write(i2c_cfg_t *cfg, uint8_t *txdata, int nbytes, int restart)
{
	if((errorno=I2C_MasterWrite(cfg->i2c_regs, cfg->i2c_addr, txdata, nbytes, restart)) != nbytes) {
		printf("(L%d) Error writing : %d\n", __LINE__, errorno);
	}
	return errorno;
}

//-- synchronous read ----------------------------------------------------------------
int i2c_read(i2c_cfg_t *cfg, uint8_t *rxdata, int nbytes, int restart)
{
    if((errorno = I2C_MasterRead(cfg->i2c_regs, cfg->i2c_addr, rxdata, nbytes, restart)) != nbytes) {
        printf("(L%d)Error reading : %d\n", __LINE__, errorno);
    }
    return errorno;
}

//-- asynchronous read --------------------------------------------------------------
int i2c_aread(i2c_cfg_t *cfg, i2c_req_t *req)
{
	printf("I am in i2c_aread before I2C_MasterAsync()\n");
    if((errorno = I2C_MasterAsync(cfg->i2c_regs, req)) != E_NO_ERROR) {
        printf("(L%d) Error starting async read %d\n", __LINE__, errorno);
    }
    printf("I am in i2c_aread after I2C_MasterAsync()\n");
    return errorno;
}

//-- end of file ----------------------------------------------------------------------
