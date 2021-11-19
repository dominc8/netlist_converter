#ifndef SVG_WRITER_HPP
#define SVG_WRITER_HPP

#include "graph.hpp"
#include "node.hpp"

namespace svg
{
void write_layout_svg(const graph &g, const std::vector<node> &nodes, const char *filename);
}

#endif /* SVG_WRITER_HPP */

