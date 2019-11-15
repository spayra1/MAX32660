/**
 * @file        main.c
 * @brief       Configures and starts the RTC and demonstrates the use of the alarms.
 * @details     The RTC is enabled and the sub-second alarm set to trigger every 250 ms.
 *              The LED (DS1) is toggled each time the sub-second alarm triggers.  The
 *              time-of-day alarm is set to 5 seconds.  When the time-of-day alarm
 *              triggers, the rate of the sub-second alarm is switched to 500 ms.  The
 *              time-of-day alarm is then rearmed for another 5 sec.  Pressing S1 will
 *              output the current value of the RTC to the console UART.
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
 * $Date: 2018-12-13 16:19:08 -0600 (Thu, 13 Dec 2018) $
 * $Revision: 39881 $
 *
 ******************************************************************************/


/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include "mxc_config.h"
#include "nvic_table.h"
#include "board.h"
#include "rtc.h"
#include "led.h"
#include "pb.h"
#include "tmr_utils.h"

/***** Definitions *****/
#define LED_ALARM           0
#define TIME_OF_DAY_SEC     2
#define SUBSECOND_MSEC_0    125
#define SUBSECOND_MSEC_1    250
#define SECS_PER_MIN        60
#define SECS_PER_HR         (60 * SECS_PER_MIN)
#define SECS_PER_DAY        (24 * SECS_PER_HR)

#define MSEC_TO_RSSA(x) (0 - ((x * 256) / 1000)) /* Converts a time in milleseconds to the equivalent RSSA register value. */

/***** Globals *****/
uint32_t ss_interval = SUBSECOND_MSEC_0;
volatile int buttonPressed = 0;

/***** Functions *****/
void RTC_IRQHandler(void)
{
    int time;
    int flags = RTC_GetFlags();

    /* Check sub-second alarm flag. */
    if (flags & MXC_F_RTC_CTRL_ALSF) {
        LED_Toggle(LED_ALARM);
        RTC_ClearFlags(MXC_F_RTC_CTRL_ALSF);
    }

    /* Check time-of-day alarm flag. */
    if (flags & MXC_F_RTC_CTRL_ALDF) {
        RTC_ClearFlags(MXC_F_RTC_CTRL_ALDF);

        /* Set a new alarm 10 seconds from current time. */
        time = RTC_GetSecond();
        if (RTC_SetTimeofdayAlarm(MXC_RTC, time + TIME_OF_DAY_SEC) != E_NO_ERROR) {
            /* Handle Error */
        }

        // Toggle the sub-second alarm interval.
        if (ss_interval == SUBSECOND_MSEC_0) {
            ss_interval = SUBSECOND_MSEC_1;
        } else {
            ss_interval = SUBSECOND_MSEC_0;
        }

        if (RTC_SetSubsecondAlarm(MXC_RTC, MSEC_TO_RSSA(ss_interval)) != E_NO_ERROR) {
            /* Handle Error */
        }
    }

    return;
}

void buttonHandler(void *pb)
{
    buttonPressed = 1;
}

void printTime(void)
{
    int day, hr, min, sec;
    double subsec;

    subsec = RTC_GetSubSecond() / 256.0;
    sec = RTC_GetSecond();

    day = sec / SECS_PER_DAY;
    sec -= day * SECS_PER_DAY;

    hr = sec / SECS_PER_HR;
    sec -= hr * SECS_PER_HR;

    min = sec / SECS_PER_MIN;
    sec -= min * SECS_PER_MIN;

    subsec += sec;

    printf("Current Time (dd:hh:mm:ss): %02d:%02d:%02d:%05.2f\n", day, hr, min, subsec);
}
int main(void)
{
    const sys_cfg_tmr_t sys_tmr_cfg = {0};  // Do not enable timer output.
    sys_cfg_rtc_t sys_cfg;

    printf("\n***** RTC Example *****\n\n");
    printf("The RTC is enabled and the sub-second alarm set to trigger every %d ms.\n", SUBSECOND_MSEC_0);
    printf("The LED (DS1) is toggled each time the sub-second alarm triggers.  The\n");
    printf("time-of-day alarm is set to %d seconds.  When the time-of-day alarm\n", TIME_OF_DAY_SEC);
    printf("triggers, the rate of the sub-second alarm is switched to %d ms.  The\n", SUBSECOND_MSEC_1);
    printf("time-of-day alarm is then rearmed for another %d sec.  Pressing S1 will\n", TIME_OF_DAY_SEC);
    printf("output the current value of the RTC to the console UART.\n\n");

    NVIC_EnableIRQ(RTC_IRQn);

    // Setup callback to receive notification of when button is pressed.
    PB_RegisterCallback(0, buttonHandler);

    // Turn LED off initially
    LED_Off(LED_ALARM);

    sys_cfg.tmr = MXC_TMR0;
    if (RTC_Init(MXC_RTC, 0, 0, &sys_cfg) != E_NO_ERROR) {
        printf("Failed RTC_Setup().\n");
        return -1;
    }

    printf("RTC started.\n");
    printTime();

    if (RTC_SetTimeofdayAlarm(MXC_RTC, TIME_OF_DAY_SEC) != E_NO_ERROR) {
        printf("Failed RTC_SetTimeofdayAlarm().\n");
        return -1;
    }

    if (RTC_EnableTimeofdayInterrupt(MXC_RTC) != E_NO_ERROR) {
        printf("Failed RTC_EnableTimeofdayInterrupt().\n");
        return -1;
    }

    if (RTC_SetSubsecondAlarm(MXC_RTC,  (uint32_t)MSEC_TO_RSSA(SUBSECOND_MSEC_0)) != E_NO_ERROR) {
        printf("Failed RTC_SetSubsecondAlarm().\n");
        return -1;
    }

    if (RTC_EnableSubsecondInterrupt(MXC_RTC) != E_NO_ERROR) {
        printf("Failed RTC_EnableSubsecondInterrupt().\n");
        return -1;
    }

    if (RTC_EnableRTCE(MXC_RTC) != E_NO_ERROR) {
        printf("Failed RTC_EnableRTCE().\n");
        return -1;
    }

    while (1) {
        if (buttonPressed) {
            // Show the time elapsed.
            printTime();
            // Delay for switch debouncing.
            TMR_Delay(MXC_TMR0, MSEC(100), &sys_tmr_cfg);
            // Re-arm switch detection.
            buttonPressed = 0;
        }
    }
}
