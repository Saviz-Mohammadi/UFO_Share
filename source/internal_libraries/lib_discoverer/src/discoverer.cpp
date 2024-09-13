#include "discoverer.hpp"
#include <QSysInfo>

Discoverer::Discoverer(QObject *parent)
    : QObject(parent)
    , groupAddress4(QStringLiteral("239.255.255.250")) // I don't know why, but it seems like this address was the key to making the project work.
    // I don't understand, everything is correct to the finest detail, but just change the address made everything work. What is so special about this one?

    // Here is the reason apparntely:
    // The address 239.255.255.250 is a well-known multicast address that many devices and services are configured to listen to by default. It is used for SSDP, which is a protocol used for service discovery on local networks. Many devices, routers, and services are pre-configured to communicate using this address, making it a reliable choice for multicast communication in many environments.

    , groupAddress6(QStringLiteral("ff12::2115"))
{
    //qDebug() << "Listening for multicast messages on both IPv4 and IPv6";

    udpSocket4.bind(QHostAddress::AnyIPv4, 45454, QUdpSocket::ShareAddress);
    udpSocket4.joinMulticastGroup(groupAddress4);

    if (!udpSocket6.bind(QHostAddress::AnyIPv6, 45454, QUdpSocket::ShareAddress) ||
        !udpSocket6.joinMulticastGroup(groupAddress6))
    {
        //qDebug() << "Listening for multicast messages on IPv4 only";
    }

    connect(&udpSocket4, &QUdpSocket::readyRead,
            this, &Discoverer::processPendingDatagrams);

    connect(&udpSocket6, &QUdpSocket::readyRead,
            this, &Discoverer::processPendingDatagrams);
}

Discoverer::~Discoverer()
{
}

void Discoverer::sendResponse(const QString &name)
{
    QByteArray data = QString("RESPONSE:%1:%2").arg(QSysInfo::machineHostName(), QHostAddress(QHostAddress::LocalHost).toString()).toUtf8();

    // Send data to the IPv4 multicast group
    udpSocket4.writeDatagram(data, groupAddress4, 45454);

    // Send data to the IPv6 multicast group
    udpSocket6.writeDatagram(data, groupAddress6, 45454);

    //qDebug() << "REQUESTment sent: " << name;
}

// This method is used to notify the network that the current device is going to close and be disconnected.
// Any devices recieving this message, should remove this device from their list.
void Discoverer::sendRemove(const QString &name)
{
    QByteArray data = QString("REMOVE:%1:%2").arg(QSysInfo::machineHostName(), QHostAddress(QHostAddress::LocalHost).toString()).toUtf8();

    // Send data to the IPv4 multicast group
    udpSocket4.writeDatagram(data, groupAddress4, 45454);

    // Send data to the IPv6 multicast group
    udpSocket6.writeDatagram(data, groupAddress6, 45454);
}

void Discoverer::sendRequest()
{
    QByteArray request = "REQUEST";

    // Send discovery request to the multicast group
    udpSocket4.writeDatagram(request, groupAddress4, 45454);
    udpSocket6.writeDatagram(request, groupAddress6, 45454);

    //qDebug() << "Discovery request sent.";
}


// TODO (Saviz): For now this is fine, but usually it is better for the key to be as unique as possible.
// So, I think it is better to have the key be ip address instead of name, just in case someone uses the same name for all of their devices.
void Discoverer::processPendingDatagrams()
{
    QByteArray datagram;

    // Process IPv4 datagrams
    while (udpSocket4.hasPendingDatagrams()) {
        datagram.resize(qsizetype(udpSocket4.pendingDatagramSize()));
        udpSocket4.readDatagram(datagram.data(), datagram.size());

        QString message = QString(datagram.constData());
        if (message.startsWith("RESPONSE:")) {
            QStringList parts = message.mid(10).split(':');
            if (parts.size() == 2) {
                QString name = parts[0];
                QString ip = parts[1];
                //qDebug() << "Received RESPONSE from" << name << "at" << ip;

                if(!m_Devices.contains(name))
                {
                    m_Devices.insert(name, ip);
                }

                qDebug() << m_Devices;
            }
        } else if (message == "REQUEST") {
            sendResponse("MyDevice"); // Replace with actual device name
        } else if (message == "REMOVE") {
            QStringList parts = message.mid(10).split(':');
            if (parts.size() == 2) {
                QString name = parts[0];
                //QString ip = parts[1];
                //qDebug() << "Received RESPONSE from" << name << "at" << ip;

                m_Devices.remove(name);

                qDebug() << m_Devices;
            }
        }

    }

    // Process IPv6 datagrams
    while (udpSocket6.hasPendingDatagrams()) {
        QNetworkDatagram dgram = udpSocket6.receiveDatagram();
        QString message = QString(dgram.data());

        if (message.startsWith("RESPONSE:")) {
            QStringList parts = message.mid(10).split(':');
            if (parts.size() == 2) {
                QString name = parts[0];
                QString ip = parts[1];
                //qDebug() << "Received RESPONSE from" << name << "at" << ip;

                if(!m_Devices.contains(name))
                {
                    m_Devices.insert(name, ip);
                }

                qDebug() << m_Devices;
            }
        } else if (message == "REQUEST") {
            sendResponse("MyDevice"); // Replace with actual device name
        } else if (message == "REMOVE") {
            QStringList parts = message.mid(10).split(':');
            if (parts.size() == 2) {
                QString name = parts[0];
                //QString ip = parts[1];
                //qDebug() << "Received RESPONSE from" << name << "at" << ip;

                m_Devices.remove(name);

                qDebug() << m_Devices;
            }
        }
    }
}
