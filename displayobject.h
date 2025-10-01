#ifndef DISPLAYOBJECT_H
#define DISPLAYOBJECT_H

#include "pointf3d.h"

#include <QGraphicsObject>
#include <QMatrix4x4>

#include <cmath>

/** @brief A wrapper class derived from `QObject` for `QGraphicItem`
*/
class DisplayObject : public QObject {
    Q_OBJECT
    Q_PROPERTY(QGraphicsItem* target READ target WRITE setTarget NOTIFY targetChanged DESIGNABLE false)
    Q_PROPERTY(qreal fov READ fov WRITE setFov NOTIFY fovChanged FINAL)
    Q_PROPERTY(qreal rotx READ rotx WRITE setRotX NOTIFY rotXChanged FINAL)
    Q_PROPERTY(qreal roty READ roty WRITE setRotY NOTIFY rotYChanged FINAL)
    Q_PROPERTY(qreal rotz READ rotz WRITE setRotZ NOTIFY rotZChanged FINAL)
    Q_PROPERTY(QPointF foc_center READ foc_center WRITE setFocCenter NOTIFY focCenterChanged)
    Q_PROPERTY(QPointF rot_center READ rot_center WRITE setRotCenter NOTIFY rotCenterChanged)
    Q_PROPERTY(QPointF posxy READ posxy WRITE setPosXY STORED false NOTIFY posXYChanged)
    Q_PROPERTY(QPointF rotxy READ rotxy WRITE setRotXY STORED false NOTIFY rotXYChanged)
    Q_PROPERTY(QPointF shear READ shear WRITE setShear NOTIFY shearChanged)
    Q_PROPERTY(PointF3d scalexyz READ scalexyz WRITE setScaleXYZ NOTIFY scaleXYZChanged)
    Q_PROPERTY(PointF3d posxyz READ posxyz WRITE setPosXYZ STORED false NOTIFY posXYZChanged)
    Q_PROPERTY(PointF3d rotxyz READ rotxyz WRITE setRotXYZ STORED false NOTIFY rotXYZChanged)

public:
    explicit DisplayObject(QObject* parent = nullptr);
    DisplayObject(QGraphicsItem* target, QObject* parent = nullptr);

    DisplayObject* useDeducedParent(QObject* parent);

    void updateTransform(bool dirty = false);
    void updateGeometry();

    DisplayObject* markDirty(bool dirty = true);

    DisplayObject* setTarget(QGraphicsItem* target);

    DisplayObject* setFov(const qreal& fov);
    DisplayObject* setFocCenter(const QPointF& foc_center);
    DisplayObject* setScaleXYZ(const PointF3d& scalexyz);
    DisplayObject* setPosXY(const QPointF& posxy);
    DisplayObject* setShear(const QPointF& shear);
    DisplayObject* setRotX(const qreal& rotx);
    DisplayObject* setRotY(const qreal& roty);
    DisplayObject* setRotZ(const qreal& rotz);
    DisplayObject* setRotCenter(const QPointF& rot_center);

    DisplayObject* setPosXYZ(const PointF3d& posxyz);
    DisplayObject* setRotXY(const QPointF& rotxy);
    DisplayObject* setRotXYZ(const PointF3d& rotxyz);

    QGraphicsItem* target();
    const QGraphicsItem* target() const;

    qreal fov() const;
    QPointF foc_center() const;
    PointF3d scalexyz() const;
    QPointF posxy() const;
    QPointF shear() const;
    qreal rotx() const;
    qreal roty() const;
    qreal rotz() const;
    QPointF rot_center() const;

    QMatrix4x4 matrix() const;

    PointF3d posxyz() const;
    QPointF rotxy() const;
    PointF3d rotxyz() const;

    QMatrix4x4 parent_matrix() const;

    static constexpr double kRadPerDeg {M_PI/180};

signals:
    void targetChanged();
    void fovChanged();
    void scaleXYZChanged();
    void posXYChanged();
    void posXYZChanged();
    void shearChanged();
    void rotXChanged();
    void rotYChanged();
    void rotZChanged();
    void rotXYChanged();
    void rotXYZChanged();
    void rotCenterChanged();
    void focCenterChanged();

public slots:

protected:
    void updateMatrix();

private:
    QGraphicsItem* target_;

    qreal fov_ {45};
    QPointF foc_center_ {0.5, 0.5};
    PointF3d scalexyz_ {1, 1, 1};
    QPointF posxy_ {};
    QPointF shear_ {};
    qreal rotx_ {};
    qreal roty_ {};
    qreal rotz_ {};
    QPointF rot_center_ {0.5, 0.5};

private: private:
    bool dirty_ {};
    QPointF foc_scale_ {};
    QPointF foc_center_abs_ {};
    QPointF rot_center_abs_ {};
    QMatrix4x4 matrix_ {};
};

#endif // DISPLAYOBJECT_H
