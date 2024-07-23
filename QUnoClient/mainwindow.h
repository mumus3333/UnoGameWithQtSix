#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QGridLayout>
#include <QVector>
#include <QStackedWidget>
#include <QHBoxLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();
    void on_connected();
    void on_readyRead();

private:
    QLineEdit *serverIpLineEdit;
    QLineEdit *playerNameLineEdit;
    QPushButton *connectButton;
    QLabel *statusLabel;
    QTcpSocket *socket;
    QStackedWidget *stackedWidget;
    QWidget *connectScreen;
    QWidget *gameScreen;
    QVector<QLabel*> playerLabels;
    QVector<QLabel*> playerHandCards; // Cartas en la mano del jugador
    QWidget *playerHandWidget; // Contenedor para las cartas en la mano del jugador
    void setupGameScreen(int playerCount);
    void displayPlayerHand();
};

#endif // MAINWINDOW_H
