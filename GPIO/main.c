/**
 * @file        main.c
 * @brief       GPIO Example
 * @details
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

/* **** Includes **** */
#include <stdio.h>
#include <string.h>
#include "mxc_config.h"
#include "board.h"
#include "gpio.h"
#include "tmr_utils.h"

/* **** Definitions **** */
#define GPIO_PORT_IN                PORT_0
#define GPIO_PIN_IN                 PIN_12

#define GPIO_PORT_OUT               PORT_0
#define GPIO_PIN_OUT                PIN_13 // PIN_13

#define GPIO_PORT_INTERRUPT_IN      PORT_0
#define GPIO_PIN_INTERRUPT_IN       PIN_3

#define GPIO_PORT_INTERRUPT_STATUS  PORT_0
#define GPIO_PIN_INTERRUPT_STATUS   PIN_2

/* **** Globals **** */

/* **** Functions **** */
void gpio_isr(void *cbdata)
{
    GPIO_OutToggle((gpio_cfg_t*)cbdata);
}

int main(void)
{
    gpio_cfg_t gpio_in;
    gpio_cfg_t gpio_out;
    gpio_cfg_t gpio_interrupt;
    gpio_cfg_t gpio_interrupt_status;

    printf("\n\n***** GPIO Example ******\n\n");
    printf("1. This example reads P0.12 (S1) and outputs the same state onto P0.13 (DS1).\n");
    printf("2. An interrupt is set up on P0.3. P0.2 toggles when that interrupt occurs.\n\n");

    /* Setup interrupt status pin as an output so we can toggle it on each interrupt. */
    gpio_interrupt_status.port = GPIO_PORT_INTERRUPT_STATUS;
    gpio_interrupt_status.mask = GPIO_PIN_INTERRUPT_STATUS;
    gpio_interrupt_status.pad = GPIO_PAD_NONE;
    gpio_interrupt_status.func = GPIO_FUNC_OUT;
    GPIO_Config(&gpio_interrupt_status);

    /* Set up interrupt on P0.3. */
    /* Switch on EV kit is open when non-pressed, and grounded when pressed.  Use an internal pull-up so pin
       reads high when button is not pressed. */
    gpio_interrupt.port = GPIO_PORT_INTERRUPT_IN;
    gpio_interrupt.mask = GPIO_PIN_INTERRUPT_IN;
    gpio_interrupt.pad = GPIO_PAD_PULL_UP;
    gpio_interrupt.func = GPIO_FUNC_IN;
    GPIO_Config(&gpio_interrupt);
    GPIO_RegisterCallback(&gpio_interrupt, gpio_isr, &gpio_interrupt_status);
    GPIO_IntConfig(&gpio_interrupt, GPIO_INT_EDGE, GPIO_INT_FALLING);
    GPIO_IntEnable(&gpio_interrupt);
    NVIC_EnableIRQ((IRQn_Type)MXC_GPIO_GET_IRQ(GPIO_PORT_INTERRUPT_IN));

    /* Setup input pin. */
    /* Switch on EV kit is open when non-pressed, and grounded when pressed.  Use an internal pull-up so pin
       reads high when button is not pressed. */
    gpio_in.port = GPIO_PORT_IN;
    gpio_in.mask = GPIO_PIN_IN;
    gpio_in.pad = GPIO_PAD_PULL_UP;
    gpio_in.func = GPIO_FUNC_IN;
    GPIO_Config(&gpio_in);

    /* Setup output pin. */
    gpio_out.port = GPIO_PORT_OUT;
    gpio_out.mask = GPIO_PIN_OUT;
    gpio_out.pad = GPIO_PAD_NONE;
    gpio_out.func = GPIO_FUNC_OUT;
    GPIO_Config(&gpio_out);

    while (1) {
        /* Read state of the input pin. */
        if (GPIO_InGet(&gpio_in)) {
            /* Input pin was high, set the output pin. */
            GPIO_OutSet(&gpio_out);
        } else {
            /* Input pin was low, clear the output pin. */
            GPIO_OutClr(&gpio_out);
        }
    }
}
