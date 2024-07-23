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
#include <QGroupBox>
#include <QVector>

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
    QWidget *gameScreen;
    QVector<QLabel*> playerLabels;
    void setupGameScreen(int playerCount);
};

#endif // MAINWINDOW_H
