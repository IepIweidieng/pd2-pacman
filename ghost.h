#ifndef GHOST_H
#define GHOST_H

#include "sprite.h"

class Ghost : public Sprite {
    Q_OBJECT

public:
    explicit Ghost (QObject* parent = nullptr);
    Ghost(QGraphicsItem* graphic_parent, QObject* parent = nullptr);

    void spawn() override;
    void apoptosis() override;

signals:

public slots:

protected:
    void init(QGraphicsItem* graphic_parent, QObject* target_parent) override;

private:
};

#endif // GHOST_H
