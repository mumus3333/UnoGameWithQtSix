#include "mazo.h"

Mazo::Mazo(QWidget *parent)
    : QWidget(parent)
{
    cargarCartas(); // Cargar todas las cartas en el vector al inicializar el mazo
}

void Mazo::cargarCartas()
{
    QStringList colores = {"Red", "Blue", "Green", "Yellow"};
    QStringList numeros = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "Skip", "Reverse", "Draw2"};
    QStringList especiales = {"Wild", "Wild Draw4"};

    // Añadir cartas numeradas y de acción de colores, excluyendo "Draw2" (comer cartas)
    for (const QString &color : colores) {
        for (const QString &numero : numeros) {
            if (numero != "Draw2") {
                cartas.append(color + " " + numero);
            }
        }
    }

    // Añadir cartas especiales (comodines)
    for (const QString &especial : especiales) {
        cartas.append(especial);
    }
}

QVector<QLabel*> Mazo::repartirCartas(int cantidadCartas)
{
    QVector<QLabel*> mano;
    for (int i = 0; i < cantidadCartas; ++i) {
        int index = QRandomGenerator::global()->bounded(cartas.size());
        QLabel *carta = new QLabel(cartas[index], this);
        mano.append(carta);
        cartas.remove(index); // Eliminar la carta del mazo después de repartirla
    }
    return mano;
}

QLabel* Mazo::tomarCarta()
{
    if (cartas.isEmpty()) {
        return nullptr; // Si el mazo está vacío, no se puede tomar carta
    }
    int index = QRandomGenerator::global()->bounded(cartas.size());
    QLabel *carta = new QLabel(cartas[index], this);
    cartas.remove(index); // Eliminar la carta del mazo después de tomarla
    return carta;
}

