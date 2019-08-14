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
			_enA = 0;
			_in1 = 0;
			_in2 = 0;
			// motor two is "B"
			_enB = 0;
			_in3 = 0;
			_in4 = 0;
			//preset internal statuses
			_stopA = true; // true> stopped, false> run
			_stopB = true; // true> stopped, false> run
			_directionA = true;// true> cw, false> ccw
			_directionB = true;// true> cw, false> ccw
			_speedA = 0; // PWM preset from 0 to 255
			_speedB = 0; // PWM preset from 0 to 255
		};
		//setup arduino, initialize driver
		void setupA(int enA, int in1, int in2){
			_enA = enA;
			_in1 = in1;
			_in2 = in2;
			// set all the motor control pins to outputs
			pinMode(_enA, OUTPUT);
			pinMode(_in1, OUTPUT);
			pinMode(_in2, OUTPUT);
		};
		void setupB(int enB, int in3, int in4){
			_enB = enB;
			_in3 = in3;
			_in4 = in4;		
			// set all the motor control pins to outputs
			pinMode(_enB, OUTPUT);
			pinMode(_in3, OUTPUT);
			pinMode(_in4, OUTPUT);
		};
		// change direction true = cw, false = ccw
		void setDirection(bool dir){
			setDirectionA(dir);
			setDirectionB(dir);
		};
		void setDirectionA(bool dir){
			if(_directionA != dir) {
				stopA();
				_directionA = dir;
				if(_directionA) {
					digitalWrite(_in1, HIGH);
					digitalWrite(_in2, LOW);
				} else{
					digitalWrite(_in1, LOW);
					digitalWrite(_in2, HIGH);					
				}
			}
		};
		void setDirectionB(bool dir){
			if(_directionB != dir) {
				stopB();
				_directionB = dir;
				if(_directionB) {
					digitalWrite(_in3, HIGH);
					digitalWrite(_in4, LOW);
				} else{
					digitalWrite(_in3, LOW);
					digitalWrite(_in4, HIGH);					
				}
			}
		};
		// preset speed
		void setSpeed(unsigned char speed){
			setSpeedA(speed);
			setSpeedB(speed);
		};
		void setSpeedA(unsigned char speed){
			_speedA = speed;
			analogWrite(_enA, _speedA);
		};
		void setSpeedB(unsigned char speed){
			_speedA = speed;
			analogWrite(_enB, _speedA);
		};
		// start rotating
		void start(){
			startA();
			startB();
		};
		void startA(){
			setSpeedA(_speedA);
			if(_directionA) {
				digitalWrite(_in1, HIGH);
				digitalWrite(_in2, LOW);
			} else{
				digitalWrite(_in1, LOW);
				digitalWrite(_in2, HIGH);					
			}
			_stopA = false;
		};
		void startB(){			
			setSpeedB(_speedB);
			if(_directionA) {
				digitalWrite(_in3, HIGH);
				digitalWrite(_in4, LOW);
			} else{
				digitalWrite(_in3, LOW);
				digitalWrite(_in4, HIGH);					
			}
			_stopB = false;
		};
		// stop rotating both motors
		void stop(){
			stopA();
			stopB();
		};
		void stopA(){
			// now turn off motor A
			analogWrite(_enA, 0);
			digitalWrite(_in1, LOW);
			digitalWrite(_in2, LOW); 
			_stopA = true;
		};
		void stopB(){
			// now turn off motor B
			analogWrite(_enB, 0);
			digitalWrite(_in3, LOW);
			digitalWrite(_in4, LOW);
			_stopB = true;
		};
	private:
		// connect motor controller pins to Arduino digital pins
		// motor one is "A"
		int _enA, _in1, _in2;
		// motor two is "B"
		int _enB, _in3, _in4;
		
		//motor driver internal statuses
		bool _stopA; // true> stopped, false> run
		bool _directionA;// true> cw, false> ccw
		unsigned char _speedA; // PWM preset from 0 to 255
		bool _stopB; // true> stopped, false> run
		bool _directionB;// true> cw, false> ccw
		unsigned char _speedB; // PWM preset from 0 to 255		
};

#endif
