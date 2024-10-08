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

    qDebug() << "New client connected, awaiting player name...";
}



void MainWindow::on_readyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) {
        qDebug() << "Ready read called but sender is not a QTcpSocket.";
        return;
    }

    QByteArray data = clientSocket->readAll();
    qDebug() << "Received data from client:" << data;

    QDataStream in(&data, QIODevice::ReadOnly);
    
    QString tablero;
    QVector<QVector<QString>> hands;
    int turno;

    in >> tablero;
    in >> hands;
    in >> turno;

    qDebug() << "Tablero:" << tablero;
    qDebug() << "Manos de jugadores:" << hands;
    qDebug() << "Turno actual:" << turno;

    if (!tablero.isEmpty() && !hands.isEmpty()) {
        updateGameScreen(tablero, hands, turno);
    } else {
        qDebug() << "Invalid game state received!";
    }
}


void MainWindow::processClientMessage(QTcpSocket *clientSocket, const QByteArray &message) {
    QByteArray data = message;
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

void MainWindow::broadcastGameState()
{
    QByteArray gameState;
    QDataStream out(&gameState, QIODevice::WriteOnly);
    out << cartaTablero;
    out << playerHands;
    out << currentPlayerIndex;

    for (QTcpSocket *client : clients) {
        if (client->state() == QAbstractSocket::ConnectedState) {
            qint64 bytesWritten = client->write(gameState);
            if (bytesWritten == -1) {
                qDebug() << "Error writing to client:" << client->errorString();
            } else if (bytesWritten < gameState.size()) {
                qDebug() << "Only part of the game state was sent to the client.";
            } else {
                qDebug() << "Game state successfully sent to client:" << client->peerAddress().toString();
            }
        } else {
            qDebug() << "Client not in a connected state, unable to send game state.";
        }
    }
}

void MainWindow::updateGameScreen(const QString &tablero, const QVector<QVector<QString>> &hands, int turno)
{
    // Actualiza el tablero con la carta actual
    // Por ejemplo, si tienes un QLabel para mostrar la carta del tablero
    // QLabel *tableroLabel = new QLabel(this);  // Asume que tienes un QLabel para el tablero
    // tableroLabel->setText(tablero);

    // Actualiza las manos de los jugadores
    // Aquí puedes iterar sobre las manos de los jugadores y mostrar las cartas en la interfaz
    for (int i = 0; i < hands.size(); ++i) {
        // Suponiendo que tienes una manera de mostrar las cartas de cada jugador
        // Por ejemplo, podrías tener una lista de QTextEdit o QLabel para cada jugador
        qDebug() << "Player" << i + 1 << "hand:" << hands[i];
    }

    // Actualiza el turno actual
    // Aquí podrías actualizar un QLabel que muestra de quién es el turno
    qDebug() << "It's player" << turno + 1 << "'s turn.";
}




//lll

