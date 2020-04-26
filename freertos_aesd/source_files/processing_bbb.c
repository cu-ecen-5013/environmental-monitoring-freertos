/*
 * processing_bbb.c
 *
 *  Created on: Apr 19, 2020
 *      Author: Akshita Bhasin
 */

#include "../include_files/processing_bbb.h"

extern QueueHandle_t xReceiveMsgQueue;
extern SemaphoreHandle_t g_pReceiveSem, g_pSendSem;

uint8_t buzzer_flag = 0;

extern struct receive_msg_queue recv_mq;

struct send_msg_queue LED;
struct send_msg_queue buzzer;

struct send_msg_queue *LED_ptr = &LED;
struct send_msg_queue *buzzer_ptr = &buzzer;

void vProcessingTask(void *pvParameters)
{
    UARTprintf("In processing task\n");
    while(1)
    {
        if(xSemaphoreTake(g_pReceiveSem, ( TickType_t ) 0) == pdTRUE )
        {
            if(xQueueReceive(xReceiveMsgQueue, &(recv_mq), (TickType_t)20))
            {
                UARTprintf("Sensor: %d, value: %d\n", recv_mq.sensor_ID, recv_mq.sensor_value);
    //            recv_mq.sensor_ID = 2;
                UARTprintf("In queue receive\n");
                //TMP102
                if (recv_mq.sensor_ID == 1)
                {
                    if (recv_mq.sensor_value > 23)
                    {
                        buzzer_flag  = 1;
                        buzzer.actuator_ID=1;
                        buzzer.actuator_value=1;
                        UARTprintf("Buzzer ID %d, value %d\n", buzzer_ptr->actuator_ID, buzzer_ptr->actuator_value);
                        send_string((char *)buzzer_ptr, sizeof(buzzer));
                    }

                    else
                    {
                        buzzer_flag  = 0;
                        buzzer.actuator_ID=1;
                        buzzer.actuator_value=0;
                        UARTprintf("Buzzer ID %d, value %d\n", buzzer_ptr->actuator_ID, buzzer_ptr->actuator_value);
                        send_string((char *)buzzer_ptr, sizeof(buzzer));
                    }
                   SysCtlDelay(SysCtlClockGet());
                }
                //Add ambient sensor processing
                if (recv_mq.sensor_ID == 2)
                {
                    UARTprintf("Testing actuation\n\n");
                    if (recv_mq.sensor_value < 35)
                    {
                        LED.actuator_ID=2;
                        LED.actuator_value=255;
                        UARTprintf("sensor value %d\nActuator ID %d\n", recv_mq.sensor_value, LED_ptr->actuator_ID);
                        UARTprintf("Actuator value %d\n", LED_ptr->actuator_value);
                        send_string((char *)LED_ptr, sizeof(LED));
                    }
                    else if (recv_mq.sensor_value > 35)
                    {
                        LED.actuator_ID=2;
                        LED.actuator_value=0;
                        UARTprintf("LED low, sensor value %d\nActuator ID %d\n", recv_mq.sensor_value, LED_ptr->actuator_ID);
                        UARTprintf("Actuator value %d\n", LED_ptr->actuator_value);
                        send_string((char *)LED_ptr, sizeof(LED));
                    }
                }
            }
            xSemaphoreGive(g_pSendSem);
        }
    }
}
