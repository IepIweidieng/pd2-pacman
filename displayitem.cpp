#include "displayitem.h"

#include "displayobject.h"
#include "screen.h"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWidget>
#include <QDebug>

DisplayItem::DisplayItem(QObject* parent):
  QObject(parent),
  target_ {}
  { }

DisplayItem::DisplayItem(QObject* target, QObject* parent):
  QObject(parent),
  target_ {target}
{
    useDeducedParent(parent);
    syncWithTarget();
}

DisplayItem::DisplayItem(QGraphicsItem* target, QObject* parent):
  QObject(parent),
  target_ {new DisplayObject{target}}
{
    useDeducedParent(parent);
    syncWithTarget();
}

QPointer<QObject> DisplayItem::getDeducedParent(QObject* parent)
{
    auto p = qobject_cast<Screen*>(parent);
    if (!p) {
        if (const auto pd = qobject_cast<DisplayItem*>(parent)) {
            p = qobject_cast<Screen*>(pd->parent());
        }
    }
    if (p) { return p; }
    return parent;
}

QGraphicsItem* DisplayItem::getGraphic(QObject* obj)
{
    if (const auto p = qobject_cast<Screen*>(obj)) {
        return p->graphic();
    }
    if (const auto p = qobject_cast<DisplayItem*>(obj)) {
        return p->graphic();
    }
    if (const auto p = qobject_cast<DisplayObject*>(obj)) {
        return p->target();
    }
    return nullptr;
}

DisplayItem* DisplayItem::useDeducedParent(QObject* parent)
{
    if (parent) {
        if (const auto p = qobject_cast<Screen*>(getDeducedParent(parent))) {
            setParent(p);
            if (const auto t = qobject_cast<DisplayObject*>(target_)) {
                if (!t->parent()) { t->setParent(parent); }
                if (const auto tt = t->target()) {
                    if (p->graphic() != t->target()) {
                        t->target()->setParentItem(p->graphic());
                    }
                    t->updateGeometry();
                }
            }
        }
    }
    else { setParent(target_); }
    return this;
}

DisplayItem* DisplayItem::update(bool dirty)
{
    markDirty(dirty);
    if (update_func_) { update_func_(this); }
    while (!oneshot_func_queue_.empty()) {
        oneshot_func_queue_.dequeue()(this);
    }
    if (const auto t = qobject_cast<DisplayObject*>(target_)) {
        t->setPosXYZ(pos_);
        t->setScaleXYZ(scale_);
        t->updateTransform(dirty_);
        for (auto k: children()) {
            if (const auto p = qobject_cast<DisplayItem*>(k)) {
                p->markDirty(dirty_);
            }
            else if (const auto p = qobject_cast<DisplayObject*>(k)) {
                p->markDirty(dirty_);
            }
        }
        dirty_ = false;
        return this;
    }
    if (const auto t = qobject_cast<QWidget*>(target_)) {
        t->move(pos_.toPoint());
        t->resize(qRound(scale_.x() * base_size_.width()),
                  qRound(scale_.y() * base_size_.height()));
        dirty_ = false;
        return this;
    }
    return this;
}

void DisplayItem::syncWithTarget()
{
    markDirty();
    if (const auto t = qobject_cast<DisplayObject*>(target_)) {
        base_size_ = t->target()->shape().boundingRect().size();
        scale_ = t->scalexyz();
        pos_ = t->posxyz();
        return;
    }
    if (const auto t = qobject_cast<QWidget*>(target_)) {
        base_size_ = t->size();
        scale_ = {1, 1, 1};
        pos_ = {t->pos(), 0};
        return;
    }
}

DisplayItem* DisplayItem::markDirty(bool dirty)
  { dirty_ |= dirty; return this; }


DisplayItem* DisplayItem::setUpdateFunc(const Update_func_t& update_func)
  { update_func_ = update_func; return this; }
DisplayItem* DisplayItem::addOneshotUpdateFunc(const Update_func_t& oneshot_update_func)
  { oneshot_func_queue_ += oneshot_update_func; return this; }

DisplayItem* DisplayItem::setTarget(QObject* target)
{
    target_ = target;
    syncWithTarget();
    return this;
}
DisplayItem* DisplayItem::setTarget(QGraphicsItem* target, QObject* target_parent)
  { return setTarget(new DisplayObject(target, target_parent)); }
DisplayItem* DisplayItem::setBaseSize(const QSizeF& base_size)
  { base_size_ = base_size; return this; }
DisplayItem* DisplayItem::setScale(const PointF3d& scale)
  { scale_ = scale; return this; }
DisplayItem* DisplayItem::setPos(const PointF3d& pos)
  { pos_ = pos; return this; }

auto DisplayItem::update_func() const -> const Update_func_t&
  { return update_func_; }
auto DisplayItem::oneshot_func_queue() const -> const QQueue<Update_func_t>&
  { return oneshot_func_queue_; }
QPointer<QObject> DisplayItem::target()
  { return target_; }
const QPointer<QObject> DisplayItem::target() const
  { return target_; }
QSizeF DisplayItem::base_size() const
  { return base_size_; }
PointF3d DisplayItem::scale() const
  { return scale_; }
PointF3d DisplayItem::pos() const
  { return pos_; }

QGraphicsItem* DisplayItem::graphic() const
{
    if (const auto p = qobject_cast<DisplayObject*>(target_)) {
        return p->target();
    }
    return nullptr;
}
