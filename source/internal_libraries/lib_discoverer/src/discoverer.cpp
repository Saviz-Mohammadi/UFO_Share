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

UdpClient::UdpClient(QObject *parent)
    : QObject(parent)
    , groupAddress6(QStringLiteral("ff12::2115"))
{
    connect(&udpSocket6, &QUdpSocket::readyRead, this, &UdpClient::processPendingDatagrams);
}

UdpClient::~UdpClient()
{
    // Leave the multicast group if necessary
    if (udpSocket6.state() == QAbstractSocket::BoundState)
    {
        udpSocket6.leaveMulticastGroup(groupAddress6);
    }

    udpSocket6.close();
}

void UdpClient::sendMessage(const QString &type)
{
    QByteArray byteArray;
    byteArray.clear();

    QString host = QSysInfo::machineHostName();
    QString message = QString("HOST:%1,TYPE:%2").arg(host, type);

    byteArray.append(message.toUtf8());

    QNetworkDatagram datagram(byteArray, groupAddress6, m_port);
    udpSocket6.writeDatagram(datagram);
}

void UdpClient::startListening(quint16 port)
{
    m_port = port;

    udpSocket6.bind(QHostAddress(QHostAddress::AnyIPv6), port);


    if (!udpSocket6.joinMulticastGroup(groupAddress6))
    {
        qDebug() << "Failed to join multicast group:" << groupAddress6.toString();
    }
}

void UdpClient::processPendingDatagrams()
{
    while (udpSocket6.hasPendingDatagrams())
    {
        QNetworkDatagram datagram = udpSocket6.receiveDatagram();

        QStringList parts = QString::fromUtf8(datagram.data()).split(",");

        QString hostPart = parts.value(0);
        QString typePart = parts.value(1);

        QString host = hostPart.section(":", 1, 1);
        QString type = typePart.section(":", 1, 1);

        if (type == "REQUEST")
        {
            sendMessage("RESPONSE");
        }

        else if (type == "RESPONSE")
        {
            if (!deviceMap.contains(host))
            {
                deviceMap.insert(host, datagram.senderAddress().toString());
                qDebug() << "Added Host: " << host << " Address: " << datagram.senderAddress().toString();
            }

            qDebug() << deviceMap;
        }

        // else if (type == "DISCONNECTED") {
        //     if (deviceMap.contains(host)) {
        //         deviceMap.remove(host);
        //         qDebug() << "Removed from map:" << host;
        //     }
        // }


    }
}
