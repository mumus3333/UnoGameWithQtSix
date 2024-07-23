#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_newConnection();
    void on_readyRead();

private:
    QTextEdit *connectionsTextEdit;
    QLabel *statusLabel;
    QLabel *ipLabel;
    QTcpServer *server;
    QList<QTcpSocket *> clients;
};

#endif // MAINWINDOW_H
