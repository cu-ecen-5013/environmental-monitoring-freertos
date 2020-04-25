/*
 * processing_bbb.c
 *
 *  Created on: Apr 19, 2020
 *      Author: Akshita Bhasin
 */

#include "../include_files/processing_bbb.h"

extern QueueHandle_t xReceiveMsgQueue;

uint8_t buzzer_flag = 0;

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
//            recv_mq.sensor_ID = 2;
            UARTprintf("In queue receive\n");
            //TMP102
            if (recv_mq.sensor_ID == 1)
            {
                if (recv_mq.sensor_value > 28)
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
            if (recv_mq.sensor_ID == 2)
            {
                if (recv_mq.sensor_value < 30)
                {
                    LED.actuator_ID=0;
                    LED.actuator_value=255;
                    UARTprintf("Actuator ID %d\n", LED_ptr->actuator_ID);
                    UARTprintf("Actuator value %d\n", LED_ptr->actuator_value);
                    send_string((char *)LED_ptr, sizeof(LED));
                }
                else
                {
                    LED.actuator_ID=0;
                    LED.actuator_value=0;
                    UARTprintf("Actuator ID %d\n", LED_ptr->actuator_ID);
                    UARTprintf("Actuator value %d\n", LED_ptr->actuator_value);
                    send_string((char *)LED_ptr, sizeof(LED));
                }
            }
        }
    }
}
