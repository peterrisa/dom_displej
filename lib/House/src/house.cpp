#include <house.h>

//ovladanie zaluzii
void Louver::setup(int en, int in1, int in2)
{
    motor.setup(en, in1, in2);
    //vykonaj prvý pohyb so záclonami aby si definoval ich polohu
    doLouverOpen();
}

void Louver::doLouverOpen()
{
    if (louverPosition == true)
        return;
    motor.setSpeed(MaxSpeed);
    motor.setDirection(true);
    motor.start();
    delay(DelayTransfer);
    motor.stop();
    louverPosition = true;
}

void Louver::doLouverClose()
{
    if (louverPosition == false)
        return;
    motor.setSpeed(MaxSpeed);
    motor.setDirection(false);
    motor.start();
    delay(DelayTransfer);
    motor.stop();
    louverPosition = false;
}

//ovladanie garaze
void Garage::setup(int en, int in1, int in2)
{
    motor.setup(en, in1, in2);
}

void Garage::doStop()
{
    motor.stop();
}

void Garage::doOpen()
{
    //ak ma limit v pozadovanom smere, nevykona pohyb
    if (_openLimit)
    {
        _isOpening = false;
        return;
    }
    motor.setSpeed(MaxSpeedToOpen);
    motor.setDirection(false);
    motor.start();
    _isOpening = true;
}

void Garage::doClose()
{
    //ak ma limit v pozadovanom smere, nevykona pohyb
    if (_closeLimit)
    {
        _isClosing = false;
        return;
    }

    motor.setSpeed(MaxSpeedToClose);
    motor.setDirection(true);
    motor.start();
    _isClosing = true;
}

//nastav limit open
void Garage::setOpened(bool limit)
{
    _openLimit = limit;
    //ak dosiahol limit v smere presunu, okamzite zastav pohyb
    if (isOpening() && _openLimit)
    {
        doStop();
        _isOpening = false;
    }
};
//nastav limit close
void Garage::setClosed(bool limit)
{
    _closeLimit = limit;
    //ak dosiahol limit v smere presunu, okamzite zastav pohyb
    if (isClosing() && _closeLimit)
    {
        doStop();
        _isClosing = false;
    }
};

//ovladanie svetla
void Light::setup()
{
    // nastavenie vystupov , relatok
    pinMode(LIGHT_RELAY_PIN, OUTPUT); // 12V zapnutie LED svetiel
    digitalWrite(LIGHT_RELAY_PIN, HIGH);
}
//zapne svetlo
void Light::doLightTurnOn()
{
    digitalWrite(LIGHT_RELAY_PIN, LOW); // relé 1 zapnut
    _isLightOn = true;
}
//vypne svetlo
void Light::doLightTurnOff()
{
    digitalWrite(LIGHT_RELAY_PIN, HIGH); // relé 1 vypnut
    _isLightOn = false;
}
//zapne automat
void Light::doLightStartAutomat()
{
    _isAutomat = true;
}
//vypne automat
void Light::doLightStopAutomat()
{
    _isAutomat = false;
}
//zisti osvit
int Light::doLightReadExposure()
{
    _osvit = analogRead(LIGHT_SENSOR_PIN);
    return doLightGetExposure();
}

//teplotne cidla
Temp::Temp()
{
    setSetTemp(25);
    // začni komunikovať s teplomermi
    _oneWireDS.begin(TEMP_INPUT_PIN);
    _senzoryDS.setOneWire(&_oneWireDS);
    _senzoryDS.begin();
    for (int i = 0; i < MAX_DS1820_SENSORS; i++)
    {
        if (!_oneWireDS.search(_addr[i]))
        {
            _oneWireDS.reset_search();
            delay(250);
            break;
        }
        _num_temp++; // počet teplomerov zvýš o 1
        delay(500);
    }
}

void Temp::doRead()
{
    // načíta všetky teplomery
    _senzoryDS.requestTemperatures();
    // uloží teploty do poľa teplota
    for (byte sensor = 0; sensor < _num_temp; sensor++)
    {
        _teplota[sensor] = _senzoryDS.getTempCByIndex(sensor);
    }
}