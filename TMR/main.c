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
 * @brief   Timer example
 * @details PWM Timer        - Outputs a PWM signal (2Hz, 30% duty cycle) on 3.7
 *          Continuous Timer - Outputs a continuous 1s timer on LED0 (GPIO toggles every 500s)
 *          One Shot Timer   - Starts a one shot timer - LED1 turns on when one shot time (1 sec) is complete
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include "mxc_config.h"
#include "mxc_sys.h"
#include "nvic_table.h"
#include "gpio.h"
#include "board.h"
#include "tmr.h"
#include "tmr_utils.h"
#include "led.h"

/***** Definitions *****/

/* Parameters for PWM output */
#define PORT_PWM   PORT_0
#define PIN_PWM    PIN_3 
#define FREQ       200 /* (Hz)  */
#define DUTY_CYCLE 75  /* (%)   */
#define PWM_TIMER  MXC_TMR0  /* The timer selection must match the available timer for PORT_PWM.PIN_PWM */

/* Parameters for Continuous timer */
#define CONT_LED_IDX       0
#define INTERVAL_TIME_CONT 1  /* (s) will toggle after every interval  */
#define CONT_TIMER MXC_TMR1   /* Any timer other than PWM_TIMER */

/* Parameter for One shot timer */
#define OST_LED_IDX        0
#define INTERVAL_TIME_OST  3  /* (s) before turns LED on, then off */
#define OST_TIMER  MXC_TMR2  /* Any timer other than PWM_TIMER */

/* Check Frequency bounds */
#if (FREQ == 0)
#error "Frequency cannot be 0."
#elif (FREQ > 100000)
#error "Frequency cannot be over 10000."
#endif

/* Check duty cycle bounds */
#if (DUTY_CYCLE < 0) || (DUTY_CYCLE > 100)
#error "Duty Cycle must be between 0 and 100."
#endif

/***** Globals *****/

/***** Functions *****/

void PWM_Output(void)
{
    tmr_pwm_cfg_t tmr_pwm;
    unsigned int period_ticks = PeripheralClock / FREQ;
    unsigned int duty_ticks = period_ticks * DUTY_CYCLE / 100;
    sys_cfg_tmr_t tmr_sys_cfg;

    /*    
     * Steps for configuring a timer for PWM mode:
     * 1. Disable the timer
     * 2. Set the prescale value and enable output
     * 3. Configure the timer for PWM mode
     * 4. Set polarity, pwm parameters
     * 5. Enable Timer
     */

    TMR_Disable(PWM_TIMER); 
    
    tmr_sys_cfg.out_en = 1;
    TMR_Init(PWM_TIMER, TMR_PRES_1, &tmr_sys_cfg);
    
    tmr_pwm.pol = 1;
    tmr_pwm.per_cnt = period_ticks;
    tmr_pwm.duty_cnt = duty_ticks;
    if (TMR_PWMConfig(PWM_TIMER, &tmr_pwm) != E_NO_ERROR) {
        printf("Failed TMR_PWMConfig.\n");
    }
    
    TMR_Enable(PWM_TIMER);

    printf("PWM started.\n");
}

/* Toggles GPIO when continuous timer repeats */
void ContinuousTimer_Handler(void)
{
    /* Clear interrupt */
    TMR_IntClear(CONT_TIMER);
    LED_Toggle(CONT_LED_IDX);
}

void ContinuousTimer(void)
{
    tmr_cfg_t tmr; 
    uint32_t period_ticks = PeripheralClock/4*INTERVAL_TIME_CONT;

    /* Initial state is off */
    LED_Off(CONT_LED_IDX);

    /*    
     * Steps for configuring a timer for PWM mode:
     * 1. Disable the timer
     * 2. Set the prescale value
     * 3  Configure the timer for continuous mode
     * 4. Set polarity, timer parameters
     * 5. Enable Timer
     */

    TMR_Disable(CONT_TIMER);
    
    TMR_Init(CONT_TIMER, TMR_PRES_4, 0);
    
    tmr.mode = TMR_MODE_CONTINUOUS;
    tmr.cmp_cnt = period_ticks;
    tmr.pol = 0;
    TMR_Config(CONT_TIMER, &tmr);

    TMR_Enable(CONT_TIMER);
    
    printf("Continuous timer started.\n");
}

void OneShotTimer(void)
{
    tmr_cfg_t tmr;
    unsigned clk_shift = 0;
    uint64_t max_us;
    uint32_t ticks;
    unsigned long us = SEC(INTERVAL_TIME_OST);

    /* Initial LED state is off. */
    LED_Off(OST_LED_IDX);
    
    /* Find the proper clock shift for timer */
    do {
        max_us = (uint64_t)((0xFFFFFFFFUL / ((uint64_t)PeripheralClock >> clk_shift++)) * 1000000UL);
    } while (us > max_us);
    clk_shift--;
 
    /*    
     * Steps for configuring a timer for PWM mode:
     * 1. Disable the timer
     * 2. Set the prescale value
     * 3  Configure the timer for oneshot mode
     * 4. Set polarity, timer parameters
     * 5. Enable Timer
     */
    
    TMR_Disable(OST_TIMER);

    TMR_Init(OST_TIMER, (tmr_pres_t)clk_shift, 0);
    
    /* Calculate the number of timer ticks we need to wait */
    TMR_GetTicks(OST_TIMER, us, TMR_UNIT_MICROSEC, &ticks); 
    tmr.mode = TMR_MODE_ONESHOT;
    tmr.cmp_cnt = ticks;
    tmr.pol = 0;
    
    TMR_Config(OST_TIMER, &tmr);

    TMR_Enable(OST_TIMER);

    printf("One shot timer started.\n");
    
    /* Wait for timer to end */
    while (TMR_TO_Check(OST_TIMER) != E_TIME_OUT) {}
    
    LED_On(OST_LED_IDX);
    
    /* The delay function can also be used for similar functionality */
    TMR_Delay(OST_TIMER, SEC(INTERVAL_TIME_OST), 0);
    
    LED_Off(OST_LED_IDX);
}

int main(void)
{
    printf("***** Timer Example *****\n\n");
    printf("1. A one shot mode timer is used to toggle LED1 on the Ev Kit\n");
    printf("   every %d sec.  This is repeated twice.\n", INTERVAL_TIME_OST);
    printf("2. After the one shot timer completes, a continuous mode timer is used\n");
    printf("   to create an interrupt every %d sec. LED1 will toggle\n", INTERVAL_TIME_CONT);
    printf("   each time the interrupt occurs.\n");
    printf("3. Timer 2 is used to output a PWM signal on Port %u.%u.  The PWM frequency\n", PORT_PWM, PIN_PWM);
    printf("   is %d Hz and the duty cycle is %d%%.\n\n", FREQ, DUTY_CYCLE);

    /* Set up the PWM to run indefinitely. */
    PWM_Output();

    /* Run one shot to completion. */
    OneShotTimer();
    
    /* Set up continuous timer. */
    NVIC_SetVector(TMR1_IRQn, ContinuousTimer_Handler);
    NVIC_EnableIRQ(TMR1_IRQn);
    ContinuousTimer();

    return 0;
}
