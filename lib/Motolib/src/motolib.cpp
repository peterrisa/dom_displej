#include <motolib.h>

// constructor
LM298N_bridge::LM298N_bridge()
{
    //define arduino pins
    // motor one is "A"
    _en = 0;
    _in1 = 0;
    _in2 = 0;
    //preset internal statuses
    _stop = true;      // true> stopped, false> run
    _direction = true; // true> cw, false> ccw
    _speed = 0;        // PWM preset from 0 to 255
};
//setup arduino, initialize driver
void LM298N_bridge::setup(int en, int in1, int in2)
{
    _en = en;
    _in1 = in1;
    _in2 = in2;
    // set all the motor control pins to outputs
    pinMode(_en, OUTPUT);
    pinMode(_in1, OUTPUT);
    pinMode(_in2, OUTPUT);
};
// change direction true = cw, false = ccw
void LM298N_bridge::setDirection(bool dir)
{
    if (_direction != dir)
    {
        stop();
        _direction = dir;
        if (_direction)
        {
            digitalWrite(_in1, HIGH);
            digitalWrite(_in2, LOW);
        }
        else
        {
            digitalWrite(_in1, LOW);
            digitalWrite(_in2, HIGH);
        }
    }
};
// preset speed
void LM298N_bridge::setSpeed(unsigned char speed)
{
    _speed = speed;
    analogWrite(_en, _speed);
};
// start rotating
void LM298N_bridge::start()
{
    setSpeed(_speed);
    if (_direction)
    {
        digitalWrite(_in1, HIGH);
        digitalWrite(_in2, LOW);
    }
    else
    {
        digitalWrite(_in1, LOW);
        digitalWrite(_in2, HIGH);
    }
    _stop = false;
};
// stop rotating both motor
void LM298N_bridge::stop()
{
    // now turn off motor
    analogWrite(_en, 0);
    digitalWrite(_in1, LOW);
    digitalWrite(_in2, LOW);
    _stop = true;
};
