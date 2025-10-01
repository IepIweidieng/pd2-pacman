#include "arrowcontroller.h"

#include <QDebug>

#include <cmath>

constexpr ArrowController::Key_map_t ArrowController::kKeyMap;
constexpr ArrowController::Key_map_t ArrowController::kKeyMaskMap;

constexpr bool operator==(const ArrowController::KeyStateItem& lhs, const ArrowController::KeyStateItem& rhs)
{
    return lhs.value == rhs.value && lhs.mask == rhs.mask;
}

auto ArrowController::getState() -> Key_state_t
{
    Key_state_t state = kNone;
    for (const auto k: getKeyList()) {
        const auto state_k = toKeyState(k);
        state = state_k.value | (state & state_k.mask);
    }

    return state;
}

PointF3d ArrowController::getValue()
{
    Key_state_t state = getState();
    QPointF res = {((state & kRight) ? 1. : 0.) - ((state & kLeft) ? 1. : 0.),
          ((state & kDown) ? 1. : 0.) - ((state & kUp) ? 1. : 0.)};
    return res.isNull()
          ? QPointF{0, 0}
          : res / sqrt(QPointF::dotProduct(res, res));
}

auto ArrowController::toKeyState(int key_index) const -> KeyStateItem
{
    if (0 <= key_index && size_t(key_index) < keyMap().second) {
        return {keyMap().first[key_index], keyMaskMap().first[key_index]};
    }
    return {kNone, kArrowMask};
}

auto ArrowController::keyMap() const -> Key_map_data_t
  { return {kKeyMap.data(), kKeyMap.size()}; }
auto ArrowController::keyMaskMap() const -> Key_map_data_t
  { return {kKeyMaskMap.data(), kKeyMaskMap.size()}; }
