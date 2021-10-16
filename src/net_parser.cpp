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

bool parse_line(char *line, line_view *view)
{
    constexpr char delimiters[] = " \t\n";
    char *token = strtok(line, &delimiters[0]);
    if (nullptr == token)
    {
        return false;
    }

    bool result;

    result = parse_str(token, &view->name[0], sizeof(view->name));
    result = parse_type(token, &view->comp_type);
    token = strtok(nullptr, &delimiters[0]);

    if ((nullptr == token) || !result)
    {
        return false;
    }

    result = parse_str(token, &view->p0[0], sizeof(view->p0));
    token = strtok(nullptr, &delimiters[0]);

    if ((nullptr == token) || !result)
    {
        return false;
    }

    result = parse_str(token, &view->p1[0], sizeof(view->p1));
    token = strtok(nullptr, &delimiters[0]);

    if ((nullptr == token) || !result)
    {
        return false;
    }

    result = parse_str(token, &view->val[0], sizeof(view->val));

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
            line_view view;
            bool result = parse_line(line, &view);

            if (result)
            {
                LOG_INFO("New view: %s, %s, %s, %s, %c", view.name, view.p0, view.p1, view.val, comp_type_map[static_cast<int32_t>(view.comp_type)]);
                views.push_back(view);
            }
        }
    }
    return views;
}

