#ifndef ARROWCONTROLLER_H
#define ARROWCONTROLLER_H

#include "controller.h"

#include "pointf3d.h"

#include <array>
#include <utility>

/** @brief The base class of controllers which generates a 3D vector value according to the pressed keys
        A controller which generates a 2D value according to the pressed keys
*/
class ArrowController : public Controller {
    Q_OBJECT

protected:
    using Key_state_t = uint;
    struct KeyStateItem {Key_state_t value; Key_state_t mask;};
    friend constexpr bool operator==(const KeyStateItem& lhs, const KeyStateItem& rhs);

    using Key_state_list_t = QList<KeyStateItem>;

public:
    enum ArrowState {
      kNone = 0x00,
      kLeft = 0x01,
      kDown = 0x02,
      kUp = 0x04,
      kRight = 0x08,
      kDownLeft = kDown | kLeft,
      kUpLeft = kUp | kLeft,
      kUpRight = kUp | kRight,
      kDownRight = kDown | kRight,

      kArrowMask = 0xffffffffU,
      kXMask = kLeft | kRight,
      kYMask = kDown | kUp,
      kXYMask = kXMask | kYMask,
      kLeftMask = kLeft | kYMask,
      kDownMask = kDown | kXMask,
      kUpMask = kUp | kXMask,
      kRightMask = kRight | kYMask,
      kDownLeftMask = kDownLeft,
      kUpLeftMask = kUpLeft,
      kUpRightMask = kUpRight,
      kDownRightMask = kDownRight,
    };
    Q_ENUM(ArrowState)

    static constexpr std::array<Key_state_t, 8> kKeyMap = {
        kLeft, kDown, kUp, kRight,
        kDownLeft, kUpLeft, kUpRight, kDownRight,
    };
    using Key_map_t = decltype(kKeyMap);

    static constexpr Key_map_t kKeyMaskMap = {
        kLeftMask, kDownMask, kUpMask, kRightMask,
        kDownLeftMask, kUpLeftMask, kUpRightMask, kDownRightMask,
    };

    using Controller::Controller;

    Key_state_t getState();
    virtual PointF3d getValue();

signals:

public slots:

protected:
    using Key_map_data_t = std::pair<const Key_state_t*, size_t>;

private:
    KeyStateItem toKeyState(int key_index) const;

    virtual Key_map_data_t keyMap() const;
    virtual Key_map_data_t keyMaskMap() const;
};

#endif // ARROWCONTROLLER_H
