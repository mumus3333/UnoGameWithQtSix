#include "mainwindow.h"
#include <QNetworkInterface>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , server(new QTcpServer(this))
{
    // Crear widgets
    connectionsTextEdit = new QTextEdit(this);
    statusLabel = new QLabel("Server status: Not started", this);
    ipLabel = new QLabel("Server IP: 192.168.0.13", this); // IP fija del servidor

    // Crear layout
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(ipLabel);
    layout->addWidget(statusLabel);
    layout->addWidget(connectionsTextEdit);

    // Crear widget central
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Conectar señales y slots
    connect(server, &QTcpServer::newConnection, this, &MainWindow::on_newConnection);

    if (server->listen(QHostAddress::Any, 1234)) {
        statusLabel->setText("Server started...");
    } else {
        statusLabel->setText("Server failed to start...");
    }
}

MainWindow::~MainWindow()
{
    // Los widgets se eliminarán automáticamente
}

void MainWindow::on_newConnection()
{
    QTcpSocket *clientSocket = server->nextPendingConnection();
    clients << clientSocket;
    connect(clientSocket, &QTcpSocket::readyRead, this, &MainWindow::on_readyRead);
}

void MainWindow::on_readyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    QByteArray data = clientSocket->readAll();
    QString playerName = QString::fromUtf8(data);
    connectionsTextEdit->append("Player connected: " + playerName + " (" + clientSocket->peerAddress().toString() + ")");
}
