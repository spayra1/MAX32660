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
 * @file    	main.c
 * @brief   	I2S Example
 * @note        I2S is configured to send Audio Data on P0.11
 */

/***** Includes *****/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "mxc_sys.h"
#include "tmr_utils.h"
#include "i2s.h"
#include "board.h"
#include "uart.h"

uint16_t tone[64] = {
    0x8000, 0x8c8b, 0x98f8, 0xa527, 0xb0fb, 0xbc56, 0xc71c, 0xd133,
    0xda82, 0xe2f1, 0xea6d, 0xf0e2, 0xf641, 0xfa7c, 0xfd89, 0xff61,
    0xffff, 0xff61, 0xfd89, 0xfa7c, 0xf641, 0xf0e2, 0xea6d, 0xe2f1,
    0xda82, 0xd133, 0xc71c, 0xbc56, 0xb0fb, 0xa527, 0x98f8, 0x8c8b,
    0x8000, 0x7374, 0x6707, 0x5ad8, 0x4f04, 0x43a9, 0x38e3, 0x2ecc,
    0x257d, 0x1d0e, 0x1592, 0x0f1d, 0x09be, 0x0583, 0x0276, 0x009e,
    0x0000, 0x009e, 0x0276, 0x0583, 0x09be, 0x0f1d, 0x1592, 0x1d0e,
    0x257d, 0x2ecc, 0x38e3, 0x43a9, 0x4f04, 0x5ad8, 0x6707, 0x7374
};


/***** Definitions *****/



/***** Global Data *****/
i2s_cfg_t cfg;


void DMA0_IRQHandler(void) {
    DMA_Handler(0);
}

/* Reset DMA Reload so we loop the audio sample forever */
void dma_ctz_cb(int ch, int err){
    I2S_DMA_SetReload(cfg.dma_src_addr, cfg.dma_dst_addr, cfg.dma_cnt);
    I2S_DMA_ClearFlags();
}

/*****************************************************************/
int main()
{
    int err;
    const sys_cfg_i2s_t sys_i2s_cfg = {MAP_A, DMA_REQSEL_SPI1TX, DMA_REQSEL_SPI1RX}; /* Use SPI1A and its associated dma request selects. */
    const sys_cfg_tmr_t sys_tmr_cfg = {0}; /* Do not enable timer output. */

    NVIC_EnableIRQ(DMA0_IRQn);

    printf("\n\n\n\nI2S Example\n\n\n\n");

    cfg.left_justify = 0;
    cfg.mono_audio = 1;
    cfg.audio_direction = AUDIO_OUT;
    cfg.sample_rate = 16000;
    cfg.start_immediately = 0;
    cfg.dma_src_addr = tone;
    cfg.dma_dst_addr = NULL;
    cfg.dma_cnt = 128;
    cfg.dma_reload_en = 1;

    while(UART_Busy(MXC_UART_GET_UART(CONSOLE_UART)));
#if defined ( __GNUC__ )
    fflush(stdout);
#endif
    Console_Shutdown();
    if((err = I2S_Init(&cfg, dma_ctz_cb, &sys_i2s_cfg)) != E_NO_ERROR) {
        printf("Error in I2S_Init: %d\n", err);
        while(1){}
    }

    // I2S Configured

    TMR_Delay(MXC_TMR0, MSEC(1000), &sys_tmr_cfg);

    // Starting I2S Output
    I2S_Start();
    TMR_Delay(MXC_TMR0, MSEC(1000), &sys_tmr_cfg);

    // Muting I2S Output
    I2S_Mute();
    TMR_Delay(MXC_TMR0, MSEC(1000), &sys_tmr_cfg);

    // Unmuting I2S Output
    I2S_Unmute();
    TMR_Delay(MXC_TMR0, MSEC(500), &sys_tmr_cfg);

    // Pausing I2S Output
    I2S_Pause();
    TMR_Delay(MXC_TMR0, MSEC(500), &sys_tmr_cfg);

    // Resuming I2S Output
    I2S_Unpause();
    TMR_Delay(MXC_TMR0, MSEC(500), &sys_tmr_cfg);

    // Stopping I2S Output.
    I2S_Stop();

    if((err = I2S_Shutdown()) != E_NO_ERROR) {
        Console_Init();
        printf("Could not shut down I2S driver: %d\n", err);
        while(1){}
    }

    Console_Init();
    printf("\n\n\n\n\n\n\n\n\n  I2S Complete.  Ignore any random characters previously displayed.");
    printf(" The I2S and UART are sharing the same pins.\n");

    return 0;
}
