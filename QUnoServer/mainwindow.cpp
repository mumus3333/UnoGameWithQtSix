#include "mainwindow.h"
#include <QNetworkInterface>
#include <QDataStream>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , server(new QTcpServer(this))
    , playerCount(0)
    , mazo(this)
    , currentPlayerIndex(0)
{
    // Crear widgets
    headerLabel = new QLabel("Numero del jugador | nombre | ip", this);
    playersTextEdit = new QTextEdit(this);
    playersTextEdit->setReadOnly(true);
    infoLabel = new QLabel("", this);
    startButton = new QPushButton("Start", this);

    // Crear layout
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(headerLabel);
    layout->addWidget(playersTextEdit);
    layout->addWidget(infoLabel);
    layout->addWidget(startButton, 0, Qt::AlignRight);

    // Crear widget central
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Conectar señales y slots
    connect(server, &QTcpServer::newConnection, this, &MainWindow::on_newConnection);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::on_startButton_clicked);

    // Iniciar el servidor
    if (server->listen(QHostAddress::Any, 1234)) {
        infoLabel->setText("Server started on port 1234");
    } else {
        infoLabel->setText("Server failed to start");
        qDebug() << "Error starting server:" << server->errorString();
    }

    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress::LocalHost) {
            infoLabel->setText(infoLabel->text() + "\nServer IP: " + address.toString());
        }
    }
}

MainWindow::~MainWindow()
{
    // Los widgets se eliminarán automáticamente
}

void MainWindow::on_newConnection() {
    if (playerCount >= 4) {
        QTcpSocket *extraClient = server->nextPendingConnection();
        extraClient->disconnectFromHost();
        extraClient->deleteLater();
        qDebug() << "Extra client tried to connect but game already has 4 players.";
        return;
    }

    QTcpSocket *clientSocket = server->nextPendingConnection();
    clients.append(clientSocket);
    playerCount++;
    connect(clientSocket, &QTcpSocket::readyRead, this, &MainWindow::on_readyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, [this, clientSocket]() {
        clients.removeOne(clientSocket);
        playerCount--;
        clientSocket->deleteLater();
        qDebug() << "Client disconnected. Remaining clients:" << clients.size();
    });

    // Leer el nombre del jugador
    QByteArray data = clientSocket->readAll();
    QString playerName = QString::fromUtf8(data);
    qDebug() << "New client connected:" << playerName;

    playerHands.append(mazo.repartirCartas(7)); // Repartir cartas al nuevo jugador
    QString playerInfoStr = QString("Player %1 | %2")
                                .arg(playerCount)
                                .arg(playerName);
    playerInfo[clientSocket] = playerInfoStr;
    playersTextEdit->append(playerInfoStr);
}



void MainWindow::on_readyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) {
        qDebug() << "Ready read called but sender is not a QTcpSocket.";
        return;
    }

    QByteArray data = clientSocket->readAll();
    processClientMessage(clientSocket, data);
}

void MainWindow::processClientMessage(QTcpSocket *clientSocket, const QByteArray &message) {
    QByteArray data = message; // Copiamos el mensaje a una variable no constante
    QDataStream in(&data, QIODevice::ReadOnly);
    
    int cardIndex;
    in >> cardIndex;

    if (cardIndex < 0 || cardIndex >= playerHands[currentPlayerIndex].size()) {
        qDebug() << "Invalid card index received:" << cardIndex;
        return;
    }

    // Actualizar el estado del juego: carta jugada y mano del jugador
    cartaTablero = playerHands[currentPlayerIndex][cardIndex];
    playerHands[currentPlayerIndex].removeAt(cardIndex);

    // Cambiar de turno
    currentPlayerIndex = (currentPlayerIndex + 1) % clients.size();

    // Sincronizar con los clientes
    broadcastGameState();
}


void MainWindow::on_startButton_clicked()
{
    if (playerCount < 2) {
        infoLabel->setText("Need at least 2 players to start the game!");
        qDebug() << "Attempted to start game with less than 2 players.";
        return;
    }
    infoLabel->setText("Game started with " + QString::number(playerCount) + " players!");
    cartaTablero = mazo.tomarCarta();
    broadcastGameState();
}

void MainWindow::broadcastGameState() {
    QByteArray gameState;
    QDataStream out(&gameState, QIODevice::WriteOnly);
    out << cartaTablero; // Enviar la carta del tablero como QString
    out << playerHands;  // Enviar todas las manos de los jugadores
    out << currentPlayerIndex; // Enviar el turno actual

    for (QTcpSocket *client : clients) {
        if (client->state() == QAbstractSocket::ConnectedState) {
            client->write(gameState);
            client->flush();
        }
    }
}

//lll

