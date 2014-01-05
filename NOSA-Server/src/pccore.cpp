#include "pccore.h"
#include <iostream>
#include "helper.h"
#include "socketclosedexception.h"

PCCore::PCCore()
    : BaseCore(), socket(NULL), handler(NULL)
{
    std::cout << "Loading config ..." << std::endl;
    configuration = Configuration::loadFile("config.conf");
    if (configuration)
        std::cout << "Config loaded!" << std::endl;
    else if (const char* error = configuration->isValid())
    {
        std::cout << "Configuration file is not valid! (" << error << ")" << std::endl;
        exit(0);
    }
    else
    {
        std::cout << "Config file not found!" << std::endl;
        exit(0);
    }

    std::cout << "Connecting to server ..." << std::endl;
    if (!prepareSockets())
        return;

    socket = MainSocket::createSocket(configuration);
    if (socket)
    {
        std::cout << "Connected!" << std::endl;
        handler = new PacketHandler(socket);
    }
}

PCCore::~PCCore()
{
    delete socket;
}

int PCCore::run()
{
    Packet* packet;
    try
    {
        while(socket->isOpen())
        {
            packet = socket->readPacket();
            handler->accepted(packet);
            delete packet;
        }
    }
    catch (SocketClosedException& /*e*/) { /* just interrupt */ }

    delete packet;

    return 0;
}


