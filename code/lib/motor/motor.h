#ifndef motor_h
#define motor_h

#include <Arduino.h>
class motor
{
	public:
		motor(const int motor_pin_1, const int motor_pin_2);
		enum state {
			boot,
			stop,
			up,
			down,
			suspend,
		};
		void motor_down(void);
		void motor_up(void);
		void motor_stop(void);
		void motor_suspend(void);
private:
		int _pin1;
		int _pin2;
};

#endif
