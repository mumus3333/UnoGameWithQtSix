#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>
#include <QMap>
#include "mazo.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_newConnection();
    void on_readyRead();
    void on_startButton_clicked();

private:
    QTextEdit *playersTextEdit;
    QLabel *infoLabel;
    QLabel *headerLabel;
    QPushButton *startButton;
    QTcpServer *server;
    QVector<QTcpSocket*> clients;
    QMap<QTcpSocket*, QString> playerInfo;
    int playerCount;

    // Estado del juego
    Mazo mazo;
    QLabel *cartaTablero;
    QVector<QVector<QString>> playerHands; // Almacena las cartas de cada jugador
    int currentPlayerIndex;

    void broadcastGameState();
    void processClientMessage(QTcpSocket *clientSocket, const QByteArray &message);
};

#endif // MAINWINDOW_H
