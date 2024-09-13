#include "peer_transfer.hpp"
#include <QDataStream>
#include <QDebug>

PeerFileTransfer::PeerFileTransfer(QObject *parent)
    : QObject(parent), server(new QTcpServer(this)), socket(nullptr), file(nullptr), isReceiving(false)
{
    connect(server, &QTcpServer::newConnection, this, &PeerFileTransfer::onNewConnection);
}

void PeerFileTransfer::startListening(quint16 port)
{
    if (!server->listen(QHostAddress::Any, port)) {
        qFatal("Failed to start server");
    }
    qDebug() << "Listening on port" << port;
}

void PeerFileTransfer::connectToPeer(const QString &host, quint16 port)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &PeerFileTransfer::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &PeerFileTransfer::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &PeerFileTransfer::onDisconnected);
    socket->connectToHost(host, port);
}

void PeerFileTransfer::sendFile(const QString &filePath)
{
    this->filePath = filePath;
    if (!socket) {
        qWarning("No connection established");
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Could not open file for reading");
        return;
    }

    QDataStream out(socket);
    out.setVersion(QDataStream::Qt_5_15); // Adjust as needed

    // Send file name
    out << QFileInfo(filePath).fileName();

    // Send file data
    QByteArray data;
    while (!file.atEnd()) {
        data = file.read(8192); // Read in chunks
        out << data;
    }

    file.close();
    socket->disconnectFromHost();
}

void PeerFileTransfer::onNewConnection()
{
    if (socket) {
        socket->deleteLater();
    }
    socket = server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &PeerFileTransfer::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &PeerFileTransfer::onDisconnected);

    isReceiving = true;
    qDebug() << "New connection established";
}

void PeerFileTransfer::onReadyRead()
{
    if (isReceiving) {
        handleReceivedFile();
    }
}

void PeerFileTransfer::handleReceivedFile()
{
    if (!file) {
        file = new QFile("received_file.txt");
        if (!file->open(QIODevice::WriteOnly)) {
            qWarning("Could not open file for writing");
            return;
        }
    }

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_15); // Adjust as needed

    QString fileName;
    QByteArray data;

    // Reading file name
    in >> fileName;
    qDebug() << "Receiving file:" << fileName;

    // Reading file data
    while (!in.atEnd()) {
        in >> data;
        file->write(data);
    }

    file->close();
    file->deleteLater();
    file = nullptr;
    isReceiving = false;
    qDebug() << "File received";
}

void PeerFileTransfer::onConnected()
{
    qDebug() << "Connected to peer";
}

void PeerFileTransfer::onDisconnected()
{
    qDebug() << "Disconnected from peer";
    socket->deleteLater();
    socket = nullptr;
}

