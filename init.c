/*
	Joshua Beard

	+-------------------------------------------------------------------------------+
      	|	RPi_Smoke_Gas:							    	|
      	|	Use a Raspberry Pi to get and interpret smoke and gas concentration	|
      	|	from a MQ-2 sensor via a PCF8951 A/D converter.			    	|
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
#define DEVICE_ID 0x48
#define PCF 120

float RsAir, Ro, baselineVolt = 0.0;
// Scale 8-bit digital input to eq. voltage: 255/5 = 51
const float divisor = 51.0;

int main(int argc, char *argv[])
{
	wiringPiSetup();

	// Check for errors in setup
	if(pcf8591Setup(PCF, DEVICE_ID) == -1)
	{
		printf("Error setting up connection to PCF8591\n");
		return -1;
	}
	else if(analogRead(PCF) == -1)
	{
		printf("Error reading from ADC\n");
		return -1;
	}
	// Everything's ok
	else
	{

		// Get sum of analogRead 100 times for an average
		for(int i = 0; i < 100; i++)
		{
			baselineVolt = baselineVolt + analogRead(PCF);
			delay(100);
		}
		// Divide by 100 to get average and by 51 to get voltage
		baselineVolt = baselineVolt/5100;

		// Calculate RsAir and Ro
		RsAir = (5-baselineVolt)/baselineVolt;
		Ro = RsAir/9.8;

		// PRINTING FOR DEBUG/INFO
		printf("RsAir=%f\n", RsAir);

		// Uncomment the following section to see simple execution instructions every time
		/*
		printf("To call gas reading function, use the following command:\n sensorRead %f\n", RsAir);
		*/
	}
return 0;
}
