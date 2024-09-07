#ifndef DISCOVERER_H
#define DISCOVERER_H

#include <QSysInfo>
#include <asio.hpp>
#include <string>
#include <array>
#include <QMap>
#include <QDebug>

// Turn this into a Qt class once the testing is done.

class Discoverer
{
private:
    QMap<QString, QString> devices;
    // Port for UDP communication
    const short PORT = 12345;

    // This address targets all devices on the local network
    const std::string BROADCAST_ADDRESS = "255.255.255.255";

    asio::io_context context;

private:
    asio::ip::udp::socket listenerSocket;
    asio::ip::udp::socket senderSocket;

private:
    std::array<char, 1024> listener_buffer;


    // TODO (Saviz): Add a Map here that is compared and regenerated every time in teh listener.

public:
    Discoverer();
    ~Discoverer();

    // TODO (Saviz): Make this multithreaded
    void start_listening();

    // TODO (Saviz): Make this multithreaded
    void send_signal();
    void send_request();
    void send_response(const asio::ip::udp::endpoint& requester_endpoint);
    asio::ip::address getCurrentDeviceAddress();
};


#endif // DISCOVERER_H
