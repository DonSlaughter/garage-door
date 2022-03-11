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
		void setup();
		void loop(unsigned long millis);
		uint8_t event;
		enum Event {
			None,
			Pressed,
			Released
		};
	private:
		unsigned long last_Debounce_time;
		unsigned long debounce_delay;
		int current_button_state;
		int button_state;
		int last_button_state;
		int _pin;
		int _on_status;
};

auto_switch::auto_switch(int pin, int on_status)
{
	event = auto_switch::Event::None;

	last_Debounce_time = 0;
	debounce_delay = 100;
	current_button_state = 0;
	button_state = 0;
	last_button_state = 0;

	_pin = pin;
	_on_status = on_status;
}

void auto_switch::setup() {
	pinMode(_pin, INPUT);
}

void auto_switch::loop(unsigned long millis)
{
	event = auto_switch::Event::None;

	current_button_state = digitalRead(_pin);

	if (current_button_state != last_button_state) {
		last_Debounce_time = millis;
	}

	if ((millis - last_Debounce_time) > debounce_delay) {
		if (current_button_state != button_state) {
			button_state = current_button_state;
			if (button_state == _on_status) {
				event = auto_switch::Event::Pressed;
			} else {
				event = auto_switch::Event::Released;
			}
		}
	}
	last_button_state = current_button_state;
}
