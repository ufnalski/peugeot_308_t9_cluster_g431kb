/*
 * focus.c
 *
 *  Created on: Jan 9, 2024
 *      Author: user
 *
 * Sources:
 *    https://autowp.github.io/
 *
 */

#include "peugeot_308_t9.h"
#include "fdcan.h"
#include <math.h>

extern FDCAN_TxHeaderTypeDef TxHeader;
extern uint8_t TxData8[8];

// ------------------------------------------------------- 0x36 (ignition) -----------------
void send_ignition_on(uint8_t lcd_brightness, uint32_t delay_before_send)
{
	HAL_Delay(delay_before_send);
	TxHeader.DataLength = FDCAN_DLC_BYTES_8;
	TxHeader.Identifier = 0x36;
	memset(TxData8, 0, sizeof(TxData8));
	TxData8[0] = 0b00001110;  // ignition on -> activates arrows!!!
	TxData8[1] = 0;
	TxData8[2] = 0;
	TxData8[3] = 0b00100000 | lcd_brightness;
	TxData8[4] = 0b00000001;
	TxData8[5] = 0;
	TxData8[6] = 0;
	TxData8[7] = 0b10100000;
	if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData8) != HAL_OK)
	{
		Error_Handler();
	}
}

//----------------------------------- 0x128 (many indicators) ---------------------------
void send_christmas_lights(uint32_t delay_before_send)
{
	HAL_Delay(delay_before_send);
	TxHeader.DataLength = FDCAN_DLC_BYTES_7;
	TxHeader.Identifier = 0x128;
	memset(TxData8, 0, sizeof(TxData8));
	TxData8[0] = 0xFF;
	TxData8[1] = 0xFF;
	TxData8[2] = 0xFF;
	TxData8[3] = 0xFF;
	TxData8[4] = 0xFF;
	TxData8[5] = 0xFF;
	TxData8[6] = 0xFF;
	if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData8) != HAL_OK)
	{
		Error_Handler();
	}
}

//----------------------------------- 0x1A8 (trip distance) ---------------------------
void send_trip_distance(uint32_t delay_before_send)
{
	HAL_Delay(delay_before_send);
	TxHeader.DataLength = FDCAN_DLC_BYTES_8;
	TxHeader.Identifier = 0x1A8;
	memset(TxData8, 0, sizeof(TxData8));
	TxData8[0] = 0;
	TxData8[1] = 0;
	TxData8[2] = 0;
	TxData8[3] = 0;
	TxData8[4] = 0;
	TxData8[5] = 0x04;
	TxData8[6] = 0;
	TxData8[7] = 0;
	if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData8) != HAL_OK)
	{
		Error_Handler();
	}
}

// --------------------------------------------------- 0xF6 (odometer) ----------------------
void send_odometer(uint8_t coolant_temp, float odometer,
		uint32_t delay_before_send)
{
	coolant_temp += 64;  // probably an incorrect formula
	odometer *= 10;
	uint32_t mileage = round(odometer);
	HAL_Delay(delay_before_send);
	TxHeader.DataLength = FDCAN_DLC_BYTES_8;
	TxHeader.Identifier = 0xF6;
	memset(TxData8, 0, sizeof(TxData8));
	TxData8[0] = (uint8_t) 0x8e;  //0x88;
	TxData8[1] = coolant_temp;
	TxData8[2] = (uint8_t) (mileage >> 16);
	TxData8[3] = (uint8_t) (mileage >> 8);
	TxData8[4] = (uint8_t) mileage;
	TxData8[5] = 0x6f;
	TxData8[6] = (uint8_t) round(25.0 / 2.0 - 39.5);
	TxData8[7] = 0x23;
	if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData8) != HAL_OK)
	{
		Error_Handler();
	}
}

// ------------------------------------------- 0xB6 (rpm and ground speed) ------------------
void send_rpm_and_speed(uint16_t rpm, uint16_t speed,
		uint32_t delay_before_send)
{
	HAL_Delay(delay_before_send);
	TxHeader.DataLength = FDCAN_DLC_BYTES_8;
	TxHeader.Identifier = 0xB6;
	memset(TxData8, 0, sizeof(TxData8));
	TxData8[0] = (uint8_t) (rpm >> 5);
	TxData8[1] = (uint8_t) (rpm << 3);
	TxData8[2] = (uint8_t) ((speed * 100) >> 8);
	TxData8[3] = (uint8_t) (speed * 100);
	TxData8[4] = 0;
	TxData8[5] = 0;
	TxData8[6] = 0;
	TxData8[7] = 0; //0xd0;
	if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData8) != HAL_OK)
	{
		Error_Handler();
	}
}

// ------------------------------------------------- 0x161 (fuel level) ----------------------
void send_fuel_level(uint16_t fuel, uint32_t delay_before_send)
{
	HAL_Delay(delay_before_send);
	TxHeader.DataLength = FDCAN_DLC_BYTES_7;
	TxHeader.Identifier = 0x161;
	memset(TxData8, 0, sizeof(TxData8));
	TxData8[0] = 0;
	TxData8[1] = 0;
	TxData8[2] = 0b10000000;  // oil temp.
	TxData8[3] = 0b00111111; // fuel level
	TxData8[4] = 0;
	TxData8[5] = 0;
	TxData8[6] = 0;
	TxData8[7] = 0;
	if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData8) != HAL_OK)
	{
		Error_Handler();
	}
}
