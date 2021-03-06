/*
   drv_mb1242.c : driver for MaxBotix MB1242 sonar

   Copyright (C) 2016 Simon D. Levy

   This file is part of BreezySTM32.

   BreezySTM32 is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   BreezySTM32 is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with BreezySTM32.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <breezystm32.h>

#include <stdbool.h>
#include <stdint.h>

#include "drv_i2c.h"

#define MB1242_DEFAULT_ADDRESS 0x70

static void update_timed_task(uint32_t * usec, uint32_t period)
{
    *usec = micros() + period;
}

static bool check_and_update_timed_task(uint32_t * usec, uint32_t period)
{

    bool result = (int32_t)(micros() - *usec) >= 0;

    if (result)
        update_timed_task(usec, period);

    return result;
}

static int32_t distance_cm;

static void adjust_reading(void) {

    distance_cm = 1.071 * distance_cm + 3.103; // emprically determined
}

static bool attempt_write(uint8_t addr)
{
    return i2cWrite(addr, 0xFF, 0x51);
}

bool mb1242_init(mb1242_t * mb1242, uint8_t addr)
{
    if (!addr)
        mb1242->address = MB1242_DEFAULT_ADDRESS;

    mb1242->time = 0;
    mb1242->state = 0;

    // The only way to know if a sonar is attached is to try to get a reading (doesn't always ACK on i2c)
    mb1242_poll(mb1242);
    delay(200);    // You have to wait 200 ms for the sensor to read
    return (mb1242_poll(mb1242) > 0); // if you have a measurement, return true, otherwise, there was no sonar attached
}

int32_t mb1242_poll(mb1242_t * mb1242)
{
    if (check_and_update_timed_task(&mb1242->time, 10000)) {

        if (mb1242->state == 0) {
            if (attempt_write(mb1242->address))
                mb1242->state++;
        }
        else if (mb1242->state == 1) {
            uint8_t bytes[2];
            if (i2cRead(mb1242->address, 0xFF, 2, bytes)) {
                distance_cm = (bytes[0] << 8) + bytes[1];
                adjust_reading();
                mb1242->state++;
            }
        }
        else {
            mb1242->state = 0;
        }
    }

    return distance_cm;
}
