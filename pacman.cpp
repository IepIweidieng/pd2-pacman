#include "pacman.h"

#include "arrow3dcontroller.h"
#include "displayobject.h"
#include "pointf3d.h"
#include "utility.h"

#include <QGraphicsItem>
#include <QRandomGenerator>
#include <QTimer>

Pacman::Pacman(QObject* parent):
 Sprite(parent)
  { init(getGraphic(getDeducedParent(parent)), parent); }

Pacman::Pacman(QGraphicsItem* graphic_parent, QObject* parent):
 Sprite(graphic_parent, parent)
  { init(graphic_parent, parent); }

void Pacman::init(QGraphicsItem* graphic_parent, QObject* target_parent)
{
    setGraphicFunc([=](QGraphicsItem* parent)
      { return new QGraphicsRectItem({0, 0, 10, 10}, parent); });
    setCtrl(new Arrow3dController{
          {Qt::Key_Left, Qt::Key_Down, Qt::Key_Up, Qt::Key_Right,
           Qt::Key_End, Qt::Key_Home, Qt::Key_PageUp, Qt::Key_PageDown,},
          (parent()) ? getDeducedParent(parent()) : this});
    setHasDirect(true);
    setPosCtrl(Control_func::linear_move({ctrl()}));
    spawn();
    Sprite::init(graphic_parent, target_parent);
}

void Pacman::spawn()
{
    setPosUnit({2, 2, 2});
    setScale({1, 1, 1});
    setRotCtrl(nullptr);
    setRotUnit({1, 1, 1});
    if (const auto t = qobject_cast<DisplayObject*>(target())) {
        t->setRotXYZ({0, 0, 0});
    }
}

void Pacman::apoptosis()
{
    const auto& qRand = QRandomGenerator::global();
    setHasDirect(true);
    setPosUnit({0, 0, 0});
    if (const auto t = qobject_cast<DisplayObject*>(target())) {
        t->setRotXYZ({0, 0, 0});
    }
    setRotCtrl(Control_func::accelerate_move(
      {qRand->bounded(16.) - 8, qRand->bounded(16.) - 8, qRand->bounded(16.) - 8},
      [=]()  { return PointF3d{}; },
      Control_func::friction(0.2)));
    setScale({1, 1, 1});
    setScaleCtrl(Control_func::accelerate_move(
      {-.175, -.175, 0},
      [=]()  { return PointF3d{}; },
      Control_func::friction(0.01)));

    QTimer::singleShot(static_cast<int>(1.5 * 1000), [=]()
    {
        setScaleCtrl(nullptr);
        setRotCtrl(nullptr);
        setScale(3 * scale());
        markDirty();
    });
}
