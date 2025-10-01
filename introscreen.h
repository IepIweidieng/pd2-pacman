#ifndef INTROSCREEN_H
#define INTROSCREEN_H

#include "screen.h"

#include "controller.h"
#include "displayitem.h"
#include "movableitem.h"

class IntroScreen : public Screen
{
    Q_OBJECT

public:
    explicit IntroScreen(QObject* parent = nullptr);

protected:
    void updateTransitionLevel(qreal level) override;
    void initIn() override;
    void initNorm() override;
    void initOut() override;
    void initEnd() override;

signals:

public slots:

private:
    Controller* meta_ctrl_ {};
    DisplayItem* foreground_ {};
    MovableItem* player_ {};
};

#endif // INTROSCREEN_H
