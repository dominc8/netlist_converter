#include "line_view.hpp"
#include <string.h>

line_view::line_view() : p0{}, p1{}, val{}, name{} {}

line_view::line_view(const char *in_p0, const char *in_p1, const char *in_val, const char *in_name, component_type in_type)
    : line_view()
{
    strncpy(&p0[0], &in_p0[0], sizeof(p0) - 1);
    strncpy(&p1[0], &in_p1[0], sizeof(p1) - 1);
    strncpy(&val[0], &in_val[0], sizeof(val) - 1);
    strncpy(&name[0], &in_name[0], sizeof(name) - 1);
    comp_type = in_type;
}

