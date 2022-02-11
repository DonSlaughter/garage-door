#include <Arduino.h>
#include "main.h"


void setup()
{
	pinMode(motor_pin_1, OUTPUT);
	pinMode(motor_pin_2, OUTPUT);
	delay(1000);
}

void loop()
{
	analogWrite(motor_pin_1, 128);
}
