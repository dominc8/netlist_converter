#include "layoutizer.hpp"
#include "logger.hpp"
#include "net_parser.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        LOG_ERROR("Usage: ./app <filename1>");
        return 1;
    }
    char* filename = argv[1];
    FILE *f = fopen(filename, "r");
    if (f == nullptr)
    {
        LOG_ERROR("Cannot open file %s", filename);
        return -1;
    }
    auto views = parse_file(f);
    fclose(f);
    auto nodes = parse_views(views);
    auto g = parse_nodes(nodes, views);
    layout(g, nodes, filename);

    LOG_INFO("Layout:");
    for (const auto &n : nodes)
    {
        LOG_INFO("%s at (%d, %d)", n.name, n.x, n.y);
    }

    return 0;
}

