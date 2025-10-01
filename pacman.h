#ifndef PACMAN_H
#define PACMAN_H

#include "sprite.h"

class Pacman : public Sprite {
    Q_OBJECT

public:
    explicit Pacman(QObject* parent = nullptr);
    Pacman(QGraphicsItem* graphic_parent, QObject* parent = nullptr);

    void spawn() override;
    void apoptosis() override;

signals:

public slots:

protected:
    void init(QGraphicsItem* graphic_parent, QObject* target_parent) override;

private:
};

#endif // PACMAN_H
