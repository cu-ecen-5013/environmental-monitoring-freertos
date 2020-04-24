/*
 * processing_bbb.c
 *
 *  Created on: Apr 19, 2020
 *      Author: Akshita Bhasin
 */

#include "../include_files/processing_bbb.h"

extern xSemaphoreHandle g_pUARTSemaphore;
extern QueueHandle_t xReceiveMsgQueue;
extern uint32_t sensor_value;

uint8_t buzzer_flag = 0;
uint8_t led_set, led_indicator=10;

extern struct receive_msg_queue recv_mq;

struct send_msg_queue LED;
struct send_msg_queue buzzer;

struct send_msg_queue *LED_ptr = &LED;
struct send_msg_queue *buzzer_ptr = &buzzer;

void vProcessingTask(void *pvParameters)
{
    UARTprintf("In processing task\n");
    UARTprintf("Sensor: %d, value: %d\n", recv_mq.sensor_ID, recv_mq.sensor_value);
    while(1)
    {
        if(xQueueReceive(xReceiveMsgQueue, &(recv_mq), (TickType_t)20))
        {
            UARTprintf("In queue receive\n");
            //TMP102
            if (recv_mq.sensor_ID == 1)
            {
                if (recv_mq.sensor_value >= 26)
                {
                    buzzer_flag  = 1;
                    buzzer.actuator_ID=1;
                    buzzer.actuator_value=1;
                    UARTprintf("Buzzer ID %d, value %d\n", buzzer_ptr->actuator_ID, buzzer_ptr->actuator_value);
                    send_string((char *)buzzer_ptr, sizeof(buzzer));
                }

                else
                {
                    if (buzzer_flag == 1)
                    {
                        buzzer.actuator_ID=1;
                        buzzer.actuator_value=0;
                        send_string((char *)buzzer_ptr, sizeof(buzzer));
                        buzzer_flag =0;
                    }
                }
            }
            //Add ambient sensor processing
        }
    }
}
