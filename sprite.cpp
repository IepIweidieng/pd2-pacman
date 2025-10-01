#include "sprite.h"

#include "displayobject.h"
#include "pointf3d.h"
#include "utility.h"

Sprite::Sprite(QObject* parent):
  MovableItem(getDeducedParent(parent))
{
    setParent(getDeducedParent(parent));
    init(getGraphic(getDeducedParent(parent)), parent);
}

Sprite::Sprite(QGraphicsItem* graphic_parent, QObject* parent):
  MovableItem(graphic_parent, parent)
{
    init(graphic_parent, parent);
}

void Sprite::spawn()  { }
void Sprite::apoptosis()  { }

void Sprite::init(QGraphicsItem* graphic_parent, QObject* target_parent)
{
    markDirty();
    if (graphic_parent) { initGraphic(graphic_parent, target_parent); }
}

Sprite* Sprite::bindCtrlToObject(QObject* obj)
{
    if (ctrl_) { ctrl_->bindToObject(obj); }
    return this;
}

void Sprite::initGraphic(QGraphicsItem* graphic_parent, QObject* target_parent)
{
    if (const auto t = graphic()) {
        if (t != graphic_parent) {
            t->setParentItem(graphic_parent);
        }
    }
    else if (graphic_func_) {
        setTarget(graphic_func_(graphic_parent));
    }
    if (const auto t = target()) {
        if (t != target_parent) {
            t->setParent(target_parent);
        }
    }
}

Sprite* Sprite::setGraphicFunc(const Graphic_func_t& graphic_func)
  { graphic_func_ = graphic_func; return this; }
Sprite* Sprite::setCtrl(ArrowController* ctrl)
  { ctrl_ = ctrl; return this; }

auto Sprite::graphic_func() const -> Graphic_func_t
  { return graphic_func_; }
QPointer<ArrowController> Sprite::ctrl()
  { return ctrl_; }
