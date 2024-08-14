#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QVector>
#include <QMessageBox>
#include "mazo.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();
    void on_connected();
    void on_readyRead();
    void showRulesScreen();
    void setupGameScreen(int playerCount);
    void displayPlayerHand();
    void playerAction(); // Ejemplo de una acción que realiza el jugador
    void playCard(int cardIndex); // Función para jugar una carta específica

private:
    QTcpSocket *socket;
    QLineEdit *serverIpLineEdit;
    QLineEdit *playerNameLineEdit;
    QPushButton *connectButton;
    QLabel *statusLabel;
    QWidget *connectScreen;
    QStackedWidget *stackedWidget;
    QWidget *rulesScreen;
    QWidget *gameScreen;

    Mazo *mazo; // Instancia de la clase Mazo
    QVector<QVector<QLabel*>> playerHands; // Almacena las cartas de cada jugador
    QVector<QLabel*> playerLabels; // Almacena las etiquetas de los jugadores
    QVector<QLabel*> turnLabels; // Vector para almacenar los QLabel de turnos
    QWidget *playerHandWidget; // Widget para mostrar las cartas del jugador
    QVector<QPushButton*> playerHandCards; // Etiquetas de las cartas del jugador
    QLabel *cartaTablero; // Carta inicial en el tablero

    // Botones de acción
    QPushButton *playCardButton;  // Botón para jugar una carta
    QPushButton *passTurnButton;  // Botón para pasar el turno

    // Variables para manejar los turnos
    int currentPlayerIndex;  // Índice del jugador cuyo turno es actualmente
    int totalPlayers;        // Número total de jugadores

    // Métodos para manejar los turnos
    void startTurn();  // Inicia el turno del jugador actual
    void endTurn();    // Finaliza el turno del jugador actual y pasa al siguiente
};

#endif // MAINWINDOW_H

