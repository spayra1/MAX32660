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
 * $Date: 2018-10-16 15:02:11 -0500 (Tue, 16 Oct 2018) $
 * $Revision: 38537 $
 *
 ******************************************************************************/

/**
 * @file    	main.c
 * @brief   	Instruction cache example
 * @details     This example shows the difference in execution time when  
 *              the instruction cache is enabled vs. disabled.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mxc_config.h"
#include "icc.h"
#include "rtc.h"
#include "tmr_utils.h"

/***** Definitions *****/

/***** Globals *****/

/***** Functions *****/

//Test function to do simple calculations
unsigned int test_function(void)
{
	unsigned int a, i, j;
	
	for (a = 0; a < 99999; ++a) {
		for (i = 0; i < 100; ++i) {
			j = (i*128/2)/4;
			j = 0;
		}
		if ((a % 10000) == 0)
			printf("%d\n", a);
	}
	printf("\n");
    return j;
}

// *****************************************************************************
int main(void)
{
	printf("\n***** ICC Example *****\n");

	//Instruction cache enabled
	printf("\nWith instruction cache enabled:\n");
	ICC_Enable();
	TMR_SW_Start(MXC_TMR0, NULL);	
	test_function(); //waste time
	printf("Time elapsed: %0.3f ms \n", (double)(TMR_SW_Stop(MXC_TMR0)/1000.0));

	//Instruction cache disabled
	printf("\nWith instruction cache disabled:\n");
	ICC_Disable();
	TMR_SW_Start(MXC_TMR0, NULL);
	test_function(); //waste time
	printf("Time elapsed: %0.3f ms\n", (double)(TMR_SW_Stop(MXC_TMR0)/1000.0));

	printf("\nExample complete.\n");
}
