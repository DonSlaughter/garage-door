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
	stopped,
	up,
	down,
};

state current = stopped;
state last = boot;

//Function Prototypes
uint8_t automatic_button();
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
	if (automatic_button() == 1)  {
		state tmp = current;
		if (current == stopped) {
			Serial.println("button -> starting");
			if (last == boot) {
				current = down;
			}
			if (last == up) {
				current = down;
			}
			if (last == down) {
				current = up;
			}
			last = tmp;
		}
		else {
		//if (current != stopped) {
			Serial.println("button -> stopping");
			last = current;
			current = stopped;
		}
	}

	if (current == up) {
#if Serial_Debug
		//Serial.println("Öffnen");
#endif
		analogWrite(motor_pin_1, 255);
		analogWrite(motor_pin_2, 0);
	}
	if (current == down) {
#if Serial_Debug
		//Serial.println("Schließen");
#endif
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
		Serial.println("Endposition erreicht");
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

	// Force Close or Open, only reachable with Buttons on PCB
	while (digitalRead(close_button) == LOW && current == stopped) {
		analogWrite(motor_pin_1, 0);
		analogWrite(motor_pin_2, 255);
	}
	while (digitalRead(open_button) == LOW && current == stopped) {
		analogWrite(motor_pin_1, 255);
		analogWrite(motor_pin_2, 0);
	}
}
unsigned long last_Debounce_time;
unsigned long debounce_delay = 100;
int button_state;
int last_button_state = HIGH;

uint8_t automatic_button()
{
	int current_button_state = digitalRead(signal_button);

	if (current_button_state != last_button_state) {
		last_Debounce_time = millis();
	}

	if ((millis() - last_Debounce_time) > debounce_delay) {
		if (current_button_state != button_state) {
			button_state = current_button_state;
			if (button_state == LOW){
				return 1;
			}
		}
	}
	last_button_state = current_button_state;
	return 0;
}



//Returns Current needed from Motor
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
