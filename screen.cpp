#include "screen.h"

#include "controller.h"
#include "displayitem.h"
#include "displayobject.h"

#include <QTimer>
#include <QDebug>

static bool isDurationValid(qreal duration)
  { return !std::isnan(duration) && duration >= 0 && std::isfinite(duration); }

Screen::Screen(QObject* parent):
  QObject(parent),
  trans_timer_ {new QTimer(this)}
{
    // NOTE: Somehow setting up a `QStateMachine` here will not do the job;
    //    just use the plain signal connection instead for now. <IID 2019-08-30>
    trans_state_ = kInit;
    connect(this, &Screen::transitionInStarted, [=]()
    {
        trans_state_ = kIn;
        trans_duration_ = duration_in_;
        setTransTimer(&Screen::endTransitionIn);
    });
    connect(this, &Screen::transitionInEnded, [=]()
    {
        trans_state_ = kNorm;
        trans_duration_ = duration_;
        setTransTimer(&Screen::startTransitionOut);
    });
    connect(this, &Screen::transitionOutStarted, [=]()
    {
        trans_state_ = kOut;
        trans_duration_ = duration_out_;
        setTransTimer(&Screen::endTransitionOut);
    });
    connect(this, &Screen::transitionOutEnded, [=]()  { trans_state_ = kEnd; });

    trans_timer_->setSingleShot(true);
    trans_timer_->start();

    changeTransitionLevel(0);
}

Screen::~Screen()
  { delete graphic_; }

Screen* Screen::useDeducedParent()
{
    if (!parent()) { setParent(scene_); }
    return this;
}

void Screen::update()
{
    if (pause_) {
        const auto remaining = trans_timer_->remainingTime();
        if (remaining > 0) {
            trans_timer_->stop();
            trans_timer_->setInterval(remaining);
        }
        setCtrlEnabled(false);
    }
    else {
        if (!trans_timer_->isActive()) {
            trans_timer_->start();
            setCtrlEnabled(true);
        }
        if (trans_timer_->remainingTime() > 0) {
            const qreal rt = (trans_timer_->remainingTime()/1000.0) / trans_duration_;
            switch (trans_state_) {
              case kIn:
                changeTransitionLevel(1.0 - rt);
                break;
              case kOut:
                changeTransitionLevel(rt);
                break;
              case kInit: case kNorm: case kEnd:;
            }
        }
        for (const auto& item: children()) {
            if (const auto k = qobject_cast<DisplayItem*>(item)) {
                k->update();
            }
        }
    }
}

void Screen::updateGeometry()
{
    for (const auto& item: children()) {
        if (const auto k = qobject_cast<DisplayItem*>(item)) {
            k->syncWithTarget();
            if (const auto p = qobject_cast<DisplayObject*>(k->target())) {
                p->updateGeometry();
            }
        }
    }
}

void Screen::bindCtrl()
{
    for (const auto& item: children()) {
        if (const auto k = qobject_cast<Controller*>(item)) {
            k->bindToObject(widget_);
        }
    }
}
void Screen::unbindCtrl()
{
    for (const auto& item: children()) {
        if (const auto k = qobject_cast<Controller*>(item)) {
            k->unbindFromObject(widget_);
        }
    }
}
void Screen::setCtrlEnabled(bool enabled)
{
    for (const auto& item: children()) {
        if (const auto k = qobject_cast<Controller*>(item)) {
            k->setEnabled(enabled);
        }
    }
}

Screen* Screen::bindToScene(QGraphicsScene* scene)
{
    if (!parent()) { setParent(scene); }
    scene_ = scene;
    scene_->addItem(graphic_);
    return this;
}

Screen* Screen::bindToWidget(QWidget* widget)
{
    widget_ = widget;
    bindCtrl();
    return this;
}

Screen* Screen::setDurationIn(qreal duration_in)
  { duration_in_ = duration_in; return this; }
Screen* Screen::setDuration(qreal duration)
  { duration_ = duration; return this; }
Screen* Screen::setDurationOut(qreal duration_out)
  { duration_out_ = duration_out; return this; }
Screen* Screen::setPause(bool pause)
  { pause_ = pause; return this; }

Screen* Screen::setDurationInOut(qreal duration_in, qreal duration_out)
{
    duration_in_ = duration_in;
    duration_out_ = duration_out;
    return this;
}

QGraphicsItemGroup* Screen::graphic() const
  { return graphic_; }
QPointer<QGraphicsScene> Screen::scene() const
  { return scene_; }
QPointer<QWidget> Screen::widget() const
  { return widget_; }

auto Screen::trans_state() const -> TransState
  { return trans_state_; }
qreal Screen::trans_level() const
  { return trans_level_; }

qreal Screen::duration_in() const
  { return duration_in_; }
qreal Screen::duration() const
  { return duration_; }
qreal Screen::duration_out() const
  { return duration_out_; }
bool Screen::is_paused() const
  { return pause_; }

void Screen::changeTransitionLevel(qreal level)
{
    if (!qFuzzyCompare(level, trans_level_)) {
        switch (trans_state_) {
          case kIn:
            if (!(level > trans_level_)) { return; }
            break;
          case kOut:
            if (!(level < trans_level_)) { return; }
            break;
          case kInit: case kNorm: case kEnd:;
        }
        trans_level_ = level;
        updateTransitionLevel(level);
        emit TransitionLevelChanged(level);
    }
}
void Screen::startTransitionIn()
  { if (trans_state_ < kIn) { initIn(); emit transitionInStarted(); } }
void Screen::endTransitionIn()
{
    changeTransitionLevel(1);
    if (trans_state_ < kNorm) { initNorm(); emit transitionInEnded(); }
}
void Screen::startTransitionOut()
  { if (trans_state_ < kOut) { initOut(); emit transitionOutStarted(); } }
void Screen::endTransitionOut()
{
    changeTransitionLevel(0);
    if (trans_state_ < kEnd) { initEnd(); emit transitionOutEnded(); }
}

void Screen::updateTransitionLevel(qreal)  { }
void Screen::initIn()  { }
void Screen::initNorm()  { }
void Screen::initOut()  { }
void Screen::initEnd()  { }

void Screen::setTransTimer(Slot_func_t func)
{
    trans_timer_->disconnect();
    if (isDurationValid(trans_duration_)) {
        qreal duration = trans_duration_;
        switch (trans_state_) {
          case kIn:
            duration *= 1.0 - trans_level_;
            break;
          case kOut:
            duration *= trans_level_;
            break;
          case kInit: case kNorm: case kEnd:;
        }
        trans_timer_->setInterval(
          std::min(std::max(
              static_cast<int>(std::round(duration * 1000)), 0), static_cast<int>(trans_duration_ * 1000)));
        trans_timer_->setSingleShot(true);
        trans_timer_->callOnTimeout([=]()  { (this->*func)(); });
    }
}
