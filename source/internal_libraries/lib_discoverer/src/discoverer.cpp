#include "discoverer.hpp"

Discoverer::Discoverer(QObject *parent)
    : QObject(parent)
    , groupAddress4(QStringLiteral("239.255.43.21"))
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
    QByteArray data = QString("RESPONSE:%1:%2").arg(name, QHostAddress(QHostAddress::LocalHost).toString()).toUtf8();

    // Send data to the IPv4 multicast group
    udpSocket4.writeDatagram(data, groupAddress4, 45454);

    // Send data to the IPv6 multicast group
    udpSocket6.writeDatagram(data, groupAddress6, 45454);

    //qDebug() << "REQUESTment sent: " << name;
}

void Discoverer::sendRequest()
{
    QByteArray request = "REQUEST";

    // Send discovery request to the multicast group
    udpSocket4.writeDatagram(request, groupAddress4, 45454);
    udpSocket6.writeDatagram(request, groupAddress6, 45454);

    //qDebug() << "Discovery request sent.";
}

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
                qDebug() << "Received RESPONSE from" << name << "at" << ip;
            }
        } else if (message == "REQUEST") {
            sendResponse("MyDevice"); // Replace with actual device name
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
                qDebug() << "Received RESPONSE from" << name << "at" << ip;
            }
        } else if (message == "REQUEST") {
            sendResponse("MyDevice"); // Replace with actual device name
        }
    }
}
