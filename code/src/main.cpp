//#include <Arduino.h>
#include <FastLED.h>
#include "motor.h"
#include "button.h"
#include "main.h"

#define OverCurrent 0

//LED's
#define NUM_LEDS 22
CRGB leds[NUM_LEDS];
#define COLOR_ORDER GRB

CRGB color = CRGB::White;

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

unsigned long time_now = 0;
unsigned long period = 2000;

button pcb_button(signal_button, LOW, 100);
button door_button(door_signal, HIGH, 100);
button security_button_1(endswitch_1, HIGH, 1);
button security_button_2(endswitch_2, HIGH, 1);
button manual_down_button(close_button, LOW, 1);
button manual_up_button(open_button, LOW, 1);

motor motor(motor_pin_1, motor_pin_2);

void setup()
{
	Serial.begin(9600);
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
			color = CRGB::Red;
			last_command = command;
			command = down;
		}
	}
	if (security_button_2.event == button::Event::Pressed) {
		Serial.println("Endschalter Ausgelöst 2");
		if (command == down) {
			color = CRGB::Red;
			Serial.println("Beim Zu-Fahren : Hinderniss erkannt, umdrehen");
			last_command = command;
			command = up;
		}
	}
	if ((security_button_1.event == button::Event::Released) ||
			(security_button_2.event == button::Event::Released)){
			if (command == up) {
				last_command = down;
				color = CRGB::Red;
			}
			if (command == down) {
				last_command = up;
				color = CRGB::Red;
			}
		command = stop;
		Serial.println("Endschalter frei");
	}

	//Motor Currents reads ~0 Ampere if Motor is cut of within its enpositons,
	//Hardeware switch turns Motor of and it is detected with an reading around
	//0 from current_value()
	if (current_value() == 0) {
		if (command == up) {
			Serial.println("Tor Offen ");
			color = CRGB::Green;
			last_command = command;
			command = open_position;
		}
		if (command == down){
			Serial.println("Tor Geschlossen");
			color = CRGB::Green;
			last_command = command;
			command = closed_position;
		}
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
				color = CRGB::Orange;
			}
			if (last_command == up) {
				color = CRGB::Orange;
				command = down;
			}
			if (last_command == down) {
				color = CRGB::Orange;
				command = up;
			}
			last_command = tmp;
		}
		else if (command == open_position) {
			Serial.println("button -> starting to close");
			color = CRGB::Orange;
			last_command = command;
			command = down;
		}
		else if (command == closed_position) {
			Serial.println("button -> starting to open");
			color = CRGB::Orange;
			last_command = command;
			command = up;
		}
		else {
			Serial.println("button -> stopping");
			color = CRGB::Blue;
			last_command = command;
			command = stop;
		}
	}

	// Manual Button Events
	if ((manual_down_button.event == button::Event::Pressed) && (command == suspend)){
		color = CRGB::Blue;
		command = manual_down;
	}
	if ((manual_down_button.event == button::Event::Released) && (command == manual_down)){
		color = CRGB::Green;
		command = stop;
	}
	if ((manual_up_button.event == button::Event::Pressed) && (command == suspend)) {
		color = CRGB::Blue;
		command = manual_up;
	}
	if ((manual_up_button.event == button::Event::Released) && (command == manual_up)){
		color = CRGB::Green;
		command = stop;
	}

	// Command Events
	if (command == up) {
		motor.motor_up();
		fill_solid(leds, NUM_LEDS, color);
		FastLED.show();
	}
	if (command == down) {
		motor.motor_down();
		fill_solid(leds, NUM_LEDS, color);
		FastLED.show();
	}
	if (command == stop) {
		motor.motor_stop();
		fill_solid(leds, NUM_LEDS, color);
		FastLED.show();
		command = suspend;
	}
	if (command == manual_down) {
		fill_solid(leds, NUM_LEDS, color);
		FastLED.show();
		motor.motor_down();
	}
	if (command == manual_up) {
		fill_solid(leds, NUM_LEDS, color);
		FastLED.show();
		motor.motor_up();
	}
	if (command == suspend) {
		set_leds();
		motor.motor_suspend();
	}
	if (command == open_position) {
		fill_solid(leds, NUM_LEDS, color);
		FastLED.show();
		command = stop;
	}
	if (command == closed_position) {
		fill_solid(leds, NUM_LEDS, color);
		FastLED.show();
		command = stop;
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

//Sets Leds to color for the given length of time, then turns the leds off.
void set_leds()
{
	if (millis() - time_now >= period){
		time_now = millis();
		fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
		FastLED.show();
	}
}
