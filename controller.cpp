#include "controller.h"

#include "screen.h"

Controller::Controller(QObject* parent):
  QObject(parent),
  key_config_ {}
  { bindToObject(parent); }

Controller::Controller(const Key_vector_t& key_config, QObject* parent):
  QObject(parent),
  key_config_ {key_config}
  { bindToObject(parent); }

Controller::Controller(Key_vector_t&& key_config, QObject* parent):
  QObject(parent),
  key_config_ {std::move(key_config)}
  { bindToObject(parent); }

Controller* Controller::bindToObject(QObject* obj)
{
    if (const auto p = qobject_cast<QWidget*>(obj)) {
        p->installEventFilter(this);
        return this;
    }
    if (const auto p = qobject_cast<Screen*>(obj)) {
        return bindToObject(p->widget());
    }
    return this;
}
Controller* Controller::unbindFromObject(QObject* obj)
{
    for (const auto k: getKeyList()) {
        key_list_.removeAll(k);
        emit keyReleased(k);
    }
    if (const auto p = qobject_cast<QWidget*>(obj)) {
        p->removeEventFilter(this);
        return this;
    }
    if (const auto p = qobject_cast<Screen*>(obj)) {
        return unbindFromObject(p->widget());
    }
    return this;
}

auto Controller::r_key_config() -> Key_vector_t&
  { return key_config_; }

Controller* Controller::setKeyConfig(const Key_vector_t& key_config)
  { key_config_ = key_config; return this; }
Controller* Controller::setKeyConfig(Key_vector_t&& key_config)
  { key_config_ = std::move(key_config); return this; }

Controller* Controller::setEnabled(bool enabled)
  { enabled_ = enabled; return this; }

auto Controller::key_config() const -> Key_vector_t
  { return key_config_; }
bool Controller::is_enabled() const
  { return enabled_; }

auto Controller::getKeyList() -> Key_list_t
{
    flushKeyListNew();
    const auto res = key_list_;
    if (enabled_) { flushKeyListRm(); }
    return res;
}

void Controller::pressKey(int key_index)
  { if (enabled_) { emit keyPressed(key_index); } }
void Controller::autoRepeatKey(int key_index)
  { if (enabled_) { emit keyAutoRepeated(key_index); } }
void Controller::releaseKey(int key_index)
  { if (enabled_) { emit keyReleased(key_index); } }

int Controller::toKeyIndex(Key_t key) const
  { return key_config_.indexOf(key); }

bool Controller::keyPressEvent(QKeyEvent* event)
{
    const auto k = toKeyIndex(event->key());
    switch (k) {
    case -1: return false;
    default:
        if (event->isAutoRepeat()) {
            if (key_list_.contains(k) || key_list_new_.contains(k)) {
                autoRepeatKey(k);
            }
            return false;
        }
        key_list_new_.push_back(k);
        pressKey(k);
        return false;
    }
}

bool Controller::keyReleaseEvent(QKeyEvent* event)
{
    if (event->isAutoRepeat()) { return false; }
    const auto k = toKeyIndex(event->key());
    switch (k) {
    case -1: return false;
    default:
        if (!enabled_ || key_list_new_.contains(k)) {
            key_list_rm_.push_back(k);
        }
        else {
            key_list_.removeAll(k);
        }
        releaseKey(k);
        return false;
    }
}

void Controller::flushKeyListNew()
{
    for (const auto k: key_list_new_) {
        key_list_.push_back(k);
    }
    key_list_new_.clear();
}

void Controller::flushKeyListRm()
{
    for (const auto k: key_list_rm_) {
        key_list_.removeAll(k);
    }
    key_list_rm_.clear();
}

bool Controller::eventFilter(QObject* watched, QEvent* event)
{
    switch (event->type()) {
    case QEvent::KeyPress:
        return keyPressEvent(dynamic_cast<QKeyEvent*>(event));
    case QEvent::KeyRelease:
        return keyReleaseEvent(dynamic_cast<QKeyEvent*>(event));
    default:
        return QObject::eventFilter(watched, event);
    }
}
