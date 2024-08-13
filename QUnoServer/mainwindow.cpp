#include "mainwindow.h"
#include <QNetworkInterface>

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

void MainWindow::on_newConnection()
{
    QTcpSocket *clientSocket = server->nextPendingConnection();
    clients.append(clientSocket);
    playerCount++;
    connect(clientSocket, &QTcpSocket::readyRead, this, &MainWindow::on_readyRead);

    // Repartir cartas al nuevo jugador
    playerHands.append(mazo.repartirCartas(7));
    QString playerInfoStr = QString("Player %1 | %2")
                                .arg(playerCount)
                                .arg(clientSocket->peerAddress().toString());
    playerInfo[clientSocket] = playerInfoStr;
    playersTextEdit->append(playerInfoStr);

    // Iniciar el juego cuando todos los jugadores están conectados
    if (playerCount == 4) {
        cartaTablero = mazo.tomarCarta();
        broadcastGameState();
    }
}

void MainWindow::on_readyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    QByteArray data = clientSocket->readAll();
    processClientMessage(clientSocket, data);
}

void MainWindow::processClientMessage(QTcpSocket *clientSocket, const QByteArray &message)
{
    // Aquí se procesa el mensaje del cliente (por ejemplo, jugar una carta)
    // Actualizar el estado del juego y enviar la actualización a todos los clientes
    broadcastGameState();
}

void MainWindow::on_startButton_clicked()
{
    if (playerCount < 4) {
        infoLabel->setText("Need 4 players to start the game!");
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
    out << cartaTablero->text(); // Enviar la carta del tablero
    out << playerHands;          // Enviar todas las manos de los jugadores
    out << currentPlayerIndex;   // Enviar el turno actual

    for (QTcpSocket *client : clients) {
        client->write(gameState);
    }
}
