#ifndef PEERFILETRANSFER_H
#define PEERFILETRANSFER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QFile>
#include <QFileInfo>

class PeerFileTransfer : public QObject
{
    Q_OBJECT

public:
    explicit PeerFileTransfer(QObject *parent = nullptr);
    void startListening(quint16 port);
    void connectToPeer(const QString &host, quint16 port);
    void sendFile(const QString &filePath);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onConnected();
    void onDisconnected();

private:
    void handleReceivedFile();

    QTcpServer *server;
    QTcpSocket *socket;
    QFile *file;
    QString filePath;
    bool isReceiving;
};

#endif // PEERFILETRANSFER_H
