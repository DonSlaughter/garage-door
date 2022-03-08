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

const int debounce_delay = 100;

int last_steady_state = LOW;
int last_flickerabele_state = LOW;
int current_state;
unsigned long last_debounce_time = 0;

enum state{
	boot,
	stopped,
	up,
	down,
};

state current = stopped;
state last = boot;

//Function Prototypes
uint8_t board_signal();
int current_value();

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
	Serial.print("Tür board_signal : ");
	Serial.println(digitalRead(door_signal));
	Serial.print("Endschalter : ");
	Serial.print(digitalRead(endswitch_1));
	Serial.print("   ");
	Serial.println(digitalRead(endswitch_2));
#endif
}

void loop()
{
	if (board_signal() == 1 && current == stopped) {
		state tmp = current;
			if (last == boot) {
				//Serial.println("Now down");
				current = down;
			}
			if (last == up) {
				//Serial.println("Now down");
				current = down;
			}
			if (last == down) {
				//Serial.println("Now up");
				current = up;
			}
			last = tmp;
	}
	if (board_signal() == 1 && current != stopped) {
			last = current;
			current = stopped;
	}

	if (current == up) {
		//Serial.println("Öffnen");
		analogWrite(motor_pin_1, 255);
		analogWrite(motor_pin_2, 0);
	}
	if (current == down) {
		//Serial.println("Schließen");
		analogWrite(motor_pin_1, 0);
		analogWrite(motor_pin_2, 255);
	}
	if (current == stopped) {
		analogWrite(motor_pin_1, 0);
		analogWrite(motor_pin_2, 0);
	}

#if Endswitch
	if (((digitalRead(endswitch_1) == HIGH) || (digitalRead(endswitch_2) == HIGH)) && current != stopped) {
		last = current;
		current = stopped;
		Serial.println("Endschalter Ausgelöst");
	}
#endif

#if ZeroCurrent
	if (current_value() == 0 && current != stopped) {
		last = current;
		current = stopped;
	}
#endif

#if OverCurrent
	if (current_value() == 512 && current != stopped) {
		last = current;
		current = stopped;
		Serial.println("Zu viel Strom, abschalten");
	}
#endif
	while (digitalRead(close_button) == LOW && current == stopped) {
		analogWrite(motor_pin_1, 0);
		analogWrite(motor_pin_2, 255);
	}
	while (digitalRead(open_button) == LOW && current == stopped) {
		analogWrite(motor_pin_1, 255);
		analogWrite(motor_pin_2, 0);
	}
}

uint8_t board_signal()
{
	//Serial.println("kopf");
	uint8_t switch_signal;
	current_state = digitalRead(signal_button);
	if (current_state != last_flickerabele_state) {
		last_debounce_time = millis();
		last_flickerabele_state = current_state;
	}
	if (last_steady_state == HIGH && current_state ==LOW) switch_signal = 1;
	else if (last_steady_state == LOW && current_state == HIGH) switch_signal = 0;
	else switch_signal =0;
	last_steady_state = current_state;
	return switch_signal;
}

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
