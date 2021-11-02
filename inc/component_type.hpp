#ifndef COMPONENT_TYPE_HPP
#define COMPONENT_TYPE_HPP

#include <stdint.h>

enum class component_type : uint8_t
{
    V = 0,
    R,
    C,
    L,
    Unknown,
    DotPoint,
    Ground
};

constexpr int32_t n_component_type = static_cast<int32_t>(component_type::Unknown);

#endif /* COMPONENT_TYPE_HPP */

