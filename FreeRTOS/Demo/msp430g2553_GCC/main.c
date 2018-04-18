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
//#include "queue.h"

/* Demo application includes. */

/* Constants required for hardware setup. */
#define mainMAX_FREQUENCY		( ( unsigned char ) 121 )

/* Demo task priorities. */
#define mainLED_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )

#define mainQUEUE_LENGTH	( 1 )

/*
 * Perform the hardware setup required by the ES449 in order to run the demo
 * application.
 */
static void prvSetupHardware( void );

/* Used to detect the idle hook function stalling. */
static volatile unsigned long ulIdleLoops = 0UL;

/* The queue used by both tasks. */
//static QueueHandle_t xQueue = NULL;

void flash_led_fast(void);
void flash_led_fast(void)
{
int j;
	for(j=0; j<50;j++) {
		volatile unsigned int i;	// volatile to prevent optimization

		P1OUT ^= 0x40;				// Toggle P1.0 using exclusive-OR

		i = 7000;					// SW Delay
		do i--;
		while(i != 0);
	}
}

static void prvTxTask( void *pvParameters )
{
BaseType_t flip_led = 0;
TickType_t xLastWakeTime;

	xLastWakeTime = xTaskGetTickCount();

	for (;;) {
		//vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(200));
		vTaskDelay(10);
		P1OUT ^= 0x40;
		//xQueueSend( xQueue, &flip_led, 0U );
		//flip_led = !flip_led;
	}
}

static void prvRxTask( void *pvParameters )
{
BaseType_t received=0;

	for (;;) {
		/* Wait until something arrives in the queue - this task will block
		indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
		FreeRTOSConfig.h. */
		//xQueueReceive( xQueue, &received, portMAX_DELAY );
		vTaskDelay(10);
		P1OUT ^= 0x01;
	}
}

/*-----------------------------------------------------------*/

/*
 * Start the demo application tasks - then start the real time scheduler.
 */
int main( void )
{
	/* Setup the hardware ready for the demo. */
	prvSetupHardware();
	flash_led_fast();

	/* Create the standard demo application tasks. */
	xTaskCreate(prvTxTask,
		    "",
		    configMINIMAL_STACK_SIZE,
		    NULL,
		    mainLED_TASK_PRIORITY,
		    NULL);
#if 0
	xTaskCreate(prvRxTask,
		    "",
		    configMINIMAL_STACK_SIZE,
		    NULL,
		    mainLED_TASK_PRIORITY,
		    NULL);
#endif
	/* Create the queue. */
	//xQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(BaseType_t));
//	if (!xQueue)
//		return -1;

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* As the scheduler has been started the demo applications tasks will be
	executing and we should never get here! */
	//flash_led_fast();
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
	P1OUT |= 0x1;
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void );
void vApplicationIdleHook( void )
{
static short count;
//BaseType_t received=0;
	/* Simple put the CPU into lowpower mode. */
	//_BIS_SR( LPM3_bits );
	//xQueueReceive( xQueue, &received, portMAX_DELAY );

	ulIdleLoops++;
}
/*-----------------------------------------------------------*/
