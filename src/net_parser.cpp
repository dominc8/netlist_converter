#include "net_parser.hpp"
#include "logger.hpp"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

namespace
{
constexpr char comp_type_map[n_component_type] = 
{
    'V',
    'R',
    'C',
    'L',
};
}

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

bool parse_type(char *token, component_type *type)
{

    for (int32_t i = 0; i < n_component_type; ++i)
    {
        if (comp_type_map[i] == toupper(token[0]))
        {
            *type = static_cast<component_type>(i);
            return true;
        }
    }

    *type = component_type::Unknown;
    return false;
}

bool parse_str(char *token, char *str, int32_t str_len)
{
    int32_t token_len = strlen(token);
    bool result = false;

    if (token_len < str_len)
    {
        memcpy(str, token, token_len);
        str[token_len] = '\0';
        result = true;
    }

    return result;
}

bool parse_line(char *line, line_view *node)
{
    constexpr char delimiters[] = " \t\n";
    char *token = strtok(line, &delimiters[0]);
    if (nullptr == token)
    {
        return false;
    }

    bool result;

    result = parse_str(token, &node->name[0], sizeof(node->name));
    result = parse_type(token, &node->comp_type);
    token = strtok(nullptr, &delimiters[0]);

    if ((nullptr == token) || !result)
    {
        return false;
    }

    result = parse_str(token, &node->p0[0], sizeof(node->p0));
    token = strtok(nullptr, &delimiters[0]);

    if ((nullptr == token) || !result)
    {
        return false;
    }

    result = parse_str(token, &node->p1[0], sizeof(node->p1));
    token = strtok(nullptr, &delimiters[0]);

    if ((nullptr == token) || !result)
    {
        return false;
    }

    result = parse_str(token, &node->val[0], sizeof(node->val));

    return result;
}

std::vector<line_view> parse_file(FILE *file)
{
    constexpr int32_t max_line_size = 128;
    char line[max_line_size];

    std::vector<line_view> views;
    views.reserve(32);

    while(1)
    {
        memset(line, '\0', max_line_size);
        fgets(line, max_line_size, file);
        if (feof(file))
        {
            LOG_INFO("Reached end of file");
            break;
        }
        else
        {
            LOG_INFO("Calling parse_line");
            line_view node;
            bool result = parse_line(line, &node);

            if (result)
            {
                LOG_INFO("New node: %s, %s, %s, %s, %c", node.name, node.p0, node.p1, node.val, comp_type_map[static_cast<int32_t>(node.comp_type)]);
                views.push_back(node);
            }
        }
    }
    return views;
}

