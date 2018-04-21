/*
 * FreeRTOS Kernel V10.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* Standard includes. */
#include <stdlib.h>
#include <signal.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Demo application includes. */

/* Demo task priorities. */
#define mainLED_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )

#define mainQUEUE_LENGTH	( 1 )

/*
 * Perform the hardware setup required by the ES449 in order to run the demo
 * application.
 */
static void prvSetupHardware( void );

/* Used to detect the idle hook function stalling. */
//static volatile unsigned long ulIdleLoops = 0UL;

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

void flash_led_fast(void);
void flash_led_fast(void)
{
int j;
	for(j=0; j<20;j++) {
		volatile unsigned int i;	// volatile to prevent optimization

		P1OUT ^= 0x40;				// Toggle P1.0 using exclusive-OR

		i = 7000;					// SW Delay
		do i--;
		while(i != 0);
	}
}

static void prvTxTask( void *pvParameters )
{
TickType_t xLastWakeTime;
BaseType_t led_status;

	xLastWakeTime = xTaskGetTickCount();

	led_status = 0x40;
	for (;;) {
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
		led_status ^= 0x41;
		xQueueSend( xQueue, &led_status, 0U );
	}
}

static void prvRxTask( void *pvParameters )
{
BaseType_t received;

	for (;;) {
		/* Wait until something arrives in the queue - this task will block
		indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
		FreeRTOSConfig.h. */
		xQueueReceive( xQueue, &received, portMAX_DELAY );
		P1OUT = (unsigned char)received;
	}
}

/*-----------------------------------------------------------*/
/* Structure that will hold the TCB of the task being created. */
StaticTask_t xTaskTxBuffer;
StaticTask_t xTaskRxBuffer;

/* Buffer that the task being created will use as its stack.  Note this is
   an array of StackType_t variables.  The size of StackType_t is dependent on
   the RTOS port. */
StackType_t xTxStack[ configMINIMAL_STACK_SIZE ];
#define RX_TASK
#ifdef RX_TASK
StackType_t xRxStack[ configMINIMAL_STACK_SIZE ];
#endif
/* The variable used to hold the queue's data structure. */
static StaticQueue_t xStaticQueue;
unsigned char ucQueueStorageArea[mainQUEUE_LENGTH * sizeof(BaseType_t)];
/*
 * Start the demo application tasks - then start the real time scheduler.
 */
int main( void )
{
	/* Setup the hardware ready for the demo. */
	prvSetupHardware();
#if 1
	/* Create the standard demo application tasks. */
	xTaskCreateStatic(prvTxTask,
		    "",
		    configMINIMAL_STACK_SIZE,
		    NULL,
		    mainLED_TASK_PRIORITY,
		    xTxStack,
		    &xTaskTxBuffer);
#endif
#ifdef RX_TASK
	xTaskCreateStatic(prvRxTask,
		    "",
		    configMINIMAL_STACK_SIZE,
		    NULL,
		    mainLED_TASK_PRIORITY,
		    xRxStack,
		    &xTaskRxBuffer);
#endif
	/* Create a static queue. */
	xQueue = xQueueCreateStatic( mainQUEUE_LENGTH,
                                 sizeof(BaseType_t),
                                 ucQueueStorageArea,
                                 &xStaticQueue );
	if (!xQueue)
		return -1;

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* As the scheduler has been started the demo applications tasks will be
	executing and we should never get here! */
	flash_led_fast();
	return 0;
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Stop the watchdog. */
	WDTCTL = WDTPW + WDTHOLD;
	BCSCTL3 |= LFXT1S_2;

	/* Set LED pin P1.0 to output direction */
	P1DIR |= 0x41;
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void );
void vApplicationIdleHook( void )
{
	/* Simple put the CPU into lowpower mode. */
	_BIS_SR( LPM3_bits );
}
/*-----------------------------------------------------------*/
/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
#define IDLE_TASK_STACK		32
static StackType_t uxIdleTaskStack[ IDLE_TASK_STACK ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = IDLE_TASK_STACK;
}
/*-----------------------------------------------------------*/
