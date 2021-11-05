#ifndef LAYOUTIZER_HPP
#define LAYOUTIZER_HPP

#include "graph.hpp"
#include "node.hpp"

void layout(graph &g, std::vector<node> &nodes, const std::vector<line_view> &views, const char *filename);
void get_neighbours(const graph &g, int32_t node_idx, int32_t *neighbours);
bool node_is_in_corner(const node &middle_node, const node &neighbour1, const node &neighbour2);
bool node_is_on_tip(const node &middle_node, const node &neighbour1, const node &neighbour2);
int32_t node_uncornerize(node &comp_node, const node &neighbour1, const node &neighbour2);

#endif /* LAYOUTIZER_HPP */

