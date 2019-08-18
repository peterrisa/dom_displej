#include <server.h>

//komunikacia so serverom
Server::Server()
{
    //************************************************************
    // commandy
    setWord(Server::STM, 25); // ziadne prikazy z nadradeneho servera
    setWord(Server::CMD, Server::CMD_NONE);  // ziadne prikazy z nadradeneho servera
}
//
void Server::setWord(Server::WORDS address, word data)
{
    telegram.msg.w[address] = data;
}
//
word Server::getWord(Server::WORDS address)
{
    return telegram.msg.w[address];
}
//
void Server::setBit(Server::WORDS address, Server::MASKS mask, bool bit)
{
    if (bit)
    {
        BITMASK_SET(telegram.msg.w[address], mask);
    }
    else
    {
        BITMASK_CLEAR(telegram.msg.w[address], mask);
    }
}
//
bool Server::getBit(Server::WORDS address, Server::MASKS mask){
    return BITMASK_CHECK_ALL(telegram.msg.w[address], mask);
}
//--------------------------------------------------------------
// odoslat data do serveru
//--------------------------------------------------------------
void Server::doWriteMessage()
{
    // zakoduj spravu pre server
    telegram.encodeTelegram();
    // odosli data po seriovej linke
    Serial.println((char *)telegram.getBuffer());
}
//--------------------------------------------------------------
// nacitaj data zo serveru
//--------------------------------------------------------------
void Server::doReadMessage()
{
    Telegram new_msg;
    // ak su prijate data zo servera, nacita ich
    if (Serial.available() >= Telegram::BUF_LEN)
    {
        for (int i = 0; i < Telegram::BUF_LEN; i++)
        {
            byte c = Serial.read();
            new_msg.setByteInBuffer(i, c);
        }
        while (Serial.available())
            Serial.read();

        // dekoduj spravu od servera
        new_msg.decodeTelegram();
        // ak je sprava validna
        if (new_msg.isValidTelegram())
        {
            //  Serial.println("arduino rx is valid\n");
            // prekopiruj cmd - prikazy zo servera pre Arduino
            telegram.msg.w[Server::CMD] = new_msg.msg.w[Server::CMD];
            // prekopiruj hodnotu setpointu
            telegram.msg.w[Server::STM] = new_msg.msg.w[Server::STM];
        }
    }
}
