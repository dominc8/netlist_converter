#include "asc_writer.hpp"
#include "layoutizer.hpp"
#include "logger.hpp"
#include "net_parser.hpp"
#include "svg_writer.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        LOG_ERROR("Usage: ./app <filename_in> <filename_out>");
        return 1;
    }
    char* input_filename = argv[1];
    FILE *f = fopen(input_filename, "r");
    if (f == nullptr)
    {
        LOG_ERROR("Cannot open file %s", input_filename);
        return -1;
    }
    auto views = parse_file(f);
    fclose(f);
    auto nodes = parse_views(views);
    auto g = parse_nodes(nodes, views);
    layout(g, nodes, views, input_filename);

    LOG_INFO("Layout:");
    for (const auto &n : nodes)
    {
        LOG_INFO("%s at (%d, %d)", n.name, n.x, n.y);
    }

    char* output_filename = argv[2];
    f = fopen(output_filename, "w");
    if (f == nullptr)
    {
        LOG_ERROR("Cannot open file %s", output_filename);
        return -1;
    }
    write_layout(f, g, nodes);
    svg::write_layout_svg(g, nodes, input_filename);
    fclose(f);

    return 0;
}

