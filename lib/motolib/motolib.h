#ifndef __MOTOLIB_HANDLER_H__
#define __MOTOLIB_HANDLER_H__

#include <Arduino.h>

//Arduino H mostík pro krokový motor L298N Dual H Most DC
//Here’s a table that summarizes the pins and corresponding motor direction. These assumes you are following the same Fritzing diagram above.
/*
motor A motor B
------- -------
IN1	IN2	IN3	IN4	Direction
0	0	0	0	Stop
1	0	1	0	Forward
0	1	0	1	Reverse
1	0	0	1	Left
0	1	1	0	Right
*/

/*
HW connection of motors
DC motor 1 "+" or stepper motor A+
DC motor 1 "-" or stepper motor A-
12V jumper - remove this if using a supply voltage greater than 12V DC. This enables power to the onboard 5V regulator
Connect your motor supply voltage here, maximum of 35V DC. Remove 12V jumper if >12V DC
GND
5V output if 12V jumper in place, ideal for powering your Arduino (etc)
DC motor 1 enable jumper. Leave this in place when using a stepper motor. Connect to PWM output for DC motor speed control.
IN1
IN2
IN3
IN4
DC motor 2 enable jumper. Leave this in place when using a stepper motor. Connect to PWM output for DC motor speed control.
DC motor 2 "+" or stepper motor B+
DC motor 2 "-" or stepper motor B-
*/

class LM298N_bridge {
	public:
		// constructor
		LM298N_bridge(){
			//define arduino pins
			// motor one is "A"
			_en = 0;
			_in1 = 0;
			_in2 = 0;
			//preset internal statuses
			_stop = true; // true> stopped, false> run
			_direction = true;// true> cw, false> ccw
			_speed = 0; // PWM preset from 0 to 255
		};
		//setup arduino, initialize driver
		void setup(int en, int in1, int in2){
			_en = en;
			_in1 = in1;
			_in2 = in2;
			// set all the motor control pins to outputs
			pinMode(_en, OUTPUT);
			pinMode(_in1, OUTPUT);
			pinMode(_in2, OUTPUT);
		};
		// change direction true = cw, false = ccw
		void setDirection(bool dir){
			if(_direction != dir) {
				stop();
				_direction = dir;
				if(_direction) {
					digitalWrite(_in1, HIGH);
					digitalWrite(_in2, LOW);
				} else{
					digitalWrite(_in1, LOW);
					digitalWrite(_in2, HIGH);					
				}
			}
		};
		// preset speed
		void setSpeed(unsigned char speed){
			_speed = speed;
			analogWrite(_en, _speed);
		};
		// start rotating
		void start(){
			setSpeed(_speed);
			if(_direction) {
				digitalWrite(_in1, HIGH);
				digitalWrite(_in2, LOW);
			} else{
				digitalWrite(_in1, LOW);
				digitalWrite(_in2, HIGH);					
			}
			_stop = false;
		};
		// stop rotating both motor
		void stop(){
			// now turn off motor
			analogWrite(_en, 0);
			digitalWrite(_in1, LOW);
			digitalWrite(_in2, LOW); 
			_stop = true;
		};
	private:
		// connect motor controller pins to Arduino digital pins
		// motor one is "A"
		int _en, _in1, _in2;
		
		//motor driver internal statuses
		bool _stop; // true> stopped, false> run
		bool _direction;// true> cw, false> ccw
		unsigned char _speed; // PWM preset from 0 to 255		
};

#endif
