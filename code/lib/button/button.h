#ifndef button_h
#define button_h

class button
{
	public:
		button(int pin, int on_status);
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

#endif
