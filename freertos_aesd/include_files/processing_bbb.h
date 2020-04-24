/*
 * processing_bbbb.h
 *
 *  Created on: Apr 19, 2020
 *      Author: Akshita Bhasin
 */

#ifndef INCLUDE_FILES_PROCESSING_BBBB_H_
#define INCLUDE_FILES_PROCESSING_BBBB_H_

/****************************************************************************
 * INCLUDES                                                                 *
 ****************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "led_task.h"
#include "switch_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// Header files for interrupts
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"

#include "uart.h"

void vProcessingTask(void *pvParameters);

#endif /* INCLUDE_FILES_PROCESSING_BBBB_H_ */
