#include <Arduino.h>

//Different States in which this control unit can be in
enum state{
	boot, 				//Boot State does nothing
	stop, 				//Stop State -> MotorPins write both 0
	up, 				//Up State -> Motor opens door
	down, 				//Down State -> Motor closes door
	suspend, 			//Suspend State -> Motor Pins write nothing
	manual_up, 			//manual_up -> motor opens while button ob PCB is pressed, without security
	manual_down, 		//manual_down -> motor closes while button on PCB is pressed, without security
	open_position, 		//fully open position
	closed_position, 	//fully closed position
};

state command = stop;
state last_command = boot;

//Function Prototypes
int current_value(void);
void set_leds(void);
