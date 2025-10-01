#ifndef SPRITE_H
#define SPRITE_H

#include "movableitem.h"

#include "arrow3dcontroller.h"

#include <QGraphicsItem>
#include <QGraphicsView>
#include <QPointer>

#include <functional>

/** @brief A `DisplayItem` with its controller and graphic integrated
*/
class Sprite : public MovableItem {
    Q_OBJECT

public:
    using Graphic_func_t = std::function<QGraphicsItem*(QGraphicsItem* graphic_parent)>;

    explicit Sprite(QObject* parent = nullptr);
    Sprite(QGraphicsItem* graphic_parent, QObject* parent = nullptr);

    virtual void spawn();
    virtual void apoptosis();

    Sprite* bindCtrlToObject(QObject* obj);

    Sprite* setGraphicFunc(const Graphic_func_t& graphic_func);
    Sprite* setCtrl(ArrowController* ctrl);

    Graphic_func_t graphic_func() const;

    QPointer<ArrowController> ctrl();

signals:

public slots:

protected:
    virtual void init(QGraphicsItem* graphic_parent, QObject* target_parent);
    void initGraphic(QGraphicsItem* graphic_parent, QObject* target_parent);

private:
    Graphic_func_t graphic_func_ {};
    QPointer<ArrowController> ctrl_ {};
};

#endif // SPRITE_H
