#include <Arduino.h>
#include "main.h"

unsigned long current_millis;
unsigned long start_time;
unsigned long door_time;

int door_state;

void setup()
{
	pinMode(motor_pin_1, OUTPUT);
	pinMode(motor_pin_2, OUTPUT);
	pinMode(speed, OUTPUT);
	pinMode(door_signal_external, INPUT);
	pinMode(end_switch_01, INPUT);
	pinMode(end_switch_02, INPUT);
	pinMode(door_signal_button, INPUT);
	pinMode(red_led, OUTPUT);
	pinMode(green_led, OUTPUT);
	door_state = 0;
	Serial.begin(9600);
	Serial.print("Endschalter 1 status: ");
	Serial.println(digitalRead(end_switch_01));
	Serial.print("Endschalter 2 status: ");
	Serial.println(digitalRead(end_switch_02));
	delay(1000);
}

void loop()
{
	Serial.print("Toor Knopf: ");
	Serial.println(digitalRead(door_signal_button));
	if (digitalRead(door_signal_button) == 1 || digitalRead(door_signal_external) == 1) {
		digitalWrite(green_led, HIGH);
		if (door_state == 0) {
			door_state = 1;
			close(30000);
		}
		else {
			door_state = 0;
			open(30000);
		}
	}
	stop();
	digitalWrite(green_led,LOW);
	digitalWrite(red_led, LOW);
}

void close(unsigned long door_time)
{
	digitalWrite(green_led, HIGH);
	start_time = millis();
	Serial.println("tor bewegung");
	while (millis() - start_time <= door_time) {
		digitalWrite(motor_pin_1, LOW);
		digitalWrite(motor_pin_2, HIGH);
		analogWrite(speed, 255);
		if (digitalRead(end_switch_01) == LOW || digitalRead(end_switch_02) == LOW) {
			stop();
			delay(100);
			break;
		}
	}
}

void open(unsigned long door_time)
{
	digitalWrite(green_led, HIGH);
	start_time = millis();
	Serial.println("tor bewegung");
	while (millis() - start_time <= door_time) {
		digitalWrite(motor_pin_1, HIGH);
		digitalWrite(motor_pin_2, LOW);
		analogWrite(speed, 255);
		if (digitalRead(end_switch_01) == LOW || digitalRead(end_switch_02) == LOW) {
			stop();
			delay(100);
			break;
		}
	}
}

void stop()
{
	digitalWrite(motor_pin_1, LOW);
	digitalWrite(motor_pin_2, LOW);
	analogWrite(speed, 0);
}
