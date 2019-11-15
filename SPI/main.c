/**
 * @file    main.c
 * @brief   SPI Master Demo
 * @details Shows Master loopback demo for SPI0 (AKA: SPI17Y) and SPI1 (AKA: SPIMSS)
 *          Read the printf() for instructions
 */

/*******************************************************************************
 * Copyright (C) 2017 Maxim Integrated Products, Inc., All Rights Reserved.
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
 * $Date: 2018-12-19 09:47:46 -0600 (Wed, 19 Dec 2018) $
 * $Revision: 40079 $
 *
 ******************************************************************************/

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pb.h"
#include "mxc_config.h"
#include "led.h"
#include "mxc_pins.h"
#include "spimss.h"
#include "uart.h"
#include "spi.h"

/***** Definitions *****/
#define TEST_LEN        60      // Words
#define OFFSET          256     //offsets data for tx_data 
#define VALUE           0xFFFF
#define SPI_SPEED       5000000  // Bit Rate

// STEP 1: SELECT ONE OF THE SPI BELOW
#define SPI0_A
// #define SPI1_A

// STEP 2: SELECT FROM BELOW
#define ASYNC         // Comment out line for Async operation


#ifdef SPI0_A
    #define SPI SPI0A
    #define SPI_IRQ SPI0_IRQn
#endif

#ifdef SPI1_A
    #define SPI SPI1A
    #define SPI_IRQ SPI1_IRQn
#endif

#ifdef SPI1_B
    #define SPI SPI1B
    #define SPI_IRQ SPI1_IRQn
#endif


/***** Globals *****/
uint16_t rx_data[TEST_LEN];
uint16_t tx_data[TEST_LEN];
volatile int spi_flag;

/***** Functions *****/

void spi_cb(void *req, int error)
{
    spi_flag = error;
}

void SPI0_IRQHandler(void)
{
    SPI_Handler(SPI0A);
}

void SPI1_IRQHandler(void)
{
    SPI_Handler(SPI1A);
}

int main(void)
{

    int i,j,k;
    uint16_t temp;
    spi_req_t req;

#ifdef ASYNC
    // Setup the interrupt
    NVIC_EnableIRQ(SPI_IRQ);
#endif


#if defined(SPI0_A)
    printf("\n************** SPI Loopback Demo ****************\n");
    printf("This example configures the SPI to send data between the MISO (P0.4) and\n");
    printf("MOSI (P0.5) pins.  Connect these two pins together.  This demo shows SPI\n");
    printf("sending different bit sizes each run through. \n");
#endif

#if defined(SPI1_A) | defined(SPI1_B)
    printf("\n************** SPIMSS Master Loopback Demo ****************\n");
    printf("This example configures the SPIMSS to send data between the MISO (P0.10) and\n");
    printf("MOSI (P0.11) pins.  Connect these two pins together.  This demo shows SPI\n");
    printf("sending between 1 and 16 bits of data at a time.  During this demo you\n");
    printf("may see junk data printed to the serial port because the console UART\n");
    printf("shares the same pins as the SPI.\n\n");
#endif

    // delay before UART shutdown
    while (UART_Busy(MXC_UART_GET_UART(CONSOLE_UART)));
    Console_Shutdown();


    for (i=1; i<17; i++) {
        if ((i== 1  || i == 9) && SPI == SPI0A) { // Sending out 2 to 16 bits except 9 bits...
            continue;
        }
        for (j = 0; j < TEST_LEN; j++) {
            tx_data[j] = j + OFFSET;
        }
        // Configure the peripheral
        if (SPI_Init(SPI, 0, SPI_SPEED) != 0) {

            Console_Init();
            printf("Error configuring SPI\n");
            while (1) {}
        }

        memset(rx_data, 0x0, TEST_LEN*2);

        req.tx_data = tx_data;
        req.rx_data = rx_data;
        req.len = TEST_LEN;
        req.bits = i;
        req.width = SPI17Y_WIDTH_1;  // NOT applicable to SPI1A and SPI1B, value ignored
        req.ssel = 0;                // NOT applicable to SPI1A and SPI1B, value ignored
        req.deass = 1;               // NOT applicable to SPI1A and SPI1B, value ignored
        req.tx_num = 0;
        req.rx_num = 0;
        req.callback = spi_cb;
        spi_flag =1;
#ifdef ASYNC
        SPI_MasterTransAsync(SPI, &req);
        while (spi_flag == 1);
#else
        SPI_MasterTrans(SPI, &req);
#endif

        k= OFFSET;
        for (j=0; j<TEST_LEN; j++) {
            if (req.bits <=8) {
                if (j<(TEST_LEN/2)) {
                    temp = (VALUE >>(16-req.bits)<<8)| (VALUE >>(16-req.bits));
                    temp &= k;
                    tx_data[j] = temp;

                } else if (j == (TEST_LEN/2)&& TEST_LEN % 2 == 1) {
                    temp = VALUE >> (16-req.bits);
                    temp &= k;
                    tx_data[j] = temp;
                } else {
                    tx_data[j] = 0x0000;
                }
            } else {
                temp = VALUE >>(16-req.bits);
                temp &= k;
                tx_data[j] = temp;
            }
            k++;
        }

        // Compare Sent data vs Received data
        // Printf needs the Uart turned on since they share the same pins
        if (memcmp(rx_data, tx_data, sizeof(tx_data)) != 0) {
            Console_Init();
            printf("\nError verifying rx_data %d\n",i);
            while (UART_Busy(MXC_UART_GET_UART(CONSOLE_UART)));
#if defined (__GNUC__)
            fflush(stdout);
#endif
            Console_Shutdown();
        } else {
            Console_Init();
            printf("\nSent %d bits per transaction\n",i);
            while (UART_Busy(MXC_UART_GET_UART(CONSOLE_UART)));
#if defined (__GNUC__)
            fflush(stdout);
#endif
            Console_Shutdown();
        }
        SPI_Shutdown(SPI);

    }

    Console_Init();
    printf("\n Done testing \n");


    return 0;
}
