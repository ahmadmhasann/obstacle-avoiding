/*
* ATMEGA32_TEST.c
*
* Created: 4/17/2020 1:42:10 AM
* Author : Hassan
*/

#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include "macros.h"
#include "types.h"
#include "dio.h"
#include "ultrasonic_sensor.h"
#include "lcd.h"

#define SERVO_SIGNAL_PORT 3
#define SERVO_SIGNAL_PIN 0

#define RIGHT_DURATION 1200
#define LEFT_DURATION 2400
#define FORWARD_DURATION 1900

#define ROBOT_PORT 0
enum move {
	STOP = '0',
	FORWARD = '1',
	BACKWARD = '2',
	RIGHT = '3',
	LEFT = '4'
};

void setServoDuty(u16 duartion) {
	u8 i;
	for (i=0; i<5; i++) {
		dio_vid_set_pin_value(SERVO_SIGNAL_PORT, SERVO_SIGNAL_PIN, HIGH);
		_delay_us(duartion);
		dio_vid_set_pin_value(SERVO_SIGNAL_PORT, SERVO_SIGNAL_PIN, LOW);
		_delay_us(20000-duartion);
	}
}

void moveRobot (u8 moveCommand) {
	switch (moveCommand) {
		case STOP:
		dio_vid_set_port_value(ROBOT_PORT, 0x00);
		break;
		case BACKWARD:
		dio_vid_set_port_value(ROBOT_PORT, 0x05);
		break;
		case FORWARD:
		dio_vid_set_port_value(ROBOT_PORT, 0x0A);
		break;
		case RIGHT:
		dio_vid_set_port_value(ROBOT_PORT, 0x08);
		break;
		case LEFT:
		dio_vid_set_port_value(ROBOT_PORT, 0x02);
		break;
	}
}



int main (void)
{
	
	u16 distance = 0, rightDistance = 0, leftDistance = 0;
	dio_vid_set_port_direction(ROBOT_PORT, 0xff);
	dio_vid_set_port_value(ROBOT_PORT, 0x00);
	dio_vid_set_pin_direction(SERVO_SIGNAL_PORT, SERVO_SIGNAL_PIN, OUTPUT);
	while (1) {
		setServoDuty(FORWARD_DURATION);
		distance = ultrasonic_u16_get_distance();
// 		setServoDuty(1500);
// 		rightDistance = ultrasonic_u16_get_distance();
// 		setServoDuty(2300);
// 		leftDistance = ultrasonic_u16_get_distance();

		if (distance>20) {
			moveRobot(FORWARD);
		}
		else {
			while (1) {
				moveRobot(STOP);
				setServoDuty(1000);
				_delay_ms(50);
				rightDistance = ultrasonic_u16_get_distance();
				setServoDuty(2500);
				_delay_ms(50);
				leftDistance = ultrasonic_u16_get_distance();
				if (rightDistance>20 || leftDistance>20) {
					setServoDuty(FORWARD_DURATION);
					moveRobot(BACKWARD);
					_delay_ms(800);
					moveRobot(STOP);
					rightDistance>leftDistance? moveRobot(RIGHT) : moveRobot(LEFT);
					_delay_ms(1500);
					moveRobot(STOP);
					break;
				}
				else {
					moveRobot(FORWARD);
					setServoDuty(FORWARD_DURATION);
					moveRobot(BACKWARD);
					_delay_ms(800);
					moveRobot(STOP);
				}
				

			}
		}
	}
	

	
	
}

