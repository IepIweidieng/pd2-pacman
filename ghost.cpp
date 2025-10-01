#include "ghost.h"

#include "arrow3dcontroller.h"
#include "displayobject.h"
#include "pointf3d.h"
#include "utility.h"

#include <QGraphicsItem>
#include <QRandomGenerator>
#include <QTimer>

Ghost::Ghost(QObject* parent):
 Sprite(parent)
  { init(getGraphic(getDeducedParent(parent)), parent); }

Ghost::Ghost(QGraphicsItem* graphic_parent, QObject* parent):
 Sprite(graphic_parent, parent)
  { init(graphic_parent, parent); }

void Ghost::init(QGraphicsItem* graphic_parent, QObject* target_parent)
{
    setGraphicFunc([=](QGraphicsItem* parent)
      { return new QGraphicsEllipseItem({0, 0, 10, 10}, parent); });
    setCtrl(new Arrow3dController({},
          //{Qt::Key_A, Qt::Key_S, Qt::Key_W, Qt::Key_D,},
          (parent()) ? getDeducedParent(parent()) : this));
    setHasDirect(false);
    setPosCtrl(Control_func::linear_move({ctrl()}));
    spawn();
    Sprite::init(graphic_parent, target_parent);
}

void Ghost::spawn()
{
    setPosUnit({2, 2, 2});
    setScale({1, 1, 1});
    setRotCtrl(nullptr);
    setRotUnit({1, 1, 1});
    if (const auto t = qobject_cast<DisplayObject*>(target())) {
        t->setRotXYZ({0, 0, 0});
    }
}

void Ghost::apoptosis()
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
        setPosUnit({0, 0, 0});
        setScaleCtrl(nullptr);
        setRotCtrl(nullptr);
        setScale(3 * scale());
        markDirty();
        setUpdateFunc([=](DisplayItem* pself)
        {
            if (!qFuzzyIsNull(pself->pos().x())) {
                pself->setPos({copysign(std::max(0., abs(pself->pos().x()) - 2), pself->pos().x()), pself->pos().y(), pself->pos().z()});
            }
            else if (!qFuzzyIsNull(pself->pos().x())) {
                pself->setPos({pself->pos().x(), copysign(std::max(0., abs(pself->pos().y()) - 2), pself->pos().y()), pself->pos().z()});
            }
            else if (!qFuzzyIsNull(pself->pos().z())) {
                pself->setPos({pself->pos().x(), pself->pos().z(), copysign(std::max(0., abs(pself->pos().z()) - 2), pself->pos().z())});
            }
            else {
                pself->addOneshotUpdateFunc([=](DisplayItem* pself)
                {
                    pself->setUpdateFunc(nullptr);
                    spawn();
                });
            }
        });
    });
}
