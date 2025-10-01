#include "mainwindow.h"

#include "arrowcontroller.h"
#include "arrow3dcontroller.h"
#include "controller.h"
#include "displayobject.h"
#include "introscreen.h"
#include "movableitem.h"
#include "utility.h"
#include "pacman.h"
#include "ghost.h"

#include <QGraphicsEffect>
#include <QScreen>

#include <memory>

#include <cmath>

using namespace Pointf_utility;
using namespace Control_func;

MainWindow::MainWindow(QWidget* parent):
  QMainWindow(parent),
  ui_ {},
  screen_stack_ {},
  next_screen_ {}
{
#ifdef USE_OPENGL
    const auto widget = new QOpenGLWidget;
    widget->makeCurrent();
#else
    const auto widget = new QWidget;
#endif

    ui_.setupUi(this);
    ui_.graphicsView->setScene(new QGraphicsScene(this));
    ui_.graphicsView->setViewport(widget);

    ui_.graphicsView->scene()->setSceneRect({{0, 0}, ui_.graphicsView->size() - QSizeF{2, 2}});

    startTimer(update_interval_);

    setNextScreen([=]()  { introScreen(); });
    startNextScreen();
}

MainWindow::~MainWindow()
  { purgeScreen(); }

void MainWindow::introScreen()
{
    const auto screen = pushScreen(new IntroScreen(this));
    setNextScreen([=]()  { gameplayScreen(); });

    ui_.label->setText("IntroScreen");

    screen->startTransitionIn();
}


void MainWindow::gameplayPauseScreen()
{
    const auto blur = new QGraphicsBlurEffect;
    topScreen()->graphic()->setGraphicsEffect(blur);
    const auto screen = pushScreen()->setDurationInOut(0.5, 0.5);

    ui_.label->setText("GameplayPauseScreen");

    const auto meta_ctrl = new Controller({
          Qt::Key_Escape, Qt::Key_R, Qt::Key_Q, 0,
          Qt::Key_Down, Qt::Key_Up, Qt::Key_Return}, screen);

    const auto menu_title = new QGraphicsTextItem("Paused", screen->graphic());
    menu_title->setScale(2);
    menu_title->setPos(screen->widget()->rect().center()
                         - menu_title->scale() * menu_title->shape().boundingRect().center()
                         - QPointF{0, 1./4 * screen->widget()->rect().height()});

    const auto index = std::make_shared<int>(0);
    const auto option = std::make_shared<QVector<QString>>();
    *option = {"Resume", "Restart", "Return to title", "Return to desktop"};
    const auto menu_item = std::make_shared<QVector<QGraphicsTextItem*>>();

    for (auto k = 0, n = option->size(); k < n; ++k) {
        const auto item = new QGraphicsTextItem((*option)[k], screen->graphic());
        item->setPos(screen->widget()->rect().center()
                       - item->scale() * item->shape().boundingRect().center()
                       + QPointF{0, k * item->shape().boundingRect().height()});
        item->setOpacity(0.5);
        *menu_item += item;
    }
    (*menu_item)[*index]->setOpacity(1);

    const auto option_func = std::make_shared<QVector<std::function<void ()>>>();
    *option_func = {
      [=]()  { },
      [=]()  { setNextScreen([=]()  { gameplayScreen(); }); topScreen()->startTransitionOut(); },
      [=]()  { setNextScreen([=]()  { introScreen(); }); topScreen()->startTransitionOut(); },
      [=]()  { setNextScreen([=]()  { QCoreApplication::quit(); }); topScreen()->startTransitionOut(); }};

    const auto key_func = [=](int key_index)
    {
        (*menu_item)[*index]->setOpacity(0.5);
        switch (key_index) {
          case 4:
            *index = (*index + 1) % option->size();
            break;
          case 5:
            *index = (*index + option->size() - 1) % option->size();
            break;

          case 6:
            key_index = *index;
            [[clang::fallthrough]];
          case 0:
          case 1:
          case 2:
          case 3:
            if ((*index && key_index) || screen->trans_state() != Screen::kIn) {
                if (key_index) { (*menu_item)[key_index]->setOpacity(1); }
                *index = key_index;
                return screen->startTransitionOut();
            }
        }
        (*menu_item)[*index]->setOpacity(1);
    };
    connect(meta_ctrl, &Controller::keyPressed, screen, key_func);
    connect(meta_ctrl, &Controller::keyAutoRepeated, screen, key_func);

    screen->graphic()->setOpacity(0);
    connect(screen, &Screen::TransitionLevelChanged, screen, [=](qreal level)
      { screen->graphic()->setOpacity(level); });
    blur->setBlurRadius(0);
    connect(screen, &Screen::TransitionLevelChanged, blur, [=](qreal level)
      { blur->setBlurRadius(5 * level); });

    connect(screen, &Screen::transitionOutStarted, meta_ctrl, [=]()
      { meta_ctrl->disconnect(); });

    setNextScreen([=]()
    {
        topScreen()->graphic()->setGraphicsEffect(nullptr);
        topScreen()->setPause(false);
        (*option_func)[*index]();
        ui_.label->setText("GameplayScreen");
    });

    screen->startTransitionIn();
}

std::function<void ()> MainWindow::next_screen() const
  { return next_screen_; }

MainWindow* MainWindow::setNextScreen(const std::function<void ()>& next_screen)
  { next_screen_ = next_screen; return this; }

void MainWindow::gameplayScreen()
{
    const auto screen = pushScreen()->setDurationInOut(1, 2);

    move(200, 200);
    resize(400, 300);
    ui_.label->setText("GameplayScreen");

    const auto rot_ctrl = new Arrow3dController(
          {Qt::Key_W, Qt::Key_D, Qt::Key_A, Qt::Key_S,
           0, 0, 0, 0,
           Qt::Key_Q, Qt::Key_E,}, screen);

    const auto meta_ctrl = new Controller({Qt::Key_Escape,}, screen);
    const auto game = new MovableItem(new QGraphicsRectItem{0, 0, 0, 0}, screen);
    game->setRotCtrl(linear_move({rot_ctrl}))->setRotUnit({1, 1, 1});

    const auto speed_ctrl = new Controller({Qt::Key_Shift}, screen);

    const auto pacman = new Pacman(game);
    pacman->setPos({0, 50, 0})->markDirty();
    pacman->ctrl()->r_key_config() += {Qt::Key_Minus, Qt::Key_Plus};

    connect(speed_ctrl, &Controller::keyPressed, pacman, [=](int)
      { pacman->setPosUnit({5, 5, 5}); });
    connect(speed_ctrl, &Controller::keyReleased, pacman, [=](int)
      { pacman->setPosUnit({2, 2, 2}); });

    const auto ghost0 = new Ghost(game);
    ghost0->ctrl()->r_key_config() = {};
    ghost0->setPos({0, 0, 0})->markDirty();

    const auto foreground = new QGraphicsRectItem{{{0, 0}, screen->widget()->size()}};
    foreground->setBrush(Qt::black);
    foreground->update();
    foreground->setOpacity(1.0);
    const auto foreground_item = new DisplayItem(foreground, screen);

    connect(screen, &Screen::TransitionLevelChanged, foreground_item, [=](qreal level)
      { foreground_item->graphic()->setOpacity(1.0-level); });

    const auto rejection_state = std::make_shared<QVector<bool>>();
    *rejection_state = {/* is_rej */ false, /* can_rej */ false};
    const auto freeze_state = std::make_shared<QVector<bool>>();
    *freeze_state = {/* is_freeze */ false, /* can_freeze */ false};
    const auto special_ctrl = new Controller(
          {Qt::Key_B, Qt::Key_X}, screen);

    const auto unfreeze_func = [=]()
    {
        if ((*freeze_state)[0]) {
            (*freeze_state)[0] = false;
            QTimer::singleShot(2 * 1000, screen, [=]()  {(*freeze_state)[1] = false;});
            foreground_item->graphic()->setOpacity(0.0);
        }
    };

    connect(special_ctrl, &Controller::keyPressed, screen, [=](int key_index)
    {
        std::remove_const<decltype(rejection_state)>::type obj_state;
        switch (key_index) {
          case 0:
            if ((*(obj_state = rejection_state))[1]) { return; }
            QTimer::singleShot(2 * 1000, screen, [=]()  {(*rejection_state)[0] = false;});
            QTimer::singleShot(4 * 1000, screen, [=]()  {(*rejection_state)[1] = false;});
            break;
          case 1:
            if ((*(obj_state = freeze_state))[1]) { return; }
            foreground_item->graphic()->setOpacity(0.25);
            QTimer::singleShot(2 * 1000, screen, unfreeze_func);
            break;
        }
        *obj_state = {true, true};
    });
    connect(special_ctrl, &Controller::keyReleased, screen, [=](int key_index)
    {
        switch (key_index) {
          case 0: return;
          case 1: return unfreeze_func();
        }
    });

    connect(screen, &Screen::transitionInEnded, screen, [=]()
    {
        connect(meta_ctrl, &Controller::keyPressed, screen, [=](int key_index)
        {
            if (key_index == 0 && !screen->is_paused()) {
                screen->setPause(true);
                gameplayPauseScreen();
            }
        });
        ghost0->setUpdateFunc([=](DisplayItem* pself)
        {
            if ((*freeze_state)[0]) { return; }
            pself->setPos(pself->pos()
                  + (((*rejection_state)[0]) ? -1 : 1) * copysign(1, pacman->pos() - pself->pos()));
            pself->markDirty();
            if (const auto g = pself->graphic()) {
                if (const auto pg = pacman->graphic()) {
                    if (abs(pg->zValue() - g->zValue()) < 10 && g->collidesWithItem(pg)) {
                        meta_ctrl->disconnect();
                        speed_ctrl->disconnect();
                        pself->addOneshotUpdateFunc([=](DisplayItem* pself)
                          { pself->setUpdateFunc(nullptr); });
                        pacman->apoptosis();

                        if (!next_screen()) { setNextScreen([=]()  { introScreen(); }); }
                        QTimer::singleShot(2 * 1000, screen, &Screen::startTransitionOut);
                    }
                }
            }
        });
    });

    connect(screen, &Screen::transitionOutStarted, meta_ctrl, [=]()
    {
        meta_ctrl->disconnect();
        connect(meta_ctrl, &Controller::keyPressed, screen, [=](int key_index)
          { if (key_index == 0) { screen->endTransitionOut(); } });
    });

    screen->startTransitionIn();
}

void MainWindow::popScreen()
{
    if (!screen_stack_.isEmpty()) {
        delete screen_stack_.pop();
    }
}

void MainWindow::purgeScreen()
{
    while (!screen_stack_.isEmpty()) {
        delete screen_stack_.pop();
    }
}

Screen* MainWindow::topScreen()
  { return screen_stack_.top(); }

Screen* MainWindow::pushScreen(Screen* screen)
{
    setNextScreen(nullptr);
    if (!screen) { screen = new Screen(this); }
    screen_stack_.push(screen
          ->bindToWidget(ui_.graphicsView)->bindToScene(ui_.graphicsView->scene()));
    connect(topScreen(), &Screen::transitionOutEnded, this, [=]()  { popScreen(); startNextScreen(); });
    return topScreen();
}

void MainWindow::startNextScreen()
{
    emit screenNextStarted();
    if (next_screen()) { QTimer::singleShot(0, this, next_screen()); }
    setNextScreen(nullptr);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    if (event->size().isEmpty()) { return resize(event->size().expandedTo({1, 1})); };
    ui_.graphicsView->resize(event->size());
    ui_.graphicsView->scene()->setSceneRect({{0, 0}, ui_.graphicsView->size() - QSizeF{2, 2}});
    for (const auto& screen: screen_stack_) {
        screen->updateGeometry();
    }
    QMainWindow::resizeEvent(event);
}

void MainWindow::timerEvent(QTimerEvent* event)
{
    for (const auto& screen: screen_stack_) {
        screen->update();
    }
    QMainWindow::timerEvent(event);
}
