#ifndef ARROW3DCONTROLLER_H
#define ARROW3DCONTROLLER_H

#include "arrowcontroller.h"

#include "pointf3d.h"

#include <array>
#include <utility>

/** @brief A controller which generates a 3D value according to the pressed keys
*/
class Arrow3dController : public ArrowController {
    Q_OBJECT

public:
    enum Arrow3dState {
      kNone = 0x00,
      kLeft = 0x01,
      kDown = 0x02,
      kUp = 0x04,
      kRight = 0x08,
      kBackward = 0x10,
      kForward = 0x20,
      kDownLeft = kDown | kLeft,
      kUpLeft = kUp | kLeft,
      kUpRight = kUp | kRight,
      kDownRight = kDown | kRight,

      kArrowMask = 0xffffffffU,
      kXMask = kLeft | kRight,
      kYMask = kDown | kUp,
      kZMask = kBackward | kForward,
      kXYMask = kXMask | kYMask,
      kXZMask = kXMask | kZMask,
      kYZMask = kYMask | kZMask,
      kXYZMask = kXMask | kYMask | kZMask,
      kLeftMask = kLeft | kYZMask,
      kDownMask = kDown | kXZMask,
      kUpMask = kUp | kXZMask,
      kRightMask = kRight | kYZMask,
      kBackwardMask = kBackward | kXYMask,
      kForwardMask = kForward | kXYMask,
      kDownLeftMask = kDownLeft | kZMask,
      kUpLeftMask = kUpLeft | kZMask,
      kUpRightMask = kUpRight | kZMask,
      kDownRightMask = kDownRight | kZMask,
    };
    Q_ENUM(Arrow3dState)

    static constexpr std::array<Key_state_t, 10> kKeyMap = {
        kLeft, kDown, kUp, kRight,
        kDownLeft, kUpLeft, kUpRight, kDownRight,
        kBackward, kForward,
    };
    using Key_map_t = decltype(kKeyMap);

    static constexpr Key_map_t kKeyMaskMap = {
        kLeftMask, kDownMask, kUpMask, kRightMask,
        kDownLeftMask, kUpLeftMask, kUpRightMask, kDownRightMask,
        kBackwardMask, kForwardMask,
    };

    using ArrowController::ArrowController;

    PointF3d getValue() override;

signals:

public slots:

private:
    Key_map_data_t keyMap() const override;
    Key_map_data_t keyMaskMap() const override;
};

#endif // ARROW3DCONTROLLER_H
