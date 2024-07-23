#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

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
};

#endif // MAINWINDOW_H
