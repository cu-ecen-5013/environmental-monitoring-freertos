/*
 * uart.c
 *
 *  Created on: Apr 17, 2020
 *      Author: Akshita Bhasin
 */

#include "uart.h"


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
 * @Brief : Receives the MessageQueueStruct on UART Module3 on Port C
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

/********************************************************************************************************
 * INTERRUPT FLAGS AND HANDLERS
 */

void send_string(void)
{
    char test_string[] = "Hello Beaglebone!";

    char *string_test = test_string;

    while(*string_test != '\0')
    {
        UARTCharPut(UART1_BASE,*string_test);
        string_test++;
//        SysCtlDelay(SysCtlClockGet()/(10000 * 3));
    }
}

/*
 *
 */
void UART_Transmit_ISR(void)
{
    uint32_t ui32transmit_status;
    ui32transmit_status = UARTIntStatus(UART1_BASE,true);
    UARTIntClear(UART1_BASE,ui32transmit_status);
}

void UART_Receive_ISR(void)
{
    uint32_t ui32receive_status;
    ui32receive_status = UARTIntStatus(UART3_BASE,true);
    UARTIntClear(UART3_BASE,ui32receive_status);
    while(UARTCharsAvail(UART3_BASE))
    {
        unsigned char received_data = ROM_UARTCharGet(UART3_BASE);
        UARTprintf("%c",received_data);
        //SysCtlDelay(SysCtlClockGet()/(1000 * 3));
    }
}
