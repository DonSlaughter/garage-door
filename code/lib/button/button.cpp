#include <Arduino.h>
#include "button.h"

button::button(int pin, int on_status)
{
	event = button::Event::None;

	last_Debounce_time = 0;
	debounce_delay = 100;
	current_button_state = 0;
	button_state = 0;
	last_button_state = 0;

	_pin = pin;
	_on_status = on_status;
}

void button::loop(unsigned long millis)
{
	event = button::Event::None;

	current_button_state = digitalRead(_pin);

	if (current_button_state != last_button_state) {
		last_Debounce_time = millis;
	}

	if ((millis - last_Debounce_time) > debounce_delay) {
		if (current_button_state != button_state) {
			button_state = current_button_state;
			if (button_state == _on_status) {
				event = button::Event::Pressed;
			} else {
				event = button::Event::Released;
			}
		}
	}
	last_button_state = current_button_state;
}
