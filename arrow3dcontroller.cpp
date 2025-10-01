#include "arrow3dcontroller.h"

#include <cmath>

constexpr Arrow3dController::Key_map_t Arrow3dController::kKeyMap;
constexpr Arrow3dController::Key_map_t Arrow3dController::kKeyMaskMap;

auto Arrow3dController::keyMap() const -> Key_map_data_t
  { return {kKeyMap.data(), kKeyMap.size()}; }
auto Arrow3dController::keyMaskMap() const -> Key_map_data_t
  { return {kKeyMaskMap.data(), kKeyMaskMap.size()}; }

PointF3d Arrow3dController::getValue()
{
    Key_state_t state = getState();
    PointF3d res = {((state & kRight) ? 1. : 0.) - ((state & kLeft) ? 1. : 0.),
          ((state & kDown) ? 1. : 0.) - ((state & kUp) ? 1. : 0.),
          ((state & kBackward) ? 1. : 0.) - ((state & kForward) ? 1. : 0.)};
    return res.isNull()
          ? PointF3d{0, 0, 0}
          : res / sqrt(PointF3d::dotProduct(res, res));
}
