#include "ledserver.h"
#include "config.h"



LedServer::LedServer(quint16 port, QObject *parent) : QObject(parent), m_port(port)
{
    m_gpio = new Gpio(this);
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &LedServer::myNewConnection);
}

void LedServer::start()
{
    m_server->listen(QHostAddress::Any, m_port);
}

// Client hat Verbindung zum Server aufgebaut
void LedServer::myNewConnection()
{
    qDebug() << "Verbunden zum PORT: " + QString::number(m_port);
    m_socket = m_server->nextPendingConnection();
    m_socket->write("Bitte Zahl von 0 bis 15 eingeben: ");
    connect(m_socket, &QTcpSocket::readyRead, this, &LedServer::myServerRead);
    // Wenn Daten im Socket ankommen (geschriebenes), verbindet er mit myServerRead (führt aus)
    connect(m_socket, &QTcpSocket::disconnected, this, &LedServer::myClientDisconnect);
    // Trennt die Verbindung wenn TcpSocket Verbindung trennt + führt myDisconnect Funktion aus
}

// Client hat Verbindung zum Server getrennt
void LedServer::myClientDisconnect()
{
    for (int i = 0; i < 4; i++)
    {
        m_gpio->set(LEDS[i], 0);
    }
    qDebug() << "Disconnected";
    m_socket->close();

}

// Client hat eine Zahl (0...15) zum Server gesendet
void LedServer::myServerRead()
{
    while (m_socket->bytesAvailable()) {
        QString message = m_socket->readAll();
        int zahl = message.toInt();
        if (zahl >= 0 && zahl <= 15)
        {
            m_socket->write("OK\n");
            m_gpio->set(zahl);
        }
        m_socket->write("Bitte Zahl von 0 bis 15 eingeben: ");

    }
}
