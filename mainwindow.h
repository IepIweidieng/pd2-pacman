#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "displayitem.h"
#include "screen.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QMainWindow>
#include <QPointer>
#include <QStack>
#include <QTimer>

#include <functional>
#include <utility>

#undef USE_OPENGL

#ifdef USE_OPENGL
#include <QOpenGLWidget>
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    std::function<void ()> next_screen() const;
    MainWindow* setNextScreen(const std::function<void ()>& next_screen);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void timerEvent(QTimerEvent* event) override;

    virtual void popScreen();
    virtual void purgeScreen();
    Screen* topScreen();
    Screen* pushScreen(Screen* screen = nullptr);

signals:
    void screenNextStarted();

public slots:
    void startNextScreen();

private:
    void introScreen();
    void gameplayScreen();
    void gameplayPauseScreen();

    Ui::MainWindow ui_;
    QStack<QPointer<Screen>> screen_stack_;
    std::function<void ()> next_screen_;
    int update_interval_ = qRound(1000./60);
};

#endif // MAINWINDOW_H
