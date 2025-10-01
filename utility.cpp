#include "utility.h"

namespace Pointf_utility {
}

using namespace Pointf_utility;
using Pointf_utility::operator*;

namespace Control_func {
Control_t linear_move(const Control_func_t& ctrl, const Control_mod_t& mod)
{
    return [=](PointF3d value, PointF3d unit)
    {
        PointF3d vec = unit * ctrl();
        vec = mod(vec);
        return value + vec;
    };
}

Control_t accelerate_move(PointF3d vec, const Control_func_t& ctrl, const Control_mod_t& mod)
{
    PointF3d value_prev = {nan(""), nan(""), nan("")};
    return [=](PointF3d value, PointF3d unit) mutable
    {
        if (!std::isnan(value_prev.x())) {
            vec = value - value_prev;
        }
        value_prev = value;

        vec += unit * ctrl();
        vec = mod(vec);
        return value + vec;
    };
}

Control_t sine_move(const PointF3d phase_inc, PointF3d phase,
                    const Control_func_t& ctrl, const Control_mod_t& mod)
{
    return [=](PointF3d value, PointF3d unit) mutable
    {
        PointF3d vec = phase_inc * ctrl();
        vec = mod(vec);
        phase += vec;
        return value + unit * PointF3d{
                sin(phase.x()), sin(phase.y()), sin(phase.z())};
    };
}
}  // namespace Control_func
