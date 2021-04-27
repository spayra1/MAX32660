/**
 * @file    main.c
 * @brief   Hello World!
 * @details This example uses the UART to print to a terminal and flashes an LED.
 */

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
 * $Date: 2018-08-09 18:45:02 -0500 (Thu, 09 Aug 2018) $
 * $Revision: 36818 $
 *
 ******************************************************************************/

// 4-19-21 commented out LED to test on WLP

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include "mxc_config.h"
#include "mxc_delay.h"
//#include "led.h"
#include "gpio.h" //added 4-26-21
#include "tmr.h"
#include "tmr_utils.h"

/***** Definitions *****/

/***** Globals *****/

/***** Functions *****/

int main(void)
{
	// pin 6 to alternate function 3 (UART 1 TX)
	gpio_cfg_t gpio_uart_tx;
	gpio_uart_tx.port = PORT_0;
	gpio_uart_tx.mask = PIN_6;
	gpio_uart_tx.pad = GPIO_PAD_NONE;
	gpio_uart_tx.func = GPIO_FUNC_ALT3;
	GPIO_Config(&gpio_uart_tx);

	// pin 7 to alternate function 3 (UART 1 RX)
	gpio_cfg_t gpio_uart_rx;
	gpio_uart_rx.port = PORT_0;
	gpio_uart_rx.mask = PIN_7;
	gpio_uart_rx.pad = GPIO_PAD_NONE;
	gpio_uart_rx.func = GPIO_FUNC_ALT3;
	GPIO_Config(&gpio_uart_rx);

    printf("Hello World!\n");

    int count = 0;
    while(1) {
//        LED_On(0);

        /* Demonstrates the TMR driver delay */
        TMR_Delay(MXC_TMR0, MSEC(500), NULL);
//        LED_Off(0);

        /* Demonstrates the SysTick-based convenience delay functions */
        mxc_delay(MXC_DELAY_MSEC(500));
        printf("Hello Gabriel #%d\n", count++);
    }
}
