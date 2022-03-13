#include <Arduino.h>
#include "motor.h"
//Constructor
motor::motor(const int motor_pin_1, const int motor_pin_2)
{
	_pin1 = motor_pin_1;
	_pin2 = motor_pin_2;
};

void motor::motor_down()
{
	analogWrite(_pin1, 0);
	analogWrite(_pin2, 255);
}

void motor::motor_up()
{
	analogWrite(_pin2, 0);
	analogWrite(_pin1, 255);
}

void motor::motor_stop()
{
	analogWrite(_pin1, 0);
	analogWrite(_pin2, 0);
}

void motor::motor_suspend()
{
	//do nothing with Motor Pins
}
