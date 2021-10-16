#ifndef NET_PARSER_HPP
#define NET_PARSER_HPP

#include "line_view.hpp"
#include "node.hpp"
#include <stdint.h>
#include <stdio.h>
#include <vector>

bool parse_type(char *token, component_type *type);
bool parse_str(char *token, char *str, int32_t str_len);
bool parse_line(char *line, line_view *node);
std::vector<line_view> parse_file(FILE *file);
std::vector<node> parse_views(std::vector<line_view> &views);

#endif /* NET_PARSER_HPP */

