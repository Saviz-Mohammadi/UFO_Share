#ifndef DISCOVERER_H
#define DISCOVERER_H

#include <QObject>
#include <QDebug>
#include <QHostAddress>
#include <QUdpSocket>
#include <QtNetwork>

class Discoverer : public QObject
{
    Q_OBJECT

public:
    explicit Discoverer(QObject *parent = nullptr);
    ~Discoverer();

    void sendRequest();
    void sendResponse(const QString &name);
    void processPendingDatagrams();

private:
    QUdpSocket udpSocket4;
    QUdpSocket udpSocket6;
    QHostAddress groupAddress4;
    QHostAddress groupAddress6;
    QString deviceName;
};

#endif // DISCOVERER_H
