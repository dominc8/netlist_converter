#ifndef LINE_VIEW_HPP
#define LINE_VIEW_HPP

#include <stdint.h>

enum class component_type : uint8_t
{
    V = 0,
    R,
    C,
    L,
    Unknown,
    DotPoint,
};

constexpr int32_t n_component_type = static_cast<int32_t>(component_type::Unknown);

struct line_view
{
    line_view();
    line_view(const char *in_p0, const char *in_p1, const char *in_val, const char *in_name, component_type in_type);

    char p0[7];
    char padp0;
    char p1[7];
    char padp1;
    char val[8];
    char name[7];
    component_type comp_type;
};

#endif /* LINE_VIEW_HPP */

