[1mdiff --git a/code/src/main.cpp b/code/src/main.cpp[m
[1mindex 7be3c61..214c81a 100644[m
[1m--- a/code/src/main.cpp[m
[1m+++ b/code/src/main.cpp[m
[36m@@ -30,6 +30,10 @@[m [menum state{[m
 state command = stop;[m
 state last_command = boot;[m
 [m
[32m+[m[32m//Class for Automatic Button on PCB an Exteral Door signal (key or switch in[m
[32m+[m[32m//Garage)[m
[32m+[m[32mauto_switch pcb_button(signal_button, LOW);[m
[32m+[m[32mauto_switch door_button(door_signal, HIGH);[m
 [m
 void setup()[m
 {[m
[36m@@ -89,7 +93,7 @@[m [mvoid loop()[m
 		Serial.println("Zu viel Strom, abschalten");[m
 	}[m
 #endif[m
[31m-	if (automatic_button() == 1)  {[m
[32m+[m	[32mif ((pcb_button.pressed() == 1 ) || door_button.pressed() ==1 )  {[m
 		state tmp = command;[m
 		if (command == stop) {[m
 			Serial.println("button -> starting");[m
[36m@@ -112,8 +116,6 @@[m [mvoid loop()[m
 	}[m
 [m
 [m
[31m-[m
[31m-	// Force Close or Open, only reachable with Buttons on PCB[m
 	if (digitalRead(close_button) == LOW && command == stop) {[m
 		last_command = command;[m
 		command = down;[m
[36m@@ -133,37 +135,12 @@[m [mvoid loop()[m
 [m
 	if (command == stop) {[m
 		motor_stop();[m
[31m-		//command = suspend;[m
 	}[m
 	if (command == suspend) {[m
 		motor_suspend();[m
 	}[m
[31m-[m
 }[m
[31m-unsigned long last_Debounce_time;[m
[31m-unsigned long debounce_delay = 100;[m
[31m-int button_state;[m
[31m-int last_button_state = HIGH;[m
[31m-[m
[31m-uint8_t automatic_button()[m
[31m-{[m
[31m-	int current_button_state = digitalRead(signal_button);[m
 [m
[31m-	if (current_button_state != last_button_state) {[m
[31m-		last_Debounce_time = millis();[m
[31m-	}[m
[31m-[m
[31m-	if ((millis() - last_Debounce_time) > debounce_delay) {[m
[31m-		if (current_button_state != button_state) {[m
[31m-			button_state = current_button_state;[m
[31m-			if (button_state == LOW){[m
[31m-				return 1;[m
[31m-			}[m
[31m-		}[m
[31m-	}[m
[31m-	last_button_state = current_button_state;[m
[31m-	return 0;[m
[31m-}[m
 [m
 void motor_start_down()[m
 {[m
[36m@@ -177,11 +154,13 @@[m [mvoid motor_start_up()[m
 	analogWrite(motor_pin_2, 0);[m
 }[m
 [m
[31m-void motor_stop(){[m
[32m+[m[32mvoid motor_stop()[m
[32m+[m[32m{[m
 	analogWrite(motor_pin_1, 0);[m
 	analogWrite(motor_pin_2, 0);[m
 }[m
 [m
[32m+[m[32m//TODO write nothing to Motor Pins[m
 void motor_suspend()[m
 {[m
 [m
[1mdiff --git a/code/src/main.h b/code/src/main.h[m
[1mindex 9f8f851..19fbd79 100644[m
[1m--- a/code/src/main.h[m
[1m+++ b/code/src/main.h[m
[36m@@ -1,7 +1,48 @@[m
 //Function Prototypes[m
[31m-uint8_t automatic_button();[m
 int current_value();[m
 void motor_start_down();[m
 void motor_start_up();[m
 void motor_stop();[m
 void motor_suspend();[m
[32m+[m
[32m+[m
[32m+[m[32mclass auto_switch[m
[32m+[m[32m{[m
[32m+[m	[32mpublic:[m
[32m+[m		[32mauto_switch(int pin, int on_status);[m
[32m+[m		[32muint8_t pressed();[m
[32m+[m	[32mprivate:[m
[32m+[m		[32munsigned long last_Debounce_time;[m
[32m+[m		[32munsigned long debounce_delay = 100;[m
[32m+[m		[32mint current_button_state;[m
[32m+[m		[32mint button_state;[m
[32m+[m		[32mint last_button_state;[m
[32m+[m		[32mint _pin;[m
[32m+[m		[32mint _on_status;[m
[32m+[m[32m};[m
[32m+[m
[32m+[m[32mauto_switch::auto_switch(int pin, int on_status)[m
[32m+[m[32m{[m
[32m+[m	[32m_pin = pin;[m
[32m+[m	[32m_on_status = on_status;[m
[32m+[m[32m}[m
[32m+[m
[32m+[m[32muint8_t auto_switch::pressed()[m
[32m+[m[32m{[m
[32m+[m	[32mcurrent_button_state = digitalRead(_pin);[m
[32m+[m
[32m+[m	[32mif (current_button_state != last_button_state) {[m
[32m+[m		[32mlast_Debounce_time = millis();[m
[32m+[m	[32m}[m
[32m+[m
[32m+[m	[32mif ((millis() - last_Debounce_time) > debounce_delay) {[m
[32m+[m		[32mif (current_button_state != button_state) {[m
[32m+[m			[32mbutton_state = current_button_state;[m
[32m+[m			[32mif (button_state == _on_status) {[m
[32m+[m				[32mreturn 1;[m
[32m+[m			[32m}[m
[32m+[m		[32m}[m
[32m+[m	[32m}[m
[32m+[m	[32mlast_button_state = current_button_state;[m
[32m+[m	[32mreturn 0;[m
[32m+[m[32m}[m
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-01-18_185934.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-01-18_185934.zip[m
[1mdeleted file mode 100644[m
[1mindex ddaa7a3..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-01-18_185934.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-01-18_190934.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-01-18_190934.zip[m
[1mdeleted file mode 100644[m
[1mindex 9ecb536..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-01-18_190934.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-01-18_191934.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-01-18_191934.zip[m
[1mdeleted file mode 100644[m
[1mindex b109e86..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-01-18_191934.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-01-19_100453.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-01-19_100453.zip[m
[1mdeleted file mode 100644[m
[1mindex 1138d4d..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-01-19_100453.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-01-19_120236.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-01-19_120236.zip[m
[1mdeleted file mode 100644[m
[1mindex 1138d4d..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-01-19_120236.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-01-19_212538.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-01-19_212538.zip[m
[1mdeleted file mode 100644[m
[1mindex 565b78a..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-01-19_212538.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-01-23_170057.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-01-23_170057.zip[m
[1mdeleted file mode 100644[m
[1mindex 565b78a..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-01-23_170057.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-01-24_065057.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-01-24_065057.zip[m
[1mdeleted file mode 100644[m
[1mindex 565b78a..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-01-24_065057.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-01-25_144947.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-01-25_144947.zip[m
[1mdeleted file mode 100644[m
[1mindex 565b78a..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-01-25_144947.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-01_140820.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-02-01_140820.zip[m
[1mdeleted file mode 100644[m
[1mindex 9048ce1..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-01_140820.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-01_193400.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-02-01_193400.zip[m
[1mdeleted file mode 100644[m
[1mindex f02fd80..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-01_193400.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-05_185033.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-02-05_185033.zip[m
[1mdeleted file mode 100644[m
[1mindex f02fd80..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-05_185033.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-10_174358.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-02-10_174358.zip[m
[1mdeleted file mode 100644[m
[1mindex f26a9c2..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-10_174358.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-10_175403.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-02-10_175403.zip[m
[1mdeleted file mode 100644[m
[1mindex 3ec060b..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-10_175403.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-10_180403.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-02-10_180403.zip[m
[1mdeleted file mode 100644[m
[1mindex 8bf19d6..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-10_180403.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-10_181403.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-02-10_181403.zip[m
[1mdeleted file mode 100644[m
[1mindex 1f75338..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-10_181403.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-10_182403.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-02-10_182403.zip[m
[1mdeleted file mode 100644[m
[1mindex e12d34f..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-10_182403.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-23_215001.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-02-23_215001.zip[m
[1mdeleted file mode 100644[m
[1mindex c02a9f4..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-23_215001.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-23_220832.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-02-23_220832.zip[m
[1mdeleted file mode 100644[m
[1mindex dfa911a..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-23_220832.zip and /dev/null differ
[1mdiff --git a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-24_192022.zip b/pcb/garagen_pcb-backups/garagen_pcb-2022-02-24_192022.zip[m
[1mdeleted file mode 100644[m
[1mindex dfa911a..0000000[m
Binary files a/pcb/garagen_pcb-backups/garagen_pcb-2022-02-24_192022.zip and /dev/null differ
