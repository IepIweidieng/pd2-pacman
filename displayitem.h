#ifndef DISPLAYITEM_H
#define DISPLAYITEM_H

#include "pointf3d.h"

#include <QGraphicsItem>
#include <QObject>
#include <QPointer>
#include <QPointF>
#include <QQueue>
#include <QSizeF>

#include <functional>

#include <cmath>

/** @brief The base class whose instances handle the display information for `QObject`
*/
class DisplayItem : public QObject {
    Q_OBJECT

public:
    static QPointer<QObject> getDeducedParent(QObject* parent);
    static QGraphicsItem* getGraphic(QObject* obj);

    using Update_func_t = std::function<void(DisplayItem* pself)>;
    explicit DisplayItem(QObject* parent = nullptr);
    DisplayItem(QObject* target, QObject* parent);
    DisplayItem(QGraphicsItem* target, QObject* parent = nullptr);

    DisplayItem* useDeducedParent(QObject* parent);

    virtual DisplayItem* update(bool dirty = false);
    void syncWithTarget();

    DisplayItem* markDirty(bool dirty = true);

    DisplayItem* setUpdateFunc(const Update_func_t& update_func);
    DisplayItem* addOneshotUpdateFunc(const Update_func_t& oneshot_func_queue);

    DisplayItem* setTarget(QObject* target);
    DisplayItem* setTarget(QGraphicsItem* target, QObject* target_parent = nullptr);
    DisplayItem* setBaseSize(const QSizeF& base_size);
    DisplayItem* setScale(const PointF3d& scale);
    DisplayItem* setPos(const PointF3d& pos);

    const Update_func_t& update_func() const;
    const QQueue<Update_func_t>& oneshot_func_queue() const;
    QPointer<QObject> target();
    const QPointer<QObject> target() const;
    QSizeF base_size() const;
    PointF3d scale() const;
    PointF3d pos() const;

    QGraphicsItem* graphic() const;

signals:

public slots:

private:
    Update_func_t update_func_ {};
    QQueue<Update_func_t> oneshot_func_queue_ {};

    QPointer<QObject> target_;
    bool dirty_ {};

    QSizeF base_size_ {};
    PointF3d scale_ {1, 1, 1};
    PointF3d pos_ {};
};

#endif // DISPLAYITEM_H
