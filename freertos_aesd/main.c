/*
Author : Madhukar Arora
File name : main.c
@Brief : Tests UART LOOPBACK on TIVA BOARD
Reference : https://www.cse.iitb.ac.in/~erts/html_pages/Resources/Tiva/TM4C123G_LaunchPad_Workshop_Workbook.pdf
https://www.freertos.org/a00018.html

Edited by: Akshita Bhasin
        -- To include changes for Message Queue Testing
 */
#include "uart.h"

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

//SemaphoreHandle_t g_pSemaphore1, g_pSemaphore2, g_pSemaphore3;
//QueueHandle_t xMessage_queue;



//typedef struct Msg_que
//{
//    char MQ_NAME;
//    uint32_t MQ_DATA;
//}MessageQueueStruct;

//void vTwoMultiplesTask1(void * pvParameters)
//{
//    MessageQueueStruct mult_two;
//    mult_two.MQ_DATA = 0;
//    uint16_t i = 1;
//    while(i<256)
//    {
//        if(xSemaphoreTake(g_pSemaphore2, ( TickType_t ) 0) == pdTRUE )
//        {
//            mult_two.MQ_NAME = 2;
//            mult_two.MQ_DATA += 2;
//
//            if(xQueueSend(xMessage_queue, (void *)&mult_two,(TickType_t)20) == pdPASS)
//            {
//                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
//                UARTprintf("Multiple of 2 at %d = %d\n",i++, mult_two.MQ_DATA);
//                xSemaphoreGive(g_pUARTSemaphore);
//            }
//
//            xSemaphoreGive(g_pSemaphore3);
//            SysCtlDelay(SysCtlClockGet()/(20 * 3));
//        }
//    }
//}
//
//
//void vSevenMultiplesTask2( void * pvParameters )
//{
//    MessageQueueStruct mult_seven;
//
//    mult_seven.MQ_DATA= 0;
//    uint16_t i=1;
//    while(i<256)
//    {
//
//        if( xSemaphoreTake(g_pSemaphore1, ( TickType_t ) 0) == pdTRUE )
//        {
//
//            mult_seven.MQ_NAME = 7;
//            mult_seven.MQ_DATA += 7;
//
//            if(xQueueSend(xMessage_queue, (void *)&mult_seven, (TickType_t)20) == pdPASS)
//            {
//
//                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
//                UARTprintf("Multiple of 7 at %d = %d\n",i++, mult_seven.MQ_DATA);
//                xSemaphoreGive(g_pUARTSemaphore);
//            }
//
//
//            xSemaphoreGive(g_pSemaphore2);
//            SysCtlDelay(SysCtlClockGet()/(20 * 3));
//        }
//    }
//}


//void vMessageQueueTask3(void * pvParameters)
//{
//    MessageQueueStruct data;
//
//    while(1)
//    {
//        if(xSemaphoreTake(g_pSemaphore3, (TickType_t)0) == pdTRUE )
//        {
//            if(xQueueReceive(xMessage_queue, &(data), (TickType_t)20))
//            {
//                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
//                xSemaphoreGive(g_pUARTSemaphore);
//            }
//            if(xQueueReceive(xMessage_queue, &(data), (TickType_t)20))
//            {
//                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
//                xSemaphoreGive(g_pUARTSemaphore);
//            }
//
//            xSemaphoreGive(g_pSemaphore1);
//
//        }
//    }
//
//}


SemaphoreHandle_t g_pTask1, g_pTask2;

int calculate_multiple(int num, int multiplier)
{
   return num * multiplier;
}

// task handlers
void vTwoMultiplesTask1(void *pvParameters);
void vSevenMultiplesTask2(void *pvParameters);


void vTwoMultipleTask1(void *pvParameters)
{
    static int i = 1;
    int result;
    while(i < 256)
    {

        xSemaphoreTake(g_pTask2,portMAX_DELAY);
        result = calculate_multiple(2,i);
        i++;
        xSemaphoreTake(g_pUARTSemaphore,portMAX_DELAY);
        UARTprintf("\nMultiple of 2 at %d = %d",i,result);
        xSemaphoreGive(g_pUARTSemaphore);
        xSemaphoreGive(g_pTask1);
    }
}

void vSevenMultipleTask2(void *pvParameters)
{
    static int j = 1;
    int result;
    while(j < 256)
    {
        xSemaphoreTake(g_pTask1,portMAX_DELAY);
        result = calculate_multiple(7,j);
        j++;
        xSemaphoreTake(g_pUARTSemaphore,portMAX_DELAY);
        UARTprintf("\nMultiple of 7 at %d = %d",j,result);
        xSemaphoreGive(g_pUARTSemaphore);
        xSemaphoreGive(g_pTask2);
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

//    g_pSemaphore1 = xSemaphoreCreateBinary();
//    g_pSemaphore2 = xSemaphoreCreateBinary();
//    g_pSemaphore3 = xSemaphoreCreateBinary();

//    if((g_pSemaphore3 == NULL) || (g_pSemaphore2 == NULL) || (g_pSemaphore1 == NULL))
//    {
//        UARTprintf("\n\rSemaphore not created.");
//    }

    g_pTask1 = xSemaphoreCreateBinary();
    g_pTask2 = xSemaphoreCreateBinary();

    if(g_pTask1 == NULL)
    {
        xSemaphoreTake(g_pUARTSemaphore,portMAX_DELAY);
        UARTprintf("\n\rBinary task 1 not created");
        xSemaphoreGive(g_pUARTSemaphore);
    }

    if(g_pTask2 == NULL)
    {
        xSemaphoreTake(g_pUARTSemaphore,portMAX_DELAY);
        UARTprintf("\n\rBinary task 2 not created");
        xSemaphoreGive(g_pUARTSemaphore);
    }

    //enables processor interrupts
    IntMasterEnable();

//    enable interrupt for UART
    IntEnable(INT_UART1);
    IntEnable(INT_UART3);

//    enable receive interrupt on module 3 and transmit interrupt on module 1
    UARTIntEnable(UART1_BASE, UART_INT_TX);
    UARTIntEnable(UART3_BASE, UART_INT_RX);

//    xMessage_queue = xQueueCreate(100, sizeof(MessageQueueStruct) );
//
//    if(xMessage_queue == NULL )
//    {
//        UARTprintf("Queue not created\n");
//    }
//
//    xTaskCreate(vMessageQueueTask3, (const char *)"Message Queue Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
//    xTaskCreate(vSevenMultiplesTask2, (const char *)"Multiple of seven Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
//    xTaskCreate(vTwoMultiplesTask1, (const char *)"Multiple of Two Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
//
//    xSemaphoreGive(g_pSemaphore1);


    xTaskCreate(vTwoMultipleTask1,"Multiple of 2",1000,NULL,2,NULL);
    xTaskCreate(vSevenMultipleTask2,"Multiple of 7",1000,NULL,2,NULL);

    xSemaphoreGive(g_pTask2);

    //
    // Start the scheduler.  This should not return.
    //
    vTaskStartScheduler();

    UARTprintf("\n\r Board Transmitting\n\r");
    //UARTprintf("\n\r Board Receiving");
    while(true)
    {

//        if(UARTCharsAvail(UART3_BASE))
//        {
//            char single_char = ROM_UARTCharGet(UART3_BASE);
//            UARTprintf("%c",single_char);
//        }

        send_string();
    }
}

