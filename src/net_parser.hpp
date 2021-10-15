#ifndef NET_PARSER_HPP
#define NET_PARSER_HPP

#include <stdint.h>
#include <stdio.h>
#include <vector>

enum class component_type : uint8_t
{
    V = 0,
    R,
    C,
    L,
    Unknown
};

constexpr int32_t n_component_type = static_cast<int32_t>(component_type::Unknown);

struct line_view
{
    line_view();
    line_view(const char *in_p0, const char *in_p1, const char *in_val, const char *in_name, component_type in_type);

    char p0[8];
    char p1[8];
    char val[8];
    char name[7];
    component_type comp_type;
};

bool parse_type(char *token, component_type *type);
bool parse_str(char *token, char *str, int32_t str_len);
bool parse_line(char *line, line_view *node);
std::vector<line_view> parse_file(FILE *file);

#endif /* NET_PARSER_HPP */

