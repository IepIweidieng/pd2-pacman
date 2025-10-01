#include "movableitem.h"

#include "displayobject.h"

#include <cmath>

MovableItem* MovableItem::update(bool dirty)
{
    markDirty(dirty);
    if (scale_ctrl_) {
        const PointF3d new_scale = scale_ctrl_(scale(), scale_unit_);
        markDirty(new_scale != scale());
        setScale(new_scale);
    }
    if (pos_ctrl_) {
        const PointF3d new_pos = pos_ctrl_(pos(), pos_unit_);
        const QPointF vec = new_pos.toPointF() - pos().toPointF();
        markDirty(new_pos != pos());
        setPos(new_pos);
        if (has_direct_ && !vec.isNull()) {
            if (const auto p = qobject_cast<DisplayObject*>(target())) {
                p->setRotZ(atan2(vec.y(), vec.x()) / DisplayObject::kRadPerDeg);
            }
            else if (const auto p = qobject_cast<QGraphicsObject*>(target())) {
                p->setRotation(atan2(vec.y(), vec.x()) / DisplayObject::kRadPerDeg);
            }
        }
    }
    if (const auto p = qobject_cast<DisplayObject*>(target())) {
        if (shear_ctrl_) {
            const QPointF new_shear = scale_ctrl_(scale(), scale_unit_).toPointF();
            markDirty(new_shear != p->shear());
            p->setShear(new_shear);
        }
        if (rot_ctrl_) {
            const PointF3d new_rotxyz = rot_ctrl_(p->rotxyz(), rot_unit_);
            markDirty(new_rotxyz != p->rotxyz());
            p->setRotXYZ(new_rotxyz);
        }
    }
    return qobject_cast<MovableItem*>(DisplayItem::update(dirty));
}

MovableItem* MovableItem::setHasDirect(bool has_direct)
  { has_direct_ = has_direct; return this; }

MovableItem* MovableItem::setScaleUnit(const PointF3d& scale_unit)
  { scale_unit_ = scale_unit; return this; }
MovableItem* MovableItem::setPosUnit(const PointF3d& pos_unit)
  { pos_unit_ = pos_unit; return this; }
MovableItem* MovableItem::setShearUnit(const QPointF& shear_unit)
  { shear_unit_ = shear_unit; return this; }
MovableItem* MovableItem::setRotUnit(const PointF3d& rot_unit)
  { rot_unit_ = rot_unit; return this; }

MovableItem* MovableItem::setScaleCtrl(Control_t scale_ctrl)
  { scale_ctrl_ = scale_ctrl; return this; }
MovableItem* MovableItem::setPosCtrl(Control_t pos_ctrl)
  { pos_ctrl_ = pos_ctrl; return this; }
MovableItem* MovableItem::setShearCtrl(Control_t shear_ctrl)
  { shear_ctrl_ = shear_ctrl; return this; }
MovableItem* MovableItem::setRotCtrl(Control_t rot_ctrl)
  { rot_ctrl_ = rot_ctrl; return this; }

bool MovableItem::has_direct() const
  { return has_direct_; }

PointF3d MovableItem::scale_unit() const
  { return scale_unit_; }
PointF3d MovableItem::pos_unit() const
  { return pos_unit_; }
QPointF MovableItem::shear_unit() const
  { return shear_unit_; }
PointF3d MovableItem::rot_unit() const
  { return rot_unit_; }

MovableItem::Control_t MovableItem::scale_ctrl() const
  { return scale_ctrl_; }
MovableItem::Control_t MovableItem::pos_ctrl() const
  { return pos_ctrl_; }
MovableItem::Control_t MovableItem::shear_ctrl() const
  { return shear_ctrl_; }
MovableItem::Control_t MovableItem::rot_ctrl() const
  { return rot_ctrl_; }
