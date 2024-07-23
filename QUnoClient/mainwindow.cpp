#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , socket(new QTcpSocket(this))
{
    // Crear widgets
    serverIpLineEdit = new QLineEdit("192.168.0.13", this); // IP predefinida del servidor
    playerNameLineEdit = new QLineEdit(this);
    connectButton = new QPushButton("Connect", this);
    statusLabel = new QLabel("Status: Not connected", this);

    // Crear layout
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(new QLabel("Server IP:", this));
    layout->addWidget(serverIpLineEdit);
    layout->addWidget(new QLabel("Player Name:", this));
    layout->addWidget(playerNameLineEdit);
    layout->addWidget(connectButton);
    layout->addWidget(statusLabel);

    // Crear widget central
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Conectar señales y slots
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::on_connectButton_clicked);
    connect(socket, &QTcpSocket::connected, this, &MainWindow::on_connected);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::on_readyRead);
}

MainWindow::~MainWindow()
{
    // Los widgets se eliminarán automáticamente
}

void MainWindow::on_connectButton_clicked()
{
    QString serverIp = serverIpLineEdit->text();
    QString playerName = playerNameLineEdit->text();
    socket->connectToHost(serverIp, 1234); // Puerto fijo para el servidor
    socket->write(playerName.toUtf8());
}

void MainWindow::on_connected()
{
    statusLabel->setText("Connected to server");
}

void MainWindow::on_readyRead()
{
    QByteArray data = socket->readAll();
    statusLabel->setText(data);
}
