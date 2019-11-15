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

/**
 * @file    main.c
 * @brief   Demonstrates a watchdog timer in run mode
 *
 * @details     This example configures the watchdog timer to generate an interrupt and then reset if
 *              the watchdog counter is not reset.  The code then enters an infinite loop that blinks
 *              an LED (DS1) and resets the watchdog counter.  Press S1 to stop resetting the counter
 *              and allow the watchdog to expire and generate an interrupt.  On the first occurence of
 *              the interrupt, the code will reset the watchdog to prevent the device from resetting.
 *              On the second occurence of the interrupt, the watchdog will not be reset.  Once the
 *              watchdog timer expires, the device will reset.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include "mxc_config.h"
#include "nvic_table.h"
#include "board.h"
#include "mxc_sys.h"
#include "wdt.h"
#include "tmr_utils.h"
#include "led.h"

/***** Definitions *****/

#define SW1 0 

/***** Globals *****/
//use push buttons defined in board.h
extern const gpio_cfg_t pb_pin[];
volatile int interrupt_count = 0;

/***** Functions *****/

// *****************************************************************************
void watchdog_timeout_handler(void)
{
    WDT_ClearIntFlag(MXC_WDT0);
    if (++interrupt_count == 1) {
        /* Reset the watchdog timer once in the interrupt */
        WDT_ResetTimer(MXC_WDT0);
    }
}

// *****************************************************************************
void WDT0_IRQHandler(void)
{
    watchdog_timeout_handler();
}

// *****************************************************************************
int main(void)
{
    const sys_cfg_tmr_t sys_tmr_cfg = {0}; /* Do not enable timer output. */
    
    if (WDT_GetResetFlag(MXC_WDT0)) {
        WDT_ClearResetFlag(MXC_WDT0);
        WDT_EnableReset(MXC_WDT0, 0);
        WDT_Enable(MXC_WDT0, 0);
        printf("Watchdog reset detected.\n");
    }

    printf("\n************** Watchdog Timer Demo ****************\n");
    printf("This example configures the watchdog timer to generate an interrupt and then\n"); 
    printf("reset if the watchdog counter is not reset.  The code then enters an infinite\n");
    printf("loop that blinks an LED (DS1) and resets the watchdog counter.  Press SW1 to\n");
    printf("stop resetting the counter and allow the watchdog to expire and generate an\n");
    printf("interrupt.  On the first occurence ofthe interrupt, the code will reset the\n");
    printf("watchdog to prevent the device from resetting.  On the second occurence of the\n");
    printf("interrupt, the watchdog will not be reset.  Once the watchdog timer expires,\n");
    printf("the device will reset.\n\n");
    LED_Off(0);

    /* Configure the watchdog */
    WDT_SetResetPeriod(MXC_WDT0, WDT_PERIOD_2_27);
    WDT_SetIntPeriod(MXC_WDT0, WDT_PERIOD_2_26);
    WDT_EnableReset(MXC_WDT0, 1);                   /* We want the WD to reset us if it is not fed in time. */
    WDT_EnableInt(MXC_WDT0, 1);                     /* We want the WD to interrupt prior to preforming a reset. */
    NVIC_EnableIRQ(WDT0_IRQn);
    WDT_Enable(MXC_WDT0, 1);                        /* Turn on the WD. */

    while (1) {
        //Push SW1 to stop the loop code from running that resets the WD counter.
        if (GPIO_InGet(&pb_pin[SW1]) == 0) {
            printf("Switch pressed.  Stop resetting the WD counter and wait for interrupt/reset.\n");
            while (interrupt_count == 0) {}
            if (interrupt_count == 1) {
                printf("Watchdog interrupt occurred. Watchdog fed (this time).\n");
            }
            while (interrupt_count < 2) {}
            printf("Watchdog interrupt occurred. Not feeding the watchdog. Prepare for reset.\n");
            while(1);
        }

        //Blink the LED (DS1)
        TMR_Delay(MXC_TMR0, MSEC(100), &sys_tmr_cfg);
        LED_On(0);
        TMR_Delay(MXC_TMR0, MSEC(100), &sys_tmr_cfg);
        LED_Off(0);

        //Reset watchdog
        WDT_ResetTimer(MXC_WDT0);
    }
}
