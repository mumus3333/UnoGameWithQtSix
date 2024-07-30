#include <QVBoxLayout>
#include <QFont>
#include "label.h"

Label::Label(QWidget *parent)
    : QWidget(parent) {

  QString rules = "UNO Rules:\n\
1. Each player is dealt 7 cards.\n\
2. The remaining cards form a draw pile.\n\
3. The top card of the draw pile is flipped to start the discard pile.\n\
4. Players take turns matching a card from their hand to the top card of the discard pile by color or number.\n\
5. Special action cards: Skip, Reverse, Draw Two, Wild, and Wild Draw Four.\n\
6. If a player cannot match a card, they must draw a card from the draw pile.\n\
7. When a player has one card left, they must shout 'UNO!'.\n\
8. The first player to get rid of all their cards wins the round.\n\
9. Points are scored based on the remaining cards in opponents' hands.";

  label = new QLabel(rules, this);
  label->setFont(QFont("Purisa", 10));

  auto *vbox = new QVBoxLayout();
  vbox->addWidget(label);
  setLayout(vbox);
}
