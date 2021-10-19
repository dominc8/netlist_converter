#include "asc_writer.hpp"
#include "logger.hpp"

namespace
{
const char* const symbol_map[n_component_type] = 
{
    "voltage",
    "res",
    "cap",
    "ind",
};
}

void write_header(FILE *f)
{
    fprintf(f, "Version 4\n"
               "SHEET 1 680 480\n");
}

void write_node_symbol(FILE *f, const node &n, int32_t rotation)
{
    fprintf(f, "SYMBOL %s %d %d R%d\n"
               "SYMATTR InstName %s\n"
               "SYMATTR Value %s\n",
               symbol_map[static_cast<int32_t>(n.comp_type)], n.x, n.y, rotation,
               n.name, n.val);
}

void write_layout(FILE *f, const graph &g, const std::vector<node> &nodes)
{
    write_header(f);
    for (const auto &n : nodes)
    {
        if (static_cast<int32_t>(n.comp_type) >= n_component_type)
        {
            continue;
        }
        int32_t rotation = 0;
        // get_rotation
        write_node_symbol(f, n, rotation);
    }
}

