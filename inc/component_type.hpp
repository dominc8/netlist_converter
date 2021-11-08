#ifndef COMPONENT_TYPE_HPP
#define COMPONENT_TYPE_HPP

#include <stdint.h>

enum class component_type : uint8_t
{
    V = 0,
    R,
    C,
    L,
    I,
    Unknown,
    DotPoint,
    Ground
};

constexpr int32_t n_component_type = static_cast<int32_t>(component_type::Unknown);

inline bool rotation_matters(component_type comp_type)
{
    return (component_type::V == comp_type) || (component_type::I == comp_type);
}

#endif /* COMPONENT_TYPE_HPP */

