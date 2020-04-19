/*
 * processing_bbb.c
 *
 *  Created on: Apr 19, 2020
 *      Author: Akshita Bhasin
 */

#include "../include_files/processing_bbb.h"

struct bbb_receive_msq_queue
{
    uint8_t sensor_message_id;
    uint16_t sensor_value;
};

struct bbb_receive_msq_queue MESSAGE_QUEUE;

struct actuator_msg_queue
{
    uint8_t actuator_ID;
    uint8_t actuator_value;
};

struct actuator_msg_queue actuate_LED;
struct actuator_msg_queue actuate_Buzzer;

struct actuator_msg_queue *actuate_LED_ptr = &actuate_LED;
struct actuator_msg_queue *actuate_Buzzer_ptr = &actuate_Buzzer;



