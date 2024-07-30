#include "mainwindow.h"
#include "label.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , socket(new QTcpSocket(this))
{
    //tamaño de ventana principal
    setFixedSize(1280, 720);

    // Crear widgets
    serverIpLineEdit = new QLineEdit("192.168.0.13", this); // IP predefinida del servidor
    playerNameLineEdit = new QLineEdit(this);
    connectButton = new QPushButton("Connect", this);
    statusLabel = new QLabel("Status: Not connected", this);

    // Crear layout para la pantalla de conexión
    QVBoxLayout *connectLayout = new QVBoxLayout();
    connectLayout->addWidget(new QLabel("Server IP:", this));
    connectLayout->addWidget(serverIpLineEdit);
    connectLayout->addWidget(new QLabel("Player Name:", this));
    connectLayout->addWidget(playerNameLineEdit);
    connectLayout->addWidget(connectButton);
    connectLayout->addWidget(statusLabel);

    // Crear widget para la pantalla de conexión
    connectScreen = new QWidget(this);
    connectScreen->setLayout(connectLayout);

    // Crear stacked widget para cambiar entre pantallas
    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(connectScreen);

    setCentralWidget(stackedWidget);

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
        showRulesScreen();
        QTimer::singleShot(5000, this, [this, playerCount] {
            setupGameScreen(playerCount);
            stackedWidget->setCurrentWidget(gameScreen);
        });
    } else {
        statusLabel->setText(data);
    }
}

void MainWindow::showRulesScreen()
{
    rulesScreen = new QWidget(this);
    QVBoxLayout *rulesLayout = new QVBoxLayout(rulesScreen);
    Label *rulesLabel = new Label(rulesScreen);
    rulesLayout->addWidget(rulesLabel);
    rulesScreen->setLayout(rulesLayout);
    stackedWidget->addWidget(rulesScreen);
    stackedWidget->setCurrentWidget(rulesScreen);
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
        playerLabel->setAlignment(Qt::AlignCenter);
        playerLabels.append(playerLabel);
    }

    // Añadir widgets al layout basado en la cantidad de jugadores
    if (playerCount == 2) {
        layout->addWidget(playerLabels[1], 0, 1); // Top (Player 2)
        QLabel *cartasTablero = new QLabel("Cartas en el tablero", gameScreen);
        cartasTablero->setAlignment(Qt::AlignCenter);
        QLabel *comerCartas = new QLabel("Comer cartas", gameScreen);
        comerCartas->setAlignment(Qt::AlignCenter);
        layout->addWidget(cartasTablero, 1, 1);
        layout->addWidget(comerCartas, 1, 2);
        displayPlayerHand();
        layout->addWidget(playerHandWidget, 2, 1); // Bottom (Player 1)
    } else if (playerCount == 3) {
        layout->addWidget(playerLabels[1], 0, 1); // Top (Player 2)
        layout->addWidget(playerLabels[2], 1, 0); // Left (Player 3)
        QLabel *cartasTablero = new QLabel("Cartas en el tablero", gameScreen);
        cartasTablero->setAlignment(Qt::AlignCenter);
        QLabel *comerCartas = new QLabel("Comer cartas", gameScreen);
        comerCartas->setAlignment(Qt::AlignCenter);
        layout->addWidget(cartasTablero, 1, 1);
        layout->addWidget(comerCartas, 1, 2);
        displayPlayerHand();
        layout->addWidget(playerHandWidget, 2, 1); // Bottom (Player 1)
    } else if (playerCount == 4) {
        layout->addWidget(playerLabels[1], 0, 1); // Top (Player 2)
        layout->addWidget(playerLabels[2], 1, 0); // Left (Player 3)
        layout->addWidget(playerLabels[3], 1, 2); // Right (Player 4)
        QLabel *cartasTablero = new QLabel("Cartas en el tablero", gameScreen);
        cartasTablero->setAlignment(Qt::AlignCenter);
        QLabel *comerCartas = new QLabel("Comer cartas", gameScreen);
        comerCartas->setAlignment(Qt::AlignCenter);
        layout->addWidget(cartasTablero, 1, 1);
        layout->addWidget(comerCartas, 1, 2);
        displayPlayerHand();
        layout->addWidget(playerHandWidget, 2, 1); // Bottom (Player 1)
    }

    // Configurar el layout
    gameScreen->setLayout(layout);
    stackedWidget->addWidget(gameScreen);
}

void MainWindow::displayPlayerHand()
{
    // Crear etiquetas para las cartas en la mano del jugador
    playerHandCards.clear();
    QHBoxLayout *handLayout = new QHBoxLayout();
    for (int i = 0; i < 7; ++i) { // Suponiendo que cada jugador comienza con 7 cartas
        QLabel *cardLabel = new QLabel("Card " + QString::number(i + 1), this);
        cardLabel->setAlignment(Qt::AlignCenter);
        playerHandCards.append(cardLabel);
        handLayout->addWidget(cardLabel);
    }
    playerHandWidget = new QWidget();
    playerHandWidget->setLayout(handLayout);
}
