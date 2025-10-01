#include "displayobject.h"

#include "displayitem.h"
#include "utility.h"

#include <QGraphicsScene>
#include <QTransform>
#include <QMatrix4x4>

#include <QGraphicsObject>

DisplayObject::DisplayObject(QObject* parent):
  QObject(parent),
  target_ {}
  { }

DisplayObject::DisplayObject(QGraphicsItem* target, QObject* parent):
  QObject(parent),
  target_ {target}
{
    useDeducedParent(parent);
    updateGeometry();
}

DisplayObject* DisplayObject::useDeducedParent(QObject* parent)
{
    if (!parent) { setParent(target_->scene()); }
    return this;
}

DisplayObject* DisplayObject::markDirty(bool dirty)
  { dirty_ |= dirty; return this; }

void DisplayObject::updateMatrix()
{
    matrix_.setToIdentity();
    matrix_.translate(posxyz().toVector3D());
    matrix_.rotate(float(rotx_), {1, 0, 0});
    matrix_.rotate(float(roty_), {0, 1, 0});
    matrix_.rotate(float(rotz_), {0, 0, 1});
    matrix_.scale(scalexyz_.toVector3D());
    matrix_ *= QTransform{}
          .shear(shear_.x(), shear_.y())
          .translate(-rot_center_abs_.x(), -rot_center_abs_.y());
    for (auto k: children()) {
        if (const auto p = qobject_cast<DisplayItem*>(k)) {
            p->markDirty(true);
        }
        else if (const auto p = qobject_cast<DisplayObject*>(k)) {
            p->markDirty(true);
        }
    }
}

void DisplayObject::updateTransform(bool dirty)
{
    if (!dirty_ && !dirty) { return; }
    updateMatrix();
    dirty_ = false;

    target_->setPos(foc_center_abs_);
    target_->setTransform(
        (parent_matrix() * matrix_).toTransform(1024)
          * QTransform{}.scale(foc_scale_.x(), foc_scale_.y()));
}

void DisplayObject::updateGeometry()
{
    using namespace Pointf_utility;
    if (target_) {
        const QRectF rect = target_->boundingRect();
        rot_center_abs_ = rect.topLeft() + rot_center_ * (rect.bottomRight() - rect.topLeft());
        if (const auto& scene = target_->scene()) {
            const QPointF scene_size = {scene->width(), scene->height()};
            foc_scale_ = QPointF{3./4, 1} * scene_size / 400 / tan(kRadPerDeg * fov_/2);
            foc_center_abs_ = foc_center_ * scene_size;
        }
        else {
            foc_scale_ = QPointF{1, 1} / tan(kRadPerDeg * fov_/2);
            foc_center_abs_ = rot_center_abs_;
        }
        markDirty(true);
    }
}

DisplayObject* DisplayObject::setTarget(QGraphicsItem* target)
{
    target_ = target;
    updateGeometry();
    return this;
}

DisplayObject* DisplayObject::setFov(const qreal& fov)
{
    fov_ = fov;
    updateGeometry();
    return this;
}
DisplayObject* DisplayObject::setFocCenter(const QPointF& foc_center)
  { foc_center_ = foc_center; return this; }

DisplayObject* DisplayObject::setScaleXYZ(const PointF3d& scalexyz)
  { scalexyz_ = scalexyz; return this; }
DisplayObject* DisplayObject::setPosXY(const QPointF& pos)
  { posxy_ = pos; return this; }
DisplayObject* DisplayObject::setShear(const QPointF& shear)
  { shear_ = shear; return this; }
DisplayObject* DisplayObject::setRotX(const qreal& rotx)
  { rotx_ = rotx; return this; }
DisplayObject* DisplayObject::setRotY(const qreal& roty)
  { roty_ = roty; return this; }
DisplayObject* DisplayObject::setRotZ(const qreal& rotz)
  { rotz_ = rotz; return this; }
DisplayObject* DisplayObject::setRotCenter(const QPointF& center)
  { rot_center_ = center; return this; }

DisplayObject* DisplayObject::setPosXYZ(const PointF3d& posxyz)
{
    posxy_ = posxyz.toPointF();
    target_->setZValue(posxyz.z());
    return this;
}
DisplayObject* DisplayObject::setRotXY(const QPointF& rotxy)
{
    rotx_ = rotxy.x();
    roty_ = rotxy.y();
    return this;
}
DisplayObject* DisplayObject::setRotXYZ(const PointF3d& rotxyz)
{
    rotx_ = rotxyz.x();
    roty_ = rotxyz.y();
    rotz_ = rotxyz.z();
    return this;
}

QGraphicsItem* DisplayObject::target()
  { return target_; }
const QGraphicsItem* DisplayObject::target() const
  { return target_; }

qreal DisplayObject::fov() const
  { return fov_; }
QPointF DisplayObject::foc_center() const
  { return foc_center_; }
PointF3d DisplayObject::scalexyz() const
  { return scalexyz_; }
QPointF DisplayObject::posxy() const
  { return posxy_; }
QPointF DisplayObject::shear() const
  { return shear_; }
qreal DisplayObject::rotx() const
  { return rotx_; }
qreal DisplayObject::roty() const
  { return roty_; }
qreal DisplayObject::rotz() const
  { return rotz_; }
QPointF DisplayObject::rot_center() const
  { return rot_center_; }

QMatrix4x4 DisplayObject::matrix() const
  { return matrix_; }

PointF3d DisplayObject::posxyz() const
  { return {posxy_, target_->zValue()}; }
QPointF DisplayObject::rotxy() const
  { return {rotx_, roty_}; }
PointF3d DisplayObject::rotxyz() const
  { return {rotx_, roty_, rotz_}; }

QMatrix4x4 DisplayObject::parent_matrix() const
{
    if (const auto pd = qobject_cast<DisplayItem*>(parent())) {
        if (const auto p = qobject_cast<DisplayObject*>(pd->target())) {
            return p->parent_matrix() * p->matrix_;
        }
    }
    if (const auto p = qobject_cast<DisplayObject*>(parent())) {
            return p->parent_matrix() * p->matrix_;
    }
    return {};
}

