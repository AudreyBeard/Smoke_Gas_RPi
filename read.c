/*
        Joshua Beard

        +-------------------------------------------------------------------------------+
        |       RPi_Smoke_Gas:                                                          |
        |       Use a Raspberry Pi to get and interpret smoke and gas concentration     |
        |       from a MQ-2 sensor via a PCF8951 A/D converter.                         |
        +-------------------------------------------------------------------------------+

        Gets readings from a MQ-2 gas and smoke sensor via an ADC (PCF8591) using I2C.
        Data is 8 bits and is a ratio of RsAir/RoH2, defined as the resistance of the
        sensor at pure air over the resistance of the sensor at 1000 ppm H2. Input
        voltage is 5V, so the ratio is in the range (0, 5).

        Let X = Rs/Ro
        Since Ro is defined as: Ro = RsAir/9.8,
        And Rs/Ro is defined as: (Vin - Vout)/(Vout/Ro)
        Then X = 9.8*(5 - Vout)/(Vout*RsAir)

        Created: 1/25/16
        Last Edited: 2//16

        To compile: gcc FILE_NAME -o EXECUTABLE_NAME -lwiringPi -lm -std=gnu99

        Copyright (C) {year}  {name of author}

        This program is free software: you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation, either version 3 of the License, or
        (at your option) any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <wiringPi.h>
#include <stdio.h>
#include <wiringPiI2C.h>
#include <pcf8591.h>
#include <math.h>
#include <stdlib.h>
#define DEVICE_ID 0x48
#define PCF 120
#define RS_OVER_RO 9.8*(5 - sensorVoltage)/(sensorVoltage*RsAir)

float sensorVoltage, RsAir;
int counter = 0;
float H2_ppm();
float LPG_ppm();
float CH4_ppm();
float CO_ppm();
float Alcohol_ppm();
float Smoke_ppm();
float Propane_ppm();

int main(int argNum, char* argv[])
{
	RsAir = atof(argv[1]);
	// If user passes anything other than RsAir, print erro message and return
	if(argNum != 2)
	{
		printf("\n*ERROR*\nUsage: sudo smokeGasRead <RsAir value>\n");
		return -1;
	}
	else
	{
		// divisor == 255/5, since MQ2 sensor will provide Vout in range [0, Vin]
		const float divisor = 51.0;
		// Equivalent voltage, resistance of sensor in normal air, in 1000 ppm H2
		int data = -1;
		wiringPiSetup();

		// Check for errors in setup
		if(pcf8591Setup(PCF, DEVICE_ID) == -1)
			printf("Error setting up connection to PCF8591\n");
		else if(analogRead(PCF) == -1)
			printf("Error reading from ADC\n");

		// Everything's ok
		else
		{
			sensorVoltage = analogRead(PCF)/divisor;
			// Display equivalent voltage seen by ADC
			printf("Sensor:%fV ", sensorVoltage);
			printf("H2:%fppm ", H2_ppm());
			printf("LPG:%fppm ", LPG_ppm());
			printf("CH4:%fppm ", CH4_ppm());
			printf("CO:%fppm ", CO_ppm());
			printf("Alcohol:%fppm ", Alcohol_ppm());
			printf("Smoke:%fppm ", Smoke_ppm());
			printf("Propane:%fppm ", Propane_ppm());

			// Uncomment the following section to see a more readable version of the data
			/*
			printf("Sensor Voltage | %f V\n", sensorVoltage);
			printf("H2             | %f ppm\n", H2_ppm());
			printf("LPG            | %f ppm\n", LPG_ppm());
			printf("CH4            | %f ppm\n", CH4_ppm());
			printf("CO             | %f ppm\n", CO_ppm());
			printf("Alcohol        | %f ppm\n", Alcohol_ppm());
			printf("Smoke          | %f ppm\n", Smoke_ppm());
			printf("Propane        | %f ppm\n", Propane_ppm());
			*/

		}
	}



return 0;
}

// Hydrogen gas concentration
float H2_ppm()
{
	return 955.9*pow(RS_OVER_RO, -2.1);
}

// Liquefied Petroleum Gas concentration
float LPG_ppm()
{
	return 573.7*pow(RS_OVER_RO, -2.1);
}

// Methane concentration
float CH4_ppm()
{
	return 4232.2*pow(RS_OVER_RO, -2.6);
}

float CO_ppm()
{
	return 27645.1*pow(RS_OVER_RO, -2.9);
}

float Alcohol_ppm()
{
	return 3436.1*pow(RS_OVER_RO, -2.7);
}

float Smoke_ppm()
{
	return 3891.0*pow(RS_OVER_RO, -2.3);
}

float Propane_ppm()
{
	return 625.3*pow(RS_OVER_RO, -2.2);
}
