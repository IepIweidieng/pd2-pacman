#ifndef SCREEN_H
#define SCREEN_H

#include <QGraphicsItemGroup>
#include <QGraphicsScene>
#include <QObject>
#include <QPointer>
#include <QWidget>

#include <cmath>

/** @brief A class whose instances handles screen transitions, updating, and controlling
*/
class Screen : public QObject {
    Q_OBJECT

public:
    enum TransState {kInit, kIn, kNorm, kOut, kEnd};
    Q_ENUM(TransState)

    explicit Screen(QObject* parent = nullptr);
    ~Screen() override;

    Screen* useDeducedParent();

    virtual void update();
    virtual void updateGeometry();

    void bindCtrl();
    void unbindCtrl();
    void setCtrlEnabled(bool enabled);

    Screen* bindToScene(QGraphicsScene* scene);
    Screen* bindToWidget(QWidget* widget);

    Screen* setDurationIn(qreal duration_in);
    Screen* setDuration(qreal duration);
    Screen* setDurationOut(qreal duration_out);
    Screen* setPause(bool pause);

    Screen* setDurationInOut(qreal duration_in, qreal duration_out);

    QGraphicsItemGroup* graphic() const;
    QPointer<QGraphicsScene> scene() const;
    QPointer<QWidget> widget() const;

    TransState trans_state() const;
    qreal trans_level() const;

    qreal duration_in() const;
    qreal duration() const;
    qreal duration_out() const;

    bool is_paused() const;

signals:
    void TransitionLevelChanged(qreal level);
    void transitionInStarted();
    void transitionInEnded();
    void transitionOutStarted();
    void transitionOutEnded();

public slots:
    void changeTransitionLevel(qreal level);
    void startTransitionIn();
    void endTransitionIn();
    void startTransitionOut();
    void endTransitionOut();

protected:
    virtual void updateTransitionLevel(qreal level);
    virtual void initIn();
    virtual void initNorm();
    virtual void initOut();
    virtual void initEnd();

private:
    using Slot_func_t = void (Screen::*)();
    void setTransTimer(Slot_func_t func);

    QGraphicsItemGroup* graphic_ {new QGraphicsItemGroup};
    QPointer<QGraphicsScene> scene_ {};
    QPointer<QWidget> widget_ {};

    TransState trans_state_ {};
    qreal trans_level_ {};
    QTimer* trans_timer_;
    qreal trans_duration_ {};

    qreal duration_in_ {};
    qreal duration_ {static_cast<qreal>(INFINITY)};
    qreal duration_out_ {};

    bool pause_ {};
};

#endif // SCREEN_H
