//Function Prototypes
int current_value();
void motor_start_down();
void motor_start_up();
void motor_stop();
void motor_suspend();


class auto_switch
{
	public:
		auto_switch(int pin, int on_status);
		uint8_t pressed();
	private:
		unsigned long last_Debounce_time;
		unsigned long debounce_delay = 100;
		int current_button_state;
		int button_state;
		int last_button_state;
		int _pin;
		int _on_status;
};

auto_switch::auto_switch(int pin, int on_status)
{
	_pin = pin;
	_on_status = on_status;
}

uint8_t auto_switch::pressed()
{
	current_button_state = digitalRead(_pin);

	if (current_button_state != last_button_state) {
		last_Debounce_time = millis();
	}

	if ((millis() - last_Debounce_time) > debounce_delay) {
		if (current_button_state != button_state) {
			button_state = current_button_state;
			if (button_state == _on_status) {
				return 1;
			}
		}
	}
	last_button_state = current_button_state;
	return 0;
}
