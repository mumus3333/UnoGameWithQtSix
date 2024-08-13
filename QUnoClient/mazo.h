#ifndef MAZO_H
#define MAZO_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QRandomGenerator>

class Mazo : public QWidget
{
    Q_OBJECT

public:
    explicit Mazo(QWidget *parent = nullptr);

    // Función para asignar cartas a un jugador
    QVector<QString> repartirCartas(int cantidadCartas);

    // Función para tomar una carta del mazo
    QString tomarCarta();

private:
    QVector<QString> cartas; // Vector para almacenar las cartas como texto
    void cargarCartas(); // Cargar las cartas al inicio
};

#endif // MAZO_H
