/*
* ultrasonic_sensor.c
*
* Created: 5/7/2020 03:50:16
*  Author: Hassan
*/

#include <avr/io.h>
#define F_CPU 8000000
#include <util/delay.h>
#include "macros.h"
#include "types.h"
#include "dio.h"
#include "timer.h"
#include "ultrasonic_sensor_cfg.h"
volatile u64 ultrasonic_counter=0;

void ultrasonic_timer (void) {
	ultrasonic_counter++;
}

u16 ultrasonic_u16_get_distance () {
	u64 ultrasonic_timer_ticks = 0;
	u16 distance = 0;
	dio_vid_set_pin_direction(TRIG_PORT, TRIG_PIN, OUTPUT);
	/*INT0, ECHO*/
	dio_vid_set_pin_direction(ECHO_PORT, ECHO_PIN, INPUT);
	dio_vid_set_pin_value(ECHO_PORT, ECHO_PIN, HIGH);
	timer_vid_init0_ctc(1,255,1);
	timer_vid_set_isr_timer0_ctc(ultrasonic_timer);
	/* Give 10us trigger pulse on trig pin */
	dio_vid_set_pin_value(TRIG_PORT, TRIG_PIN, HIGH);
	_delay_us(10);
	dio_vid_set_pin_value(TRIG_PORT, TRIG_PIN, LOW);
	TCNT0 = 0;	/* Clear Timer counter */
	MCUCR = 0x03; /*The rising edge of INT0 generates an interrupt request*/
	while (!getBit(GIFR, INTF0));  /*Wait for rising edge */
	setBit(GIFR, INTF0); /*Clear the interrupt flag*/
	TCNT0 = 0;	 /*Clear Timer counter */
	ultrasonic_counter = 0;
	MCUCR = 0x02; /*The falling edge of INT0 generates an interrupt request*/
	while (!getBit(GIFR, INTF0));  /*Wait for falling edge */
	setBit(GIFR, INTF0); /*Clear the interrupt flag*/
	ultrasonic_timer_ticks = TCNT0 + (255 * ultrasonic_counter);
	clrBit(SREG, 7);
	distance = (u64)((34300 * (ultrasonic_timer_ticks/2))/8000000); /*distance = velocity*time = velocity/frequency*/
	_delay_ms(100);
	return distance;
}

