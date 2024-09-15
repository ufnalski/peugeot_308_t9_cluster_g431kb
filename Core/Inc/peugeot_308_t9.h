/*
 * mustang.h
 *
 *  Created on: Jan 9, 2024
 *      Author: user
 */

// Sources:
// https://autowp.github.io/

#ifndef INC_PEUGEOT_308_T9_H_
#define INC_PEUGEOT_308_T9_H_

#include "main.h"
#include <string.h>

void send_ignition_on(uint8_t lcd_brightness, uint32_t delay_before_send);
void send_christmas_lights(uint32_t delay_before_send);
void send_odometer(uint8_t coolant_temp, float odometer,
		uint32_t delay_before_send);
void send_trip_distance(uint32_t delay_before_send);
void send_fuel_level(uint16_t fuel, uint32_t delay_before_send);
void send_rpm_and_speed(uint16_t rpm, uint16_t speed,
		uint32_t delay_before_send);

#endif /* INC_PEUGEOT_308_T9_H_ */
