#ifndef MOVABLEITEM_H
#define MOVABLEITEM_H

#include "displayitem.h"
#include "pointf3d.h"

#include <functional>

/** @brief The class whose instances handle the move information for `QObject`
*/
class MovableItem : public DisplayItem {
    Q_OBJECT

public:
    using Control_t = std::function<PointF3d(PointF3d value, PointF3d unit)>;

    using DisplayItem::DisplayItem;

    MovableItem* update(bool dirty = false) override;

    MovableItem* setHasDirect(bool has_direct);

    MovableItem* setScaleUnit(const PointF3d& scale_unit);
    MovableItem* setPosUnit(const PointF3d& pos_unit);
    MovableItem* setShearUnit(const QPointF& shear_unit);
    MovableItem* setRotUnit(const PointF3d& rot_unit);

    MovableItem* setScaleCtrl(Control_t scale_ctrl);
    MovableItem* setPosCtrl(Control_t pos_ctrl);
    MovableItem* setShearCtrl(Control_t shear_ctrl);
    MovableItem* setRotCtrl(Control_t rot_ctrl);

    bool has_direct() const;

    PointF3d scale_unit() const;
    PointF3d pos_unit() const;
    QPointF shear_unit() const;
    PointF3d rot_unit() const;

    Control_t pos_ctrl() const;
    Control_t scale_ctrl() const;
    Control_t shear_ctrl() const;
    Control_t rot_ctrl() const;

signals:

public slots:

private:
    bool has_direct_ {};

    PointF3d scale_unit_ {};
    PointF3d pos_unit_ {};
    QPointF shear_unit_ {};
    PointF3d rot_unit_ {};

    Control_t pos_ctrl_ {};
    Control_t scale_ctrl_ {};
    Control_t shear_ctrl_ {};
    Control_t rot_ctrl_ {};
};

#endif // MOVABLEITEM_H
