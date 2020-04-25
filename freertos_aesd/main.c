/*
Author : Madhukar Arora
File name : main.c
@Brief : Tests UART LOOPBACK on TIVA BOARD
Reference : https://www.cse.iitb.ac.in/~erts/html_pages/Resources/Tiva/TM4C123G_LaunchPad_Workshop_Workbook.pdf
https://www.freertos.org/a00018.html

Edited by: Akshita Bhasin
        -- To include changes for Message Queue Testing\
        -- Integrated code, for Smart Environmental Monitoring System
 */
#include "include_files/uart.h"
#include "include_files/processing_bbb.h"

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

SemaphoreHandle_t g_pSemaphore1, g_pSemaphore2, g_pSemaphore3;
QueueHandle_t xMessage_queue;
QueueHandle_t xReceiveMsgQueue;

struct receive_msg_queue recv_mq;

typedef struct Msg_que
{
    char MQ_NAME;
    uint32_t MQ_DATA;
}MessageQueueStruct;

void vTwoMultiplesTask1(void * pvParameters)
{
    MessageQueueStruct mult_two;
    mult_two.MQ_DATA = 0;
    uint16_t i = 1;
    while(i<4)
    {
        if(xSemaphoreTake(g_pSemaphore2, ( TickType_t ) 0) == pdTRUE )
        {
            mult_two.MQ_NAME = 2;
            mult_two.MQ_DATA += 2;

            if(xQueueSend(xMessage_queue, (void *)&mult_two,(TickType_t)20) == pdPASS)
            {
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("Multiple of 2 at %d = %d\n",i++, mult_two.MQ_DATA);
                xSemaphoreGive(g_pUARTSemaphore);
            }

            xSemaphoreGive(g_pSemaphore3);
            SysCtlDelay(SysCtlClockGet()/(20 * 3));
        }
    }
}


void vSevenMultiplesTask2( void * pvParameters )
{
    MessageQueueStruct mult_seven;

    mult_seven.MQ_DATA= 0;
    uint16_t i=1;
    while(i<3)
    {

        if( xSemaphoreTake(g_pSemaphore1, ( TickType_t ) 0) == pdTRUE )
        {

            mult_seven.MQ_NAME = 7;
            mult_seven.MQ_DATA += 7;

            if(xQueueSend(xMessage_queue, (void *)&mult_seven, (TickType_t)20) == pdPASS)
            {

                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("Multiple of 7 at %d = %d\n",i++, mult_seven.MQ_DATA);
                xSemaphoreGive(g_pUARTSemaphore);
            }


            xSemaphoreGive(g_pSemaphore2);
            SysCtlDelay(SysCtlClockGet()/(20 * 3));
        }
    }
}


void vMessageQueueTask3(void * pvParameters)
{
    MessageQueueStruct data;

    while(1)
    {
        if(xSemaphoreTake(g_pSemaphore3, (TickType_t)0) == pdTRUE )
        {
            if(xQueueReceive(xMessage_queue, &(data), (TickType_t)20))
            {
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                xSemaphoreGive(g_pUARTSemaphore);
            }
            if(xQueueReceive(xMessage_queue, &(data), (TickType_t)20))
            {
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                xSemaphoreGive(g_pUARTSemaphore);
            }

            xSemaphoreGive(g_pSemaphore1);

        }
    }

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

    g_pUARTSemaphore = xSemaphoreCreateMutex();

    UARTprintf("Environmental monitoring system");

    xReceiveMsgQueue = xQueueCreate(300, sizeof(recv_mq));

    if(xReceiveMsgQueue == NULL )
    {
        UARTprintf("Queue not created\n");
    }

    //enables processor interrupts
    IntMasterEnable();

    UARTFIFOLevelSet(UART3_BASE,NULL,UART_FIFO_RX2_8);

//    enable interrupt for UART
    IntEnable(INT_UART1);
    IntEnable(INT_UART3);

//    enable receive interrupt on module 3 and transmit interrupt on module 1
    UARTIntEnable(UART1_BASE, UART_INT_TX);
    UARTIntEnable(UART3_BASE, UART_INT_RX);

    xTaskCreate(vProcessingTask, (const portCHAR *)"Sensors to actuators", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    UARTprintf("\n\r Board Transmitting\n\r");

    //
    // Start the scheduler.  This should not return.
    //
    vTaskStartScheduler();

    while(1)
    {
    }
}


