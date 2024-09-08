// Discoverer::Discoverer()
//     : listenerSocket(context, asio::ip::udp::v4())
//     , senderSocket(context, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0))
//     , broadcast_endpoint(asio::ip::address::from_string(BROADCAST_ADDRESS), PORT)
// {
//     try {
//         listenerSocket.bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), PORT));
//         senderSocket.set_option(asio::socket_base::broadcast(true));
//     } catch (const std::exception& e) {
//         std::cerr << "Socket initialization error: " << e.what() << std::endl;
//         throw;
//     }

//      // Start listening as soon as the object is constructed.

//     // Run the io_context in a separate thread if needed
//     // std::thread([this] { context.run(); }).detach();
//     std::thread([this] { context.run(); }).detach();
// }

// Discoverer::~Discoverer()
// {
//     // Properly shut down and close sockets if necessary
//     listenerSocket.close();
//     senderSocket.close();
// }

// void Discoverer::start_listening()
// {
//     std::array<char, 1024> buffer;
//     asio::ip::udp::endpoint endpoint;

//     listenerSocket.async_receive_from(
//         asio::buffer(buffer),
//         endpoint,

//         [&](std::error_code errorCode, std::size_t bytes_received)
//         {
//             if (errorCode)
//             {
//                 std::cerr << "Error receiving data: " << errorCode.message() << std::endl;
//                 // Optionally, restart listening in case of error
//                 start_listening();
//                 return;
//             }

//             std::string response_std(buffer.data(), bytes_received);

//             // Print received response for debugging
//             std::cout << "Received response from "
//                       << endpoint.address() << ": " << response_std
//                       << std::endl;

//             // Parse the response
//             std::string type;
//             std::string ip;
//             std::string host;

//             // Simple parsing based on the expected format "IP:<ip>,HOST:<host>,TYPE:<type>"
//             std::size_t ip_start = response_std.find("IP:") + 3;
//             std::size_t ip_end = response_std.find(",HOST:");
//             ip = response_std.substr(ip_start, ip_end - ip_start);

//             std::size_t host_start = response_std.find("HOST:") + 5;
//             std::size_t host_end = response_std.find(",TYPE:");
//             host = response_std.substr(host_start, host_end - host_start);

//             std::size_t type_start = response_std.find("TYPE:") + 5;
//             type = response_std.substr(type_start);

//             if (type == "REQUEST")
//             {
//                 qDebug() << "Hello";
//                 // Respond to the request with the current device's IP and host
//                 send_response(endpoint);

//             }

//             if (type == "RESPONSE")
//             {
//                 // Handle the response
//                 if (!devices.contains(QString::fromStdString(ip)))
//                 {
//                     devices.insert(QString::fromStdString(ip), QString::fromStdString(host));
//                 }

//                 qDebug() << devices;
//             }

//             // Continue listening
//             start_listening();
//         }
//         );
// }

// void Discoverer::send_signal()
// {

//     QString machineHostName = QSysInfo::machineHostName();

//     // Convert these to standard string types
//     std::string ip_address_str = getCurrentDeviceAddress().to_string();
//     std::string host_name_str = machineHostName.toStdString();

//     // Format the message
//     std::string data = "IP:" + ip_address_str + ",HOST:" + host_name_str + ",OPERATION TYPE:" + "REQUEST";

//     // Send "device name", "ip", and "operation type" here.
//     senderSocket.async_send_to(asio::buffer(data), broadcast_endpoint,

//                                [&](std::error_code ec, std::size_t bytes_sent)
//                                {
//                                    if (ec)
//                                    {
//                                        std::cerr << "Error sending signal: " << ec.message() << std::endl;
//                                    }
//                                    else
//                                    {
//                                        std::cout << "Broadcast message sent: " << data << std::endl;
//                                    }
//                                }
//                                );
// }

// void Discoverer::send_response(const asio::ip::udp::endpoint requester_endpoint)
// {
//     // Construct the response message
//     asio::ip::address ip_address = getCurrentDeviceAddress();
//     QString host_name = QSysInfo::machineHostName();
//     std::string ip_address_str = ip_address.to_string();
//     std::string host_name_str = host_name.toStdString();
//     std::string response = "IP:" + ip_address_str + ",HOST:" + host_name_str + ",TYPE:RESPONSE";

//     // Send the response to the requester
//     senderSocket.async_send_to(
//         asio::buffer(response),
//         requester_endpoint,
//         [&](std::error_code ec, std::size_t bytes_sent)
//         {
//             if (ec)
//             {
//                 std::cerr << "Error sending response: " << ec.message() << std::endl;
//             }
//             else
//             {
//                 std::cout << "Response sent" << std::endl;
//             }
//         }
//         );
// }

// asio::ip::address Discoverer::getCurrentDeviceAddress()
// {
//     asio::ip::address result;

//     try {
//         asio::ip::tcp::resolver resolver(context);
//         asio::ip::tcp::resolver::results_type endpoints = resolver.resolve("localhost", "0");

//         // Iterate over the endpoints and obtain IP addresses
//         for (const auto& endpoint : endpoints) {
//             asio::ip::address address = endpoint.endpoint().address();
//             //std::cout << "IP address: " << address.to_string() << std::endl;

//             result = address;
//         }
//     } catch (const std::exception& e) {
//         std::cerr << "Error resolving address: " << e.what() << std::endl;
//     }

//     return result;
// }
















#include "discoverer.hpp"

BluetoothManager::BluetoothManager(QObject *parent)
    : QObject(parent)
    , discoveryAgent(new QBluetoothDeviceDiscoveryAgent(this))
    , server(new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this))
    , bluetoothSocket(new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this))
    , serviceUuid("00001101-0000-1000-8000-00805F9B34FB")
{
    connect(server, &QBluetoothServer::newConnection, this, &BluetoothManager::handleNewConnection);

    connect(
        discoveryAgent,
        &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
        this,
        &BluetoothManager::deviceDiscovered
    );

    connect(bluetoothSocket, &QBluetoothSocket::connected, this, [this]() {
        qDebug() << "Connected to device!";
        sendFile(bluetoothSocket, "path/to/your/file");
    });

    connect(bluetoothSocket, &QBluetoothSocket::disconnected, this, [this]() {
        qDebug() << "Disconnected from device!";
        //bluetoothSocket->deleteLater();
        //bluetoothSocket = nullptr;
    });

    connect(bluetoothSocket, &QBluetoothSocket::errorOccurred, this, [this](QBluetoothSocket::SocketError error) {
        qDebug() << "Socket error:" << bluetoothSocket->errorString();
    });

    connect(bluetoothSocket, &QBluetoothSocket::stateChanged, this, [this](QBluetoothSocket::SocketState state) {
        qDebug() << "Socket state changed:" << static_cast<int>(state);
    });

    server->listen();  // Start listening for connections
    discoveryAgent->start();
}

BluetoothManager::~BluetoothManager()
{
    discoveryAgent->stop();
    server->close();
}

void BluetoothManager::startDeviceDiscovery()
{
    discoveryAgent->start();
}

// In your local slot, read information about the found devices
void BluetoothManager::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    qDebug() << "Found new device:" << device.name() << '(' << device.address().toString() << ')' << "\n";

    // Here you can call discoveredDevices() and refresh your list to QML
    // qDebug() << "New list: ";

    // for(const QBluetoothDeviceInfo& device : discoveryAgent->discoveredDevices())
    // {
    //     qDebug() << device.name();
    // }

    connectToDevice(device);
}

void BluetoothManager::connectToDevice(const QBluetoothDeviceInfo &device)
{
    bluetoothSocket->connectToService(device.address(), serviceUuid, QIODevice::ReadWrite);
}

void BluetoothManager::disconnectDevice()
{
    bluetoothSocket->disconnect();
}

void BluetoothManager::sendFile(QBluetoothSocket *socket, const QString &filePath)
{
    bluetoothSocket->write("Sending signal");
    // QFile file(filePath);
    // if (!file.open(QIODevice::ReadOnly)) {
    //     qDebug() << "Failed to open file:" << file.errorString();
    //     return;
    // }

    // QByteArray fileData = file.readAll();
    // qint64 bytesWritten = socket->write(fileData);
    // if (bytesWritten == -1) {
    //     qDebug() << "Failed to write data to socket:" << socket->errorString();
    // } else {
    //     qDebug() << bytesWritten << "bytes written to socket.";
    // }

    // file.close();
}


void BluetoothManager::handleNewConnection()
{
    QBluetoothSocket *socket = server->nextPendingConnection();

    connect(socket, &QBluetoothSocket::readyRead, this, [this, socket]() {
        handleReadyRead(socket);
    });

    connect(socket, &QBluetoothSocket::disconnected, socket, &QObject::deleteLater);
}

void BluetoothManager::handleReadyRead(QBluetoothSocket *socket)
{
    QByteArray data = socket->readAll();
    qDebug() << "Received data:" << data;

    // Example of saving received data to a file
    // QFile file("received_file");
    // if (file.open(QIODevice::WriteOnly)) {
    //     file.write(data);
    //     file.close();
    //     qDebug() << "File saved successfully.";
    // } else {
    //     qDebug() << "Failed to save file:" << file.errorString();
    // }
}
