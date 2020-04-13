/*
Author : Madhukar Arora
File name : main.c
@Brief : Tests UART LOOPBACK on TIVA BOARD
 */
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
#include "inc/hw_ints.h"




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
    /***********************uart 0-FOR PRINTING ON CONSOLE************************************************/
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
 * @Brief : using UART Module 1 to Transmit the received
 */

void Configure_TX(void)
{
    // Enable UART peripheral used by UART Module 1 on PORT B for transmission
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Enable UART
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);

    //Configure GPIO Pins for UART mode
    ROM_GPIOPinConfigure(GPIO_PB0_U1RX);
    ROM_GPIOPinConfigure(GPIO_PB1_U1TX);
    ROM_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART1_BASE, UART_CLOCK_PIOSC);
    //
    // Initialize the UART for console I/O.
    //
    ROM_UARTConfigSetExpClk(UART1_BASE, 16000000, 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

}


/*
 * Function : Configure_RX
 * Parameters : void
 * Return Type : void
 * @Brief : Receives the message on UART Module3 on Port C
 */
void Configure_RX(void)
{
    // Enable UART peripheral used by UART Module 3 on PORT C for receiving
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    //Enable UART
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);

    //Configure GPIO Pins for UART mode
    ROM_GPIOPinConfigure(GPIO_PC6_U3RX);
    ROM_GPIOPinConfigure(GPIO_PC7_U3TX);
    ROM_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART3_BASE, UART_CLOCK_PIOSC);
    //
    // Initialize the UART for console I/O.
    //
    ROM_UARTConfigSetExpClk(UART3_BASE,16000000, 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

}

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
    Configure_TX(); //module 1 to transmit | PB1
    Configure_RX(); //module 3 to receive  | PC6


    //adding to try interrupt based UART

    //enables processor interrupts
    IntMasterEnable();

    //enable interrupt for UART
    IntEnable(INT_UART1);
    IntEnable(INT_UART3);

    //enable receive interrupt on module 3 and transmit interrupt on module 1
    UARTIntEnable(UART1_BASE, UART_INT_TX);
    UARTIntEnable(UART3_BASE, UART_INT_RX);





    /********************************************************************************************************
     * INTERRUPT FLAGS
     */

    uint32_t ui32transmit_status, ui32receive_status;



    //UARTprintf("\n\r Board Transmitting");
    UARTprintf("\n\r Board Receiving");


    //CHAR TRANSFER TESTING
    //    UARTCharPutNonBlocking(UART1_BASE,'A');
    if(UARTCharsAvail(UART3_BASE))
    {
        char single_char = ROM_UARTCharGet(UART3_BASE);
        UARTprintf("\n\r Single Character Testing : %c ",single_char);
    }



    //STRING TRANSFER TESTING
        char test_string[]="Hello World!";
        char *ptr=test_string;
        while(*ptr != '\n')
        {
            UARTCharPutNonBlocking(UART1_BASE,*ptr);
            ptr++;
        }

        while(UARTCharsAvail(UART3_BASE)){

            char string_char = ROM_UARTCharGet(UART3_BASE);
            UARTprintf("Received character: %c \r\n", string_char);

        }

    //
    // Start the scheduler.  This should not return.
    //
    vTaskStartScheduler();

    g_pUARTSemaphore = xSemaphoreCreateMutex();
    //
    // In case the scheduler returns for some reason, print an error and loop
    // forever.
    //

    while(1)
    {

    }
}
//
//void Transmit_ISR(void)
//{
//    uint32_t ui32TransmitCheck;
//    ui32TransmitCheck = UARTIntStatus(UART2_BASE,true);
//    UARTIntClear(UART2_BASE,ui32TransmitCheck);
//    UARTprintf("\n\rTransmit done");
//}
//
//
//
//void Receive_ISR(void)
//{
//    uint32_t ui32ReceiveCheck;
//    //check interrupt status
//    ui32ReceiveCheck  = UARTIntStatus(UART3_BASE,true);
//    UARTIntClear(UART3_BASE,ui32ReceiveCheck);
//    while(UARTCharsAvail(UART3_BASE))
//    {
//        unsigned char received = ROM_UARTCharGet(UART3_BASE);
//        UARTprintf("\n\r received = %c",received);
//    }
//
//}





