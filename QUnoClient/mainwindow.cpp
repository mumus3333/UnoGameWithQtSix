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
    if (data.startsWith("start")) {
        int playerCount = data.split(' ')[1].toInt();
        setupGameScreen(playerCount);
    } else {
        statusLabel->setText(data);
    }
}

void MainWindow::setupGameScreen(int playerCount)
{
    // Crear widgets para la pantalla del juego
    gameScreen = new QWidget(this);
    QGridLayout *layout = new QGridLayout(gameScreen);

    playerLabels.clear();

    // Crear etiquetas de jugador dinámicamente
    for (int i = 0; i < playerCount; ++i) {
        QLabel *playerLabel = new QLabel("Player " + QString::number(i + 1), gameScreen);
        playerLabels.append(playerLabel);
    }

    // Añadir widgets al layout basado en la cantidad de jugadores
    if (playerCount > 0) layout->addWidget(playerLabels[0], 0, 1); // Top
    if (playerCount > 1) layout->addWidget(playerLabels[1], 1, 0); // Left
    if (playerCount > 2) layout->addWidget(playerLabels[2], 1, 3); // Right
    if (playerCount > 3) layout->addWidget(playerLabels[3], 2, 1); // Bottom

    // Crear área de cartas en el tablero
    QLabel *cartasTablero = new QLabel("cartas que ponen en el tablero", gameScreen);
    QLabel *comerCartas = new QLabel("Comer cartas", gameScreen);

    // Añadir widgets al layout
    layout->addWidget(cartasTablero, 1, 1);
    layout->addWidget(comerCartas, 1, 2);

    // Configurar el layout
    gameScreen->setLayout(layout);
    setCentralWidget(gameScreen);
}
