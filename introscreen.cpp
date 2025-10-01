#include "introscreen.h"

#include "arrow3dcontroller.h"
#include "displayitem.h"
#include "movableitem.h"
#include "utility.h"

#include <QWidget>
#include <QGuiApplication>
#include <QScreen>
#include <QTimer>
#include <QDebug>

using namespace Pointf_utility;
using namespace Control_func;

IntroScreen::IntroScreen(QObject* parent):
  Screen(parent)
{
    setDurationInOut(2, 2);
    setDuration(10);
}

void IntroScreen::updateTransitionLevel(qreal level)
{
    if (foreground_) {
        foreground_->graphic()->setOpacity(1.0 - level);
    }
}

void IntroScreen::initIn()
{
    const auto pos_ctrl = new Arrow3dController(
          {Qt::Key_Left, Qt::Key_Down, Qt::Key_Up, Qt::Key_Right,
           Qt::Key_End, Qt::Key_Home, Qt::Key_PageUp, Qt::Key_PageDown,
           Qt::Key_Shift, Qt::Key_Slash,}, this);
    const auto rot_ctrl = new Arrow3dController(
          {Qt::Key_A, Qt::Key_S, Qt::Key_W, Qt::Key_D,
           0, 0, 0, 0,
           Qt::Key_Q, Qt::Key_E,}, this);
    const auto rect_rot_ctrl = new Arrow3dController(
          {Qt::Key_8, Qt::Key_4, Qt::Key_6, Qt::Key_2,
           Qt::Key_7, Qt::Key_9, Qt::Key_3, Qt::Key_1,
           Qt::Key_Minus, Qt::Key_Plus,}, this);
    const auto win_pos_ctrl = new ArrowController(
          {Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L,}, this);
    const auto win_scale_ctrl = new ArrowController(
          {Qt::Key_N, Qt::Key_M, Qt::Key_Comma, Qt::Key_Period,}, this);

    meta_ctrl_ = new Controller({Qt::Key_Escape,}, this);

    const auto origin = (new MovableItem(new QGraphicsRectItem{0, 0, 0, 0}, this))
        ->setRotCtrl(linear_move([=]()
            { return PointF3d{0.5, 0.5, 0.5} + rot_ctrl->getValue(); }))
        ->setRotUnit({1, 1, 1});

    player_ = (new MovableItem(new QGraphicsRectItem{0, 0, 0, 0}, origin))
        ->setPosCtrl(linear_move({pos_ctrl}))
        ->setPosUnit({10, 10, 10})
        ->setScaleUnit({.05, .05, .05});
    player_->setScale({1e-9, 1e-9, 1e-9});

    const int rect_cb = 3;
    for (int x = -rect_cb; x <= rect_cb; ++x) {
    for (int y = -rect_cb; y <= rect_cb; ++y) {
    for (int z = -rect_cb; z <= rect_cb; ++z) {
        (new MovableItem(new QGraphicsRectItem{0, 0, 10, 10}, player_))
            ->setRotCtrl(linear_move([=]()
                { return PointF3d{0.2, 0.2, 0.4} + rect_rot_ctrl->getValue(); }))
            ->setRotUnit({2, 2, 2})
            ->setScaleCtrl([=](...)  { return 10/player_->scale(); })
            ->setPos({100.*x, 100.*y, 100.*z})
            ->markDirty();
    }}}

    (new MovableItem(qobject_cast<QWidget*>(parent()), this))
        ->setPosCtrl([=]()
        {
            int t = 0;
            const auto get_move = accelerate_move(ArrowFunc{win_pos_ctrl}, friction(0.8));
            return [=](PointF3d pos, PointF3d pos_unit) mutable
            {
                auto calc_pos = [](int t)
                  { return 0 /** 1./sin(t/(2*M_PI)/10)*/ * QPointF{cos(t/20. * 6/M_PI + 1./4 * 2*M_PI*t), sin(t/20. * 6/M_PI + 1./4 * 2*M_PI*t)}; };
                ++t;
                return bound_rebound({0, 0},
                    (get_move(pos - calc_pos(t-1), pos_unit) + calc_pos(t)).toPointF(),
                    toPointF(QGuiApplication::screenAt(pos.toPoint())->virtualSize()
                        - qobject_cast<QWidget*>(parent())->size()));
            };
        }())
        ->setScaleCtrl(linear_move({win_scale_ctrl}))
        ->setScaleUnit({0.1, 0.1, 0})
        ->setPosUnit({10, 10, 0})
        ->setPos({200, 200, 0})
        ->markDirty();

    const auto foreground = new QGraphicsRectItem{{{0, 0}, this->widget()->size()}};
    foreground->setBrush(Qt::black);
    foreground->update();
    foreground->setOpacity(1.0);
    foreground_ = new DisplayItem(foreground, this);

    connect(meta_ctrl_, &Controller::keyPressed, this, [=](int key_index)
      { if (key_index == 0) { startTransitionOut(); } });
}

void IntroScreen::initNorm()
{
    if (player_) {
        const auto scale_ctrl = new ArrowController(
              {Qt::Key_Z, 0, 0, Qt::Key_X,}, this);
        player_->setScaleCtrl(accelerate_move({.2, .2, .2},
            scaleUnit<PointF3d, qreal>([=]() { return scale_ctrl->getValue().x(); }),
            friction(.005)));
    }
}

void IntroScreen::initOut()
{
    if (meta_ctrl_) {
        meta_ctrl_->disconnect();
        connect(meta_ctrl_, &Controller::keyPressed, this, [=](int key_index)
          { if (key_index == 0) { endTransitionOut(); } });
    }
}

void IntroScreen::initEnd()  { }
