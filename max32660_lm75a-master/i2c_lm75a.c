/**
 * @file    	i2c_lm75a.c
 * @brief   	I2C, Example, LM75A() Temperature sensor
 * @details 	Wiring, I2C Address (0x48)
 * 		        MAX32600		LM75A
 * 		        P0.8 (SCL)  -	SCL
 * 		        P0.9 (SDA)  -	SDA
 * 		        VDDIO		- 	Vcc
 * 		        GND			- 	GND
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "i2c.h"
#include "mxc_config.h"
#include "mxc_sys.h"
#include "nvic_table.h"
#include "tmr.h"
#include "tmr_utils.h"
#include "gpio.h"
#include "led.h"

#include "lm75a.h"
#include "i2c_bus.h"

/*
#define I2C_MASTER	    MXC_I2C0	//-- I2C register for master, max32660.h
#define I2C_MASTER_IDX	0			//-- I2C register 0
#define I2C_SLAVE_ADDR	(0x48 << 1)		//-- LM75A Temperature Sensor
*/

i2c_req_t 	i2c_req;	//-- request
i2c_cfg_t	i2c_cfg;	//-- i2c configuration

#define BUFLENGTH	32
uint8_t rxbuf[BUFLENGTH];
uint8_t txbuf[BUFLENGTH];

volatile int i2c_flag;
volatile int timer_flag;

//-- for continuous timer
#define CONT_LED_IDX		0
#define INTERVAL_TIME_CONT	3			//-- 1 seconds
#define CONT_TIMER			MXC_TMR0	//-- timer MXC_TMR0


void	measurement_loop(void);	//-- measurement loop
//-- continuous timer handler ---------------------------------------------------------

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

	printf("Contious timer started and will be interrupted every one second\n");
}

//-- Master interrupt handler --------------------------------------------------------
void I2C0_IRQHandler(void)
{
    I2C_Handler(I2C_MASTER);	//-- this function to be called in the interrupt handler
    // return;
}

//-- I2C callback function ------------------------------------------------------------
//-- this is the callback function for asynchronous read but not working very well ----
void i2c_callback(i2c_req_t *req, int error)
{
	i2c_flag=error;
}

//-- main(): init I2C communication and report temperature every designated seconds.
int main(void)
{
	int error;

	printf("\n\nLM75A Initialization Process started....\n\n");

	//-- setup continuous timer --------------------------------------------
	NVIC_SetVector(TMR0_IRQn, ContinousTimer_Handler);
	NVIC_EnableIRQ(TMR0_IRQn);
	ContinuousTimer();	//-- Install timer function

	//-- i2c bus establish info -------------------------------------------
	i2c_cfg.i2c_role 	= I2C_MASTER;
	i2c_cfg.i2c_regs	= MXC_I2C0;
	i2c_cfg.i2c_speed	= I2C_STD_MODE;		//-- 100MHz
	i2c_cfg.i2c_addr	= LM75A_I2C_ADDRESS;	//-- i2c address

	//-- i2c request for asynchronous read --------------------------------
    i2c_req.addr 		= LM75A_I2C_ADDRESS;
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

	//-- init lm75a ------------------------------------------------------
	measurement_loop();	//-- measurement loop

	return 0;
}

//-- loop() ---------------------------------------------------------------
void	measurement_loop(void)	//-- measurement loop
{
	float temp=0.0;

	//-- do nothing all measurement shall be done by timer interrupt
	while(1) {
		while(timer_flag != 1);	//-- wait until timer interrupt
		if(timer_flag==1) {
			timer_flag = 0;
			temp = get_temp();
			printf("Measured temperature %f\n", temp);
		}
	}
}
//-- end of program ---------------------------------------------------------
