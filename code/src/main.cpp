#include <Arduino.h>
#include <FastLED.h>
#include "motor.h"
#include "button.h"
#include "main.h"

#define Endswitch 1
#define OverCurrent 0
#define ZeroCurrent 1
#define Serial_Debug 1

//LED's
#define NUM_LEDS 40
CRGB leds[NUM_LEDS];
#define COLOR_ORDER GBR
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
	boot, 				//Boot State does nothing
	stop, 				//Stop State -> MotorPins write both 0
	up, 				//Up State -> Motor opens door
	down, 				//Down State -> Motor closes door
	suspend, 			//Suspend State -> Motor Pins write nothing
	manual_up, 			//manual_up -> motor opens while button ob PCB is pressed, without security
	manual_down, 		//manual_down -> motor closes while button on PCB is pressed, without security
};

state command = stop;
state last_command = boot;

button pcb_button(signal_button, LOW, 100);
button door_button(door_signal, HIGH, 100);
button security_button_1(endswitch_1, HIGH, 1);
button security_button_2(endswitch_2, HIGH, 1);
button manual_down_button(close_button, LOW, 1);
button manual_up_button(open_button, LOW, 1);

motor motor(motor_pin_1, motor_pin_2);

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
	motor.motor_stop();

	FastLED.addLeds<WS2812B, ext_LED, COLOR_ORDER>(leds, NUM_LEDS);

	delay(100);
}

void loop()
{
	//Button loop
	unsigned long current_millis = millis();
	pcb_button.loop(current_millis);
	door_button.loop(current_millis);
	security_button_1.loop(current_millis);
	security_button_2.loop(current_millis);
	manual_down_button.loop(current_millis);
	manual_up_button.loop(current_millis);

	//Security Switches to detect an obstacle. Switches open if Motor runs into obstacle
	//Security Button 1 = up switch
	//Security Button 2 = down switch
	if (security_button_1.event == button::Event::Pressed) {
		Serial.println("Endschalter Ausgelöst 1");
		if (command == up) {
			Serial.println("Beim Auf-Fahren : Hinderniss erkannt, umdrehen");
			last_command = command;
			command = down;
		}
	}
	if (security_button_2.event == button::Event::Pressed) {
		Serial.println("Endschalter Ausgelöst 2");
		if (command == down) {
			Serial.println("Beim Zu-Fahren : Hinderniss erkannt, umdrehen");
			last_command = command;
			command = up;
		}
	}
	if ((security_button_1.event == button::Event::Released) ||
			(security_button_2.event == button::Event::Released)){
			if (command == up) {
			last_command = down;
			}
			if (command == down) {
				last_command = up;
			}
		command = stop;
		Serial.println("Endschalter frei");
	}

	//Motor Currents reads ~0 Ampere if Motor is cut of within its enpositons,
	//Hardeware switch turns Motor of and it is detected with an reading around
	//0 from current_value()
	if (current_value() == 0 && ((command == up) || (command == down))) {
		Serial.println("Endposition erreicht");
		last_command = command;
		command = stop;
	}

	//OverCurrent protection. This Function detects an obstacle that wont get
	//detectet with the Security Switches but the motor will drain to much
	//Current. Will get implemented if Door is connected to Motor to determain
	//the correct vale to turn off
#if OverCurrent
	if (current_value() == 512 && command != stop) {
		last_command = command;
		command = stop;
		Serial.println("Zu viel Strom, abschalten");
	}
#endif

	if ( (pcb_button.event == button::Event::Pressed ) || (door_button.event == button::Event::Pressed) )  {
		state tmp = command;
		if (command == suspend) {
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

	if ((manual_down_button.event == button::Event::Pressed) && (command == suspend)){
		command = manual_down;
	}
	if ((manual_down_button.event == button::Event::Released) && (command == manual_down)){
		command = stop;
	}
	if ((manual_up_button.event == button::Event::Pressed) && (command == suspend)) {
		command = manual_up;
	}
	if ((manual_up_button.event == button::Event::Released) && (command == manual_up)){
		command = stop;
	}

	if (command == up) {
		motor.motor_up();
		fill_solid(leds, NUM_LEDS, CRGB::Red);
		FastLED.show();
	}

	if (command == down) {
		motor.motor_down();
		fill_solid(leds, NUM_LEDS, CRGB::Red);
		FastLED.show();
	}

	if (command == stop) {
		motor.motor_stop();
		fill_solid(leds, NUM_LEDS, CRGB::Green);
		FastLED.show();
		command = suspend;
	}
	if (command == manual_down) {
		motor.motor_down();
	}
	if (command == manual_up) {
		motor.motor_up();
	}
	//TODO
	if (command == suspend) {
		motor.motor_suspend();
		Serial.println("suspend");
	}
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
