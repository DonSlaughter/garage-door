#include <Arduino.h>
#include "main.h"

#define Endswitch 1
#define OverCurrent 0
#define ZeroCurrent 1
#define Serial_Debug 1

// Pin Declaration
const int ext_LED = 4;
const int close_button = 5;
const int open_button = 6;
const int signal_button = 7;
const int motor_pin_1 = 8;
const int motor_pin_2 = 9;
const int door_signal = A0;
const int endswitch_1 = A1;
const int endswitch_2 = A2;
const int current_sensor = A3;

//Different States in which this control unit can be in
enum state{
	boot,
	stop,
	up,
	down,
	suspend,
};

state command = stop;
state last_command = boot;

//Class for Automatic Button on PCB an Exteral Door signal (key or switch in
//Garage)
auto_switch pcb_button(signal_button, LOW);
auto_switch door_button(door_signal, HIGH);

void setup()
{
#if Serial_Debug
	Serial.begin(9600);
#endif
	pinMode(ext_LED, OUTPUT);
	pinMode(close_button, INPUT_PULLUP);
	pinMode(open_button, INPUT_PULLUP);
	pinMode(signal_button, INPUT_PULLUP);
	pinMode(motor_pin_1, OUTPUT);
	pinMode(motor_pin_2, OUTPUT);
	pinMode(door_signal, INPUT);
	pinMode(endswitch_1, INPUT_PULLUP);
	pinMode(endswitch_2, INPUT_PULLUP);
	pinMode(current_sensor, INPUT);

	delay(100);
#if Serial_Debug
	Serial.print("Close button : ");
	Serial.println(digitalRead(close_button));
	Serial.print("open button : ");
	Serial.println(digitalRead(open_button));
	Serial.print("signal_button : ");
	Serial.println(digitalRead(signal_button));
	Serial.print("Tür automatic_button : ");
	Serial.println(digitalRead(door_signal));
	Serial.print("Endschalter : ");
	Serial.print(digitalRead(endswitch_1));
	Serial.print("   ");
	Serial.println(digitalRead(endswitch_2));
#endif
}

void loop()
{
	unsigned long current_millis = millis();
	pcb_button.loop(current_millis);
	door_button.loop(current_millis);

#if Endswitch
	if (((digitalRead(endswitch_1) == HIGH) || (digitalRead(endswitch_2) == HIGH)) && command != stop) {
		last_command = command;
		command = stop;
		Serial.println("Endschalter Ausgelöst");
	}
#endif

#if ZeroCurrent
	if (current_value() == 0 && command != stop) {
		Serial.println("Endposition erreicht");
		last_command = command;
		command = stop;
	}
#endif

#if OverCurrent
	if (current_value() == 512 && command != stop) {
		last_command = command;
		command = stop;
		Serial.println("Zu viel Strom, abschalten");
	}
#endif
	if ( (pcb_button.event == auto_switch::Event::Pressed ) || (door_button.event == auto_switch::Event::Pressed) )  {
		state tmp = command;
		if (command == stop) {
			Serial.println("button -> starting");
			if (last_command == boot) {
				command = down;
			}
			if (last_command == up) {
				command = down;
			}
			if (last_command == down) {
				command = up;
			}
			last_command = tmp;
		}
		else {
			Serial.println("button -> stopping");
			last_command = command;
			command = stop;
		}
	}


	if (digitalRead(close_button) == LOW && command == stop) {
		last_command = command;
		command = down;
	}
	if (digitalRead(open_button) == LOW && command == stop) {
		last_command = command;
		command = up;
	}

	if (command == up) {
		motor_start_up();
	}

	if (command == down) {
		motor_start_down();
	}

	if (command == stop) {
		motor_stop();
	}
	if (command == suspend) {
		motor_suspend();
	}
}


void motor_start_down()
{
	analogWrite(motor_pin_1, 0);
	analogWrite(motor_pin_2, 255);
}

void motor_start_up()
{
	analogWrite(motor_pin_2, 0);
	analogWrite(motor_pin_1, 255);
}

void motor_stop()
{
	analogWrite(motor_pin_1, 0);
	analogWrite(motor_pin_2, 0);
}

//TODO write nothing to Motor Pins
void motor_suspend()
{

}

//Returns command needed from Motor
//If Motor is in one End-Position he cant drive further in that so an external
//Switch turns that motor in that direction of, This will return an 0 and is
//interpreted that the Motor has reached its endposition.
int current_value()
{
	int value = 0;
	int value_sum = 0;
	for (int i = 0; i < 5; i++) {
		value = analogRead(current_sensor);
		value = map(value, 0, 1023, -512, 511);
		value_sum += value;
	}
	value_sum = value_sum/5;
	if ((value_sum < 20) && (value_sum > -20))  return 0;
	if ((value_sum > 250) || (value_sum < -250)) return 512;
	return value_sum;
}
