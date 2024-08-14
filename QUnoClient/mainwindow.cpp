#include "mainwindow.h"
#include "label.h"
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , socket(new QTcpSocket(this))
    , mazo(new Mazo(this))
{
    // Tamaño de ventana principal
    setFixedSize(1280, 720);

    // Crear widgets para la pantalla de conexión
    serverIpLineEdit = new QLineEdit("192.168.0.13", this); // IP predefinida del servidor
    playerNameLineEdit = new QLineEdit(this);
    connectButton = new QPushButton("Connect", this);
    statusLabel = new QLabel("Status: Not connected", this);

    QVBoxLayout *connectLayout = new QVBoxLayout();
    connectLayout->addWidget(new QLabel("Server IP:", this));
    connectLayout->addWidget(serverIpLineEdit);
    connectLayout->addWidget(new QLabel("Player Name:", this));
    connectLayout->addWidget(playerNameLineEdit);
    connectLayout->addWidget(connectButton);
    connectLayout->addWidget(statusLabel);

    connectScreen = new QWidget(this);
    connectScreen->setLayout(connectLayout);

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

    if (socket->waitForConnected(3000)) {
        qDebug() << "Connected to server, sending player name...";
        socket->write(playerName.toUtf8());
    } else {
        qDebug() << "Failed to connect to server: " << socket->errorString();
        statusLabel->setText("Failed to connect to server");
    }
}

void MainWindow::on_connected()
{
    statusLabel->setText("Connected to server");
}

void MainWindow::on_readyRead() {
    QByteArray data = socket->readAll();
    QDataStream in(&data, QIODevice::ReadOnly);

    QString tablero;
    QVector<QVector<QString>> hands;
    int turno;

    in >> tablero;
    in >> hands;
    in >> turno;

    if (turno < 0 || turno >= totalPlayers) {
        qDebug() << "Invalid player turn index received:" << turno;
        return;
    }

    updateGameScreen(tablero, hands, turno);
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

QString getColor(const QString &card) {
    return card.split(" ")[0];
}

QString getNumber(const QString &card) {
    return card.split(" ").size() > 1 ? card.split(" ")[1] : "";
}

void MainWindow::setupGameScreen(int playerCount)
{
    totalPlayers = playerCount;
    currentPlayerIndex = 0;

    // Crear widgets para la pantalla del juego
    gameScreen = new QWidget(this);
    QGridLayout *layout = new QGridLayout(gameScreen);

    playerLabels.clear();
    turnLabels.clear();  // Asegúrate de que turnLabels esté vacío
    playerHands.resize(playerCount);

    // Crear etiquetas de jugador dinámicamente
    for (int i = 0; i < playerCount; ++i) {
        QLabel *playerLabel = new QLabel("Jugador " + QString::number(i + 1), gameScreen);
        playerLabel->setAlignment(Qt::AlignCenter);
        playerLabels.append(playerLabel);

        QLabel *turnLabel = new QLabel("", gameScreen);
        turnLabels.append(turnLabel);

        QHBoxLayout *playerLayout = new QHBoxLayout();
        playerLayout->addWidget(playerLabel);
        playerLayout->addWidget(turnLabel);

        layout->addLayout(playerLayout, i, 0);
        playerHands[i] = mazo->repartirCartas(7);
    }

    // Crear botones para las acciones del jugador
    playCardButton = new QPushButton("Jugar Carta", gameScreen);
    passTurnButton = new QPushButton("Pasar Turno", gameScreen);

    // Añadir los botones al layout
    layout->addWidget(playCardButton, playerCount + 2, 0);
    layout->addWidget(passTurnButton, playerCount + 2, 1);

    // Conectar los botones a las funciones correspondientes
    connect(playCardButton, &QPushButton::clicked, this, &MainWindow::playerAction);
    connect(passTurnButton, &QPushButton::clicked, this, &MainWindow::endTurn);

    // Deshabilitar los botones inicialmente
    playCardButton->setEnabled(false);
    passTurnButton->setEnabled(false);

    // Seleccionar una carta aleatoria para el tablero
    cartaTablero = new QLabel(this); // Crear un QLabel
    cartaTablero->setAlignment(Qt::AlignCenter);
    layout->addWidget(cartaTablero, 1, 1);

    displayPlayerHand();
    layout->addWidget(playerHandWidget, playerCount + 1, 0);

    gameScreen->setLayout(layout);
    stackedWidget->addWidget(gameScreen);

    // Iniciar el primer turno
    startTurn();
}

void MainWindow::startTurn()
{
    // Restablecer los QLabel de turnos para todos los jugadores
    for (int i = 0; i < totalPlayers; ++i) {
        turnLabels[i]->setText("");
    }

    // Mostrar quién tiene el turno actualmente
    turnLabels[currentPlayerIndex]->setText("Es tu turno");

    // Habilitar los botones solo si es el turno del jugador correspondiente
    playCardButton->setEnabled(true);
    passTurnButton->setEnabled(true);
}

void MainWindow::endTurn()
{
    // Deshabilitar los botones cuando el turno termina
    playCardButton->setEnabled(false);
    passTurnButton->setEnabled(false);

    // Finaliza el turno del jugador actual
    currentPlayerIndex = (currentPlayerIndex + 1) % totalPlayers;

    // Inicia el turno del siguiente jugador
    startTurn();
}

void MainWindow::displayPlayerHand()
{
    playerHandCards.clear();
    QHBoxLayout *handLayout = new QHBoxLayout();

    QString tableroColor = getColor(cartaTablero->text());
    QString tableroNumber = getNumber(cartaTablero->text());

    for (int i = 0; i < playerHands[currentPlayerIndex].size(); ++i) {
        QString cardText = playerHands[currentPlayerIndex][i];
        QPushButton *cardButton = new QPushButton(cardText, this);

        QString cardColor = getColor(cardText);
        QString cardNumber = getNumber(cardText);

        // Habilitar la carta solo si coincide en color o número, o si es "Wild"
        if (cardColor == tableroColor || cardNumber == tableroNumber || cardColor == "Wild") {
            cardButton->setEnabled(true);
            connect(cardButton, &QPushButton::clicked, this, [this, i]() {
                this->playCard(i);
            });
        } else {
            cardButton->setEnabled(false); // Deshabilitar la carta si no coincide
        }

        handLayout->addWidget(cardButton);
        playerHandCards.append(cardButton);
    }
    playerHandWidget = new QWidget();
    playerHandWidget->setLayout(handLayout);
}

void MainWindow::playerAction()
{
    // Ejemplo simple: realizar una acción y mostrar un mensaje
    QString currentPlayerName = "Jugador " + QString::number(currentPlayerIndex + 1);
    QMessageBox::information(this, "Acción", currentPlayerName + " ha realizado una acción.");

    // Después de que el jugador haya realizado su acción, pasa al siguiente turno
    endTurn();
}

void MainWindow::playCard(int cardIndex)
{
    // Mover la carta seleccionada al tablero
    cartaTablero->setText(playerHands[currentPlayerIndex][cardIndex]);

    // Eliminar la carta de la mano del jugador
    delete playerHandCards[cardIndex];
    playerHandCards.removeAt(cardIndex);
    playerHands[currentPlayerIndex].removeAt(cardIndex);

    // Actualizar la mano del jugador
    displayPlayerHand();

    // Finaliza el turno después de jugar la carta
    endTurn();
}

void MainWindow::updateGameScreen(const QString &tablero, QVector<QVector<QString>> &hands, int turno)
{
    cartaTablero->setText(tablero);
    playerHands = hands;
    currentPlayerIndex = turno;

    displayPlayerHand();
    startTurn();
}
