#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QKeyEvent>
#include <QWidget>

/** @brief The base class of controllers which handles the ordered list of pressed buttons/keys
       A basic controller with any number of buttons/keys
*/
class Controller : public QObject {
    Q_OBJECT

protected:
    using Key_t = decltype(std::declval<QKeyEvent>().key());
    using Key_vector_t = QVector<Key_t>;
    using Key_list_t = QList<Key_t>;

public:
    using Key_config_t = Key_vector_t;

    explicit Controller(QObject* parent = nullptr);
    Controller(const Key_vector_t& key_config, QObject* parent = nullptr);
    Controller(Key_vector_t&& key_config, QObject* parent = nullptr);

    Controller* bindToObject(QObject* obj);
    Controller* unbindFromObject(QObject* obj);

    Key_vector_t& r_key_config();

    Controller* setKeyConfig(const Key_vector_t& key_config);
    Controller* setKeyConfig(Key_vector_t&& key_config);

    Key_vector_t key_config() const;

    Key_list_t getKeyList();

    bool is_enabled() const;
    Controller* setEnabled(bool enabled);

signals:
    void keyPressed(int key_index);
    void keyAutoRepeated(int key_index);
    void keyReleased(int key_index);

public slots:
    void pressKey(int key_index);
    void autoRepeatKey(int key_index);
    void releaseKey(int key_index);

protected:
    int toKeyIndex(Key_t key) const;
    bool keyPressEvent(QKeyEvent* event);
    bool keyReleaseEvent(QKeyEvent* event);
    void flushKeyListNew();
    void flushKeyListRm();

private:
    bool eventFilter(QObject* watched, QEvent* event) override;

    Key_vector_t key_config_;

    Key_list_t key_list_ {};
    Key_list_t key_list_new_ {};
    Key_list_t key_list_rm_ {};

    bool enabled_ {true};
};

#endif // CONTROLLER_H
