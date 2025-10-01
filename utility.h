#ifndef UTILITY_H
#define UTILITY_H

#include "arrowcontroller.h"
#include "pointf3d.h"

#include <QPointF>
#include <QPointer>
#include <QSizeF>

#include <functional>
#include <utility>

#include <cmath>

namespace Pointf_utility {
constexpr inline QPointF toPointF(const QSizeF& value)
  { return {value.width(), value.height()}; }

constexpr inline QPointF operator*(const QPointF& lhs, const QPointF& rhs)
  { return {lhs.x() * rhs.x(), lhs.y() * rhs.y()}; }
constexpr inline PointF3d operator*(const PointF3d& lhs, const PointF3d& rhs)
  { return {lhs.x() * rhs.x(), lhs.y() * rhs.y(), lhs.z() * rhs.z()}; }

constexpr inline QPointF operator/(qreal lhs, const QPointF& rhs)
  { return {lhs / rhs.x(), lhs / rhs.y()}; }
constexpr inline PointF3d operator/(qreal lhs, const PointF3d& rhs)
  { return {lhs / rhs.x(), lhs / rhs.y(), lhs / rhs.z()}; }

inline QPointF bounded(const QPointF& lhs, const QPointF& rhs)
  { return {std::min(lhs.x(), rhs.x()), std::min(lhs.y(), rhs.y())}; }
inline PointF3d bounded(const PointF3d& lhs, const PointF3d& rhs)
  { return {std::min(lhs.x(), rhs.x()), std::min(lhs.y(), rhs.y()), std::min(lhs.z(), rhs.z())}; }

using std::min;
template <class T>
 inline auto min(const T& lhs, const T& rhs) -> decltype(bounded(lhs, rhs))
  { return bounded(lhs, rhs); }

inline qreal min_rebound(qreal val, qreal upp)
  { return upp - abs(val - upp); }

inline QPointF bounded_rebound(const QPointF& val, const QPointF& upp)
  { return {min_rebound(val.x(), upp.x()), min_rebound(val.y(), upp.y())}; }
inline PointF3d bounded_rebound(const PointF3d& val, const PointF3d& upp)
  { return {min_rebound(val.x(), upp.x()), min_rebound(val.y(), upp.y()), min_rebound(val.z(), upp.z())}; }

template <class T>
 inline auto min_rebound(const T& val, const T& upp) -> decltype(bounded_rebound(val, upp))
  { return bounded_rebound(val, upp); }

inline QPointF expanded(const QPointF& lhs, const QPointF& rhs)
  { return {std::max(lhs.x(), rhs.x()), std::max(lhs.y(), rhs.y())}; }
inline PointF3d expanded(const PointF3d& lhs, const PointF3d& rhs)
  { return {std::max(lhs.x(), rhs.x()), std::max(lhs.y(), rhs.y()), std::max(lhs.z(), rhs.z())}; }

using std::max;
template <class T>
 inline auto max(const T& lhs, const T& rhs) -> decltype(expanded(lhs, rhs))
  { return expanded(lhs, rhs); }

inline qreal max_rebound(qreal val, qreal low)
  { return low + abs(val - low); }

inline QPointF expanded_rebound(const QPointF& val, const QPointF& low)
  { return {max_rebound(val.x(), low.x()), max_rebound(val.y(), low.y())}; }
inline PointF3d expanded_rebound(const PointF3d& val, const PointF3d& low)
  { return {max_rebound(val.x(), low.x()), max_rebound(val.y(), low.y()), max_rebound(val.z(), low.z())}; }

template <class T>
 inline auto max_rebound(const T& val, const T& low) -> decltype(expanded_rebound(val, low))
  { return expanded_rebound(val, low); }

template <class T>
 inline T bound(const T& lower, const T& value, const T& upper)
  { return expanded(lower, bounded(value, upper)); }

inline qreal minmax_rebound(qreal low, qreal val, qreal upp)
  { return (qIsNull(upp-low)) ? upp : max_rebound(upp - fmod(abs(upp-val), 2*(upp-low)), low); }

inline QPointF bound_rebound(const QPointF& low, const QPointF& val, const QPointF& upp)
  { return {minmax_rebound(low.x(), val.x(), upp.x()),
            minmax_rebound(low.y(), val.y(), upp.y())}; }
inline PointF3d bound_rebound(const PointF3d& low, const PointF3d& val, const PointF3d& upp)
  { return {minmax_rebound(low.x(), val.x(), upp.x()),
            minmax_rebound(low.y(), val.y(), upp.y()),
            minmax_rebound(low.z(), val.z(), upp.z())}; }

template <class T>
 inline auto minmax_rebound(const T& lower, const T& value, const T& upper)
      -> decltype(bound_rebound(lower, value, upper))
  { return bound_rebound(lower, value, upper); }

template <class T>
  constexpr inline T unit()  { return {1, 1}; }
template <>
  constexpr inline qreal unit()  { return 1; }
template <>
  constexpr inline PointF3d unit<PointF3d>()  { return {1, 1, 1}; }

template <class T, class U = qreal>
  constexpr inline T scaleUnit(U scale = unit<U>())  { return scale * unit<T>(); }

using std::abs;
template <class T = qreal>
 inline T abs(const PointF3d& value)
  { return sqrt(PointF3d::dotProduct(value, value)); }
template <>
 inline PointF3d abs<PointF3d>(const PointF3d& value)
  { return {abs(value.x()), abs(value.y()), abs(value.z())}; }

using std::copysign;
inline PointF3d copysign(qreal lhs, const PointF3d& rhs)
  { return (qIsNull(abs(rhs))) ? lhs * unit<PointF3d>() : lhs * rhs / abs(rhs); }
inline PointF3d copysign(const PointF3d& lhs, const PointF3d& rhs)
  { return {copysign(lhs.x(), rhs.x()), copysign(lhs.y(), rhs.y()), copysign(lhs.z(), rhs.z())}; }
}  // namespace Pointf_utility

namespace Control_func {
using Control_t = std::function<PointF3d(PointF3d value, PointF3d unit)>;
using Control_mod_t = std::function<PointF3d(PointF3d value)>;
using Control_func_t = std::function<PointF3d()>;

inline Control_t operator*(const Control_mod_t& outer, const Control_t& inner)
  { return [=](PointF3d value, PointF3d unit)  { return outer(inner(value, unit)); }; }
inline Control_mod_t operator*(const Control_mod_t& outer, const Control_mod_t& inner)
  { return [=](PointF3d value)  { return outer(inner(value)); }; }
inline Control_func_t operator*(const Control_mod_t& outer, const Control_func_t& inner)
  { return [=]()  { return outer(inner()); }; }

struct ArrowFunc {
    QPointer<ArrowController> ctrl;
    inline PointF3d operator()() { return (ctrl) ? ctrl->getValue() : PointF3d{}; }
};
Control_t linear_move(const Control_func_t& ctrl = Pointf_utility::unit<PointF3d>,
                      const Control_mod_t& mod = Pointf_utility::scaleUnit<PointF3d, PointF3d>);
inline Control_t linear_move(const ArrowFunc& func,
                             const Control_mod_t& mod = Pointf_utility::scaleUnit<PointF3d, PointF3d>)
  { return linear_move(Control_func_t{func}, mod); }

Control_t accelerate_move(PointF3d vec, const Control_func_t& ctrl = Pointf_utility::unit<PointF3d>,
                          const Control_mod_t& mod = Pointf_utility::scaleUnit<PointF3d, PointF3d>);
inline Control_t accelerate_move(const Control_func_t& ctrl = Pointf_utility::unit<PointF3d>,
                                 const Control_mod_t& mod = Pointf_utility::scaleUnit<PointF3d, PointF3d>)
  { return accelerate_move({}, ctrl, mod); }

Control_t sine_move(PointF3d phase_inc, PointF3d phase = {},
                    const Control_func_t& ctrl = Pointf_utility::unit<PointF3d>,
                    const Control_mod_t& mod = Pointf_utility::scaleUnit<PointF3d, PointF3d>);
inline Control_t sine_move(PointF3d phase_inc, const Control_func_t& ctrl,
                           const Control_mod_t& mod = Pointf_utility::scaleUnit<PointF3d, PointF3d>)
  { return sine_move(phase_inc, {}, ctrl, mod); }

template <class T>
 Control_mod_t limition(T limit)
{
    using namespace Pointf_utility;
    return [=](PointF3d value)
      { return copysign(min(abs<T>(value), limit), value); };
};

template <class T>
  Control_mod_t friction(T fric)
{
    using namespace Pointf_utility;
    return [=](PointF3d value)
    {
        T value_abs = abs<T>(value);
        value_abs = max({}, value_abs - fric);
        return copysign(value_abs, value);
    };
};

template <class T, class U = qreal>
 inline std::function<T()> scaleUnit(const std::function<U()>& func)
  { return [=]()  { return Pointf_utility::scaleUnit<T>(func()); }; }
}  // namespace Control_func

#endif // UTILITY_H
