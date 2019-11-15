/**
 * @file    main.c
 * @brief   Flash Control Mass Erase & Write 32-bit enabled mode Example
 * @details This example shows how to mass erase the flash using the library
 *          and also how to Write and Verify 4 Words to the flash.
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


/***** Includes *****/
#include "mxc_config.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "flc.h"
#include "flc_regs.h"
#include "gcr_regs.h"

/***** Definitions *****/
#define TESTSIZE        64
#define MXC_FLASH_MEM_SIZE_TEST MXC_FLASH_MEM_SIZE


/***** Globals *****/
uint32_t testaddr = 3;
uint32_t testdata[TESTSIZE];

/***** Functions *****/

// *****************************************************************************

int flash_verify(uint32_t address, uint32_t length, uint8_t *data)
{
  volatile uint8_t *ptr;

  for (ptr = (uint8_t *)address; ptr < (uint8_t *)(address + length); ptr++, data++) {
    if (*ptr != *data) {
        printf("Verify failed at 0x%x (0x%x != 0x%x)\n", (unsigned int)ptr, (unsigned int)*ptr, (unsigned int)*data);
        return E_UNKNOWN;
    }
  }

  return E_NO_ERROR;
}
//******************************************************************************

int check_mem(uint32_t startaddr, uint32_t length, uint32_t data)
{
    uint32_t *ptr;
    for (ptr = (uint32_t *)startaddr; ptr < (uint32_t *)(startaddr + length);
        ptr++) {

        if (*ptr != data)
        {
            return 0;
        }
    }

    return 1;
}

//******************************************************************************

int check_erased(uint32_t startaddr, uint32_t length)
{
    return check_mem(startaddr, length, 0xFFFFFFFF);
}

//******************************************************************************

int check_not_erased(uint32_t startaddr, uint32_t length)
{
    uint32_t * ptr;
    int erasedvaluefound = 0;

    for (ptr = (uint32_t*)startaddr; ptr < (uint32_t*)(startaddr + length); ptr++)
    {
        if (*ptr == 0xFFFFFFFF)
        {
            if (!erasedvaluefound)
            {
                erasedvaluefound = 1;
            }
            else
            {
                return 0;
            }
        }
    }

    return 1;
}

//******************************************************************************

int main(void)
{
    int error_status, i;

    /* Note: This example must execute out of RAM, due to FLC_MassErase() call, below */
    printf("\n\n***** Flash Control Example *****\n");

    /* Clear flash operation flags */
    FLC_ClearFlags(MXC_F_FLC_INTR_DONE | MXC_F_FLC_INTR_AF);

    error_status = FLC_MassErase();

    if (error_status == E_NO_ERROR) {
        printf("Flash erased.\n");
    } else if (error_status == E_BAD_STATE) {
        printf("Flash erase operation is not allowed in this state.\n");
    } else {
        printf("Fail to erase flash's content.\n");
    }

    // Check flash content
    if (check_erased(MXC_FLASH_MEM_BASE, MXC_FLASH_MEM_SIZE_TEST)){
        printf("Flash mass erase is verified.\n");
    } else {
        printf("Flash mass erase failed.\n");
    }

    printf("Size of testdata : %d\n", sizeof(testdata));
    // Initializing Test Data
    for (i = 0; i < TESTSIZE; i++) {
        testdata[i] = i;
    }

    printf("Writing %d bytes to flash (using FLC_Write) at address 0x%08X\n", sizeof(testdata), testaddr);
    if (FLC_Write(testaddr, sizeof(testdata), (uint8_t*)&testdata[i]) != E_NO_ERROR) {
        printf("Failure in writing flash.\n");
        return 1;
    }

    // Verify that data is written properly
    if (flash_verify(testaddr, sizeof(testdata), (uint8_t*)&testdata[i]) != E_NO_ERROR) {
        printf("Flash is not written properly.\n");
        return 1;
    }

    printf("Flash written properly and has been verified.\n");

    printf("End of example.\n");
    return 0;
}
