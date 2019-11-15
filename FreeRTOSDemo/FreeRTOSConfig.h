 /**
 * @file    FreeRTOSConfig.h
 * @brief   FreeRTOSCOnfig function prototypes and data types.
 */
 
/*******************************************************************************
* Copyright (C) 2015 Maxim Integrated Products, Inc., All Rights Reserved.
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
* $Id: FreeRTOSConfig.h 22182 2016-03-30 23:42:14Z zach.metzinger $
*
********************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* **** Includes **** */
#include "max32660.h"

/**
 * @brief   Application specific definitions.
 * @details These definitions should be adjusted for your particular hardware and
 *          application requirements.
 * @note    THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 *          FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * @details See http://www.freertos.org/a00110.html.
 */

/* CMSIS keeps a global updated with current system clock in Hz */
#define configCPU_CLOCK_HZ          SystemCoreClock

#define configUSE_TICKLESS_IDLE     1
#define configTICK_RATE_HZ          ((portTickType)1000)

#define configTOTAL_HEAP_SIZE       ((size_t)(26 * 1024))

#define configMINIMAL_STACK_SIZE    ((unsigned short)128)

#define configMAX_PRIORITIES        5
#define configUSE_PREEMPTION        0
#define configUSE_IDLE_HOOK         0
#define configUSE_TICK_HOOK         0
#define configUSE_CO_ROUTINES       0
#define configUSE_16_BIT_TICKS      0
#define configUSE_MUTEXES           1

/* Run time and task stats gathering related definitions. */
#define configUSE_TRACE_FACILITY                1
#define configUSE_STATS_FORMATTING_FUNCTIONS    1

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet    0
#define INCLUDE_vTaskDelete         0
#define INCLUDE_vTaskSuspend        1
#define INCLUDE_vTaskDelayUntil     1
#define INCLUDE_uxTaskPriorityGet   0
#define INCLUDE_vTaskDelay          1

/* # of priority bits (configured in hardware) is provided by CMSIS */
#define configPRIO_BITS             __NVIC_PRIO_BITS

/* Priority 7, or 255 as only the top three bits are implemented.  This is the lowest priority. */
#define configKERNEL_INTERRUPT_PRIORITY       ( ( unsigned char ) 7 << ( 8 - configPRIO_BITS) )

/* Priority 5, or 160 as only the top three bits are implemented. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY  ( ( unsigned char ) 5 << ( 8 - configPRIO_BITS) )  

/* Alias the default handler names to match CMSIS weak symbols */
#define vPortSVCHandler       SVC_Handler
#define xPortPendSVHandler    PendSV_Handler
#define xPortSysTickHandler   SysTick_Handler

#ifdef configUSE_TICKLESS_IDLE
#define configRTC_TICK_RATE_HZ          ((portTickType)4096)
/* Provide routines for tickless idle pre- and post- processing */
void vPreSleepProcessing( unsigned long * );
void vPostSleepProcessing( unsigned long );
#define configPRE_SLEEP_PROCESSING( idletime ) vPreSleepProcessing( &idletime );
#define configPOST_SLEEP_PROCESSING( idletime ) vPostSleepProcessing( idletime );
#endif

/* FreeRTOS+CLI requires this size to be defined, but we do not use it */
#define configCOMMAND_INT_MAX_OUTPUT_SIZE 1

#endif /* FREERTOS_CONFIG_H */



