//*****************************************************************************
//
// freertos_demo.c - Simple FreeRTOS example.
//
// Copyright (c) 2012-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

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
#include "timers.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>FreeRTOS Example (freertos_demo)</h1>
//!
//! This application demonstrates the use of FreeRTOS on Launchpad.
//!
//! The application blinks the user-selected LED at a user-selected frequency.
//! To select the LED press the left button and to select the frequency press
//! the right button.  The UART outputs the application status at 115,200 baud,
//! 8-n-1 mode.
//!
//! This application utilizes FreeRTOS to perform the tasks in a concurrent
//! fashion.  The following tasks are created:
//!
//! - An LED task, which blinks the user-selected on-board LED at a
//!   user-selected rate (changed via the buttons).
//!
//! - A Switch task, which monitors the buttons pressed and passes the
//!   information to LED task.
//!
//! In addition to the tasks, this application also uses the following FreeRTOS
//! resources:
//!
//! - A Queue to enable information transfer between tasks.
//!
//! - A Semaphore to guard the resource, UART, from access by multiple tasks at
//!   the same time.
//!
//! - A non-blocking FreeRTOS Delay to put the tasks in blocked state when they
//!   have nothing to do.
//!
//! For additional details on FreeRTOS, refer to the FreeRTOS web page at:
//! http://www.freertos.org/
//
//*****************************************************************************


//*****************************************************************************
//
// The mutex that protects concurrent access of UART from multiple tasks.
//
//*****************************************************************************
xSemaphoreHandle g_pUARTSemaphore;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}

#endif

//*****************************************************************************
//
// This hook is called by FreeRTOS when an stack overflow error is detected.
//
//*****************************************************************************
void
vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

/*
 * Function : Configure_TX
 * Parameters : void
 * Return Type: void
 * @Brief : using UART Module 2 to Transmit the received
 */
void Configure_TX(void)
{

    //xSemaphoreTake(g_pUARTSemaphore,portMAX_DELAY);
    UARTprintf("\n\rConfiguring Transmission");
    //xSemaphoreGive(g_pUARTSemaphore);
    // Enable UART peripheral used by UART Module 2 on PORT D
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    //
    // Enable UART2
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PD6_U2RX);
    ROM_GPIOPinConfigure(GPIO_PD7_U2TX);
    ROM_GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART2_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    //UARTStdioConfig(2, 115200, 16000000);
    UARTConfigSetExpClk(UART2_BASE, SysCtlClockGet(), 115200,
                           (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                            UART_CONFIG_PAR_NONE));
 }

/*
 * Function : Configure_RX
 * Parameters : void
 * Return Type : void
 * @Brief : Receives the message on UART Module3 on Port C
 */
void Configure_RX(void)
{

    UARTprintf("\n\rConfiguring Receive");

    // Enable UART peripheral used by UART Module 3 on PORT C
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    //
    // Enable UART3
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PC6_U3RX);
    ROM_GPIOPinConfigure(GPIO_PC7_U3TX);
    ROM_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART3_BASE, UART_CLOCK_PIOSC);
    UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), 115200,
                           (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                            UART_CONFIG_PAR_NONE));
    //
    // Initialize the UART for console I/O.
    //
   // UARTStdioConfig(3, 115200, 16000000);

}


/*
 * Task Handles
 */
SemaphoreHandle_t transmit;
SemaphoreHandle_t receive;

/*
 * Task Handlers
 */
void Transmit(void *pvParameters);
void Receive(void *pvParameters);


//*****************************************************************************
//
// Initialize FreeRTOS and start the initial set of tasks.
//
//*****************************************************************************
int
main(void)
{
    //
    // Set the clocking to run at 50 MHz from the PLL.
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);

    //
    // Initialize the UART and configure it for 115,200, 8-N-1 operation.
    //
    ConfigureUART();
    Configure_TX(); //configure UART Module 2 as Transmitter
    Configure_RX(); // configure UART Module 3 as Receiver

    //
    // Print demo introduction.
    //
    UARTprintf("\n\nFree RTOS Setup for AESD Final Project!\n");

    //
    // Create a mutex to guard the UART.
    //
    g_pUARTSemaphore = xSemaphoreCreateMutex();


    transmit = xSemaphoreCreateBinary();
    if( transmit == NULL )
    {
        UARTprintf("\n\rBinary Task Transmit not created.");
    }

    receive = xSemaphoreCreateBinary();
    if( receive == NULL )
    {
        UARTprintf("\n\rBinary Task Receive not created.");
    }


    xTaskCreate( Transmit, "Transmit", 1000, NULL, 2, NULL);
    xTaskCreate( Receive, "Receive", 1000, NULL, 2, NULL);

    xSemaphoreGive(receive);







    //
    // Create the LED task.
    //
//    if(LEDTaskInit() != 0)
//    {
//
//        while(1)
//        {
//        }
//    }

    //
    // Create the switch task.
    //
//    if(SwitchTaskInit() != 0)
//    {
//
//        while(1)
//        {
//        }
//    }

    //
    // Start the scheduler.  This should not return.
    //
    vTaskStartScheduler();

    //
    // In case the scheduler returns for some reason, print an error and loop
    // forever.
    //

    while(1)
    {
    }
}


void Transmit(void *pvParameters)
{
    UARTprintf("\n\rTransmitting");
    while(1)
    {
        xSemaphoreTake(receive,portMAX_DELAY);
        uint8_t sensor_values[5] = {1,2,3,4,5}; //proxy sensor values to display
        uint8_t i = 0;
        while(i < 5)
        {
            UARTCharPutNonBlocking(UART2_BASE,sensor_values[i]);
            i++;
        }
        xSemaphoreGive(transmit);
    }

}


void Receive(void *pvParameters)
{
    UARTprintf("\n\rReceiving");
    while(1)
    {
        xSemaphoreTake(transmit,portMAX_DELAY);
        while(UARTCharsAvail(UART3_BASE))
        {
        uint8_t recv_val = UARTCharGetNonBlocking(UART3_BASE);
        UARTprintf("\n\rReceived Value : %d",recv_val);
        }
        xSemaphoreGive(receive);
    }
}
