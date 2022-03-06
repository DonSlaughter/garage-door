#include <Arduino.h>
#include "main.h"

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

int button_state_signal;
int last_button_state_signal;
int reading_signal_button;
int last_button_state = HIGH;

unsigned long last_debounce_time;
unsigned long debounce_delay = 50;

uint8_t status = 0;

//Function Prototypes
uint8_t open_door();
uint8_t close_door();
void force_open_door();
void force_close_door();
void stop_door();
int current_value();

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
	pinMode(endswitch_1, INPUT);
	pinMode(endswitch_2, INPUT);
	pinMode(current_sensor, INPUT);
}

void loop()
{

	while (digitalRead(close_button)== LOW){
		force_close_door();
	}
	while (digitalRead(open_button) == LOW){
		force_open_door();
	}

	reading_signal_button = digitalRead(signal_button);
	if (reading_signal_button != last_button_state_signal) {
		last_debounce_time = millis();
	}
	if ((millis() - last_debounce_time) > debounce_delay) {
		if (reading_signal_button !=  button_state_signal) {
			button_state_signal = reading_signal_button;
			if (button_state_signal == LOW) {
				Serial.println("pressed");
				switch (status) {
					case (0):
						stop_door();
						break;
					case (1):
						open_door();
						break;
					case (2):
						stop_door();
						break;
					case (3):
						close_door();
						break;
				}
			}
		}
	}
	last_button_state_signal = reading_signal_button;
	//Serial.println(current_value());
	stop_door();
}

uint8_t open_door()
{
	while (current_value() != 0 && current_value() != 512){
		analogWrite(motor_pin_1, 255);
		analogWrite(motor_pin_2, 0);
		if ((digitalRead(endswitch_1) == LOW) || (digitalRead(endswitch_2) == LOW)) {
			stop_door();
			break;
		}
		if (digitalRead(door_signal == HIGH)) {
			stop_door();
			break;
		}
		if (digitalRead(signal_button) == LOW) {
			stop_door();
			break;
		}
	}
	status += 1;
	if (status == 3) status =0;
	return status;
}

uint8_t close_door()
{
	while (current_value() != 0 && current_value() != 512){
		analogWrite(motor_pin_1, 0);
		analogWrite(motor_pin_2, 255);
		if ((digitalRead(endswitch_1) == LOW) || (digitalRead(endswitch_2) == LOW)) {
			stop_door();
			break;
		}
		if (digitalRead(door_signal == HIGH)) {
			stop_door();
			break;
		}
		if (digitalRead(signal_button) == LOW) {
			stop_door();
			break;
		}
	}
	status += 1;
	if (status == 3) status =0;
	return status;
}


void force_open_door()
{
	//Serial.println("opening door");
	analogWrite(motor_pin_1, 255);
	analogWrite(motor_pin_2, 0);
	Serial.println(current_value());
}

void force_close_door()
{
	//Serial.println("closing door");
	analogWrite(motor_pin_1, 0);
	analogWrite(motor_pin_2, 255);
	Serial.println(current_value());
}

void stop_door()
{
	analogWrite(motor_pin_1, 0);
	analogWrite(motor_pin_2, 0);
}

int current_value()
{
	int value;
	value = analogRead(current_sensor);
	value = map(value, 0, 1023, -512, 511);
	if (value < 20 || value > -20 ) return 0;
	if (value > 150 || value < -150) return 512;
	return value;
}
