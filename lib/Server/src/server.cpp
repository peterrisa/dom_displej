#include <server.h>

//---Komunikacia zo serverom---
Server::Server()
{
    //************************************************************
    //commandy
    setWord(Server::STM, 25);//žiadne prikazy z nadradeneho servera
    setWord(Server::CMD, Server::CMD_NONE);//ziadne prikazy z nadradeného servera
}

void Server::setWord(Server::WORDS address, word data)
{
    telegram.msg.w[address] = data;
}

word Server::getWord(Server::WORDS address)
{
    return telegram.msg.w[address];
}

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
//odoslať dáta do serveru
//--------------------------------------------------------------
void Server::doWriteMessage()
{
    //zakóduj správu pre server
    telegram.encodeTelegram();
    //odošli dáta po sériovej linke
    Serial.println((char *)telegram.getBuffer());
}
//--------------------------------------------------------------
//načítaj dáta zo serveru
//--------------------------------------------------------------
void Server::doReadMessage()
{
    Telegram new_msg;
    //ak sú prijaté data zo servera, načita ich
    if (Serial.available() >= Telegram::BUF_LEN)
    {
        for (int i = 0; i < Telegram::BUF_LEN; i++)
        {
            byte c = Serial.read();
            new_msg.setByteInBuffer(i, c);
        }
        while (Serial.available())
            Serial.read();

        //dekóduj spravu od servera
        new_msg.decodeTelegram();
        //ak je sprava validna
        if (new_msg.isValidTelegram())
        {
            //vypíš do sériovej linky |Serial.println("arduino rx is valid\n");|
            //prekopíruj cmd - prikazy zo servera pre Arduino
            telegram.msg.w[Server::CMD] = new_msg.msg.w[Server::CMD];
            // prekopiruj hodnotu setpointu
            telegram.msg.w[Server::STM] = new_msg.msg.w[Server::STM];
        }
    }
}
