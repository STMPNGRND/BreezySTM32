/*
   ms4525.c : Airpseed Measurement Values

   Copyright (C) 2016 James Jackson

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


void setup(void)
{
    delay(500);
    i2cInit(I2CDEV_2);
}

int16_t velocity;
int16_t temp;

void loop(void)
{
    if( ms4525_detect() )
    {
        ms4525_read(&velocity, &temp);
        printf("velocity = %d, temp = %d\n", velocity, temp);
    }
    else
    {
        printf("no airspeed\n");
    }
    delay(10);
}

