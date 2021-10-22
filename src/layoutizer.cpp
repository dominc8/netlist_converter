#include "layoutizer.hpp"
#include "logger.hpp"
#include <ogdf/basic/graph_generators.h>
#include <ogdf/basic/Array.h>
#include <ogdf/orthogonal/OrthoLayout.h>
#include <ogdf/planarity/EmbedderMinDepthMaxFaceLayers.h>
#include <ogdf/planarity/PlanarSubgraphFast.h>
#include <ogdf/planarity/PlanarizationLayout.h>
#include <ogdf/planarity/SubgraphPlanarizer.h>
#include <ogdf/planarity/VariableEmbeddingInserter.h>

#include <ogdf/fileformats/GraphIO.h>

namespace
{
void create_edges(ogdf::Graph &G, ogdf::Array<ogdf::node> &v, const graph &g)
{
    for (int32_t i = 0; i < g.n_node - 1; ++i)
    {
        for (int32_t j = i + 1; j < g.n_node; ++j)
        {
            if (g.edge_of(i, j))
            {
                G.newEdge(v[i], v[j]);
            }
        }
    }
}

void get_neighbours(const graph &g, int32_t node_idx, int32_t *neighbours)
{
    int32_t tmp_neighbours[3];
    int32_t neighbour_cnt = 0;
    auto *edges = &g.matrix[node_idx * g.n_all_node];

    for (int32_t i = 0; i < g.n_node; ++i)
    {
        tmp_neighbours[neighbour_cnt] = i;
        if (edges[i])
        {
            ++neighbour_cnt;
        }
    }

    neighbours[0] = tmp_neighbours[0];
    neighbours[1] = tmp_neighbours[1];
}

bool node_is_in_corner(const node &middle_node, const node &neighbour1, const node &neighbour2)
{
    bool is_in_corner = false;

    if (((middle_node.x == neighbour1.x) && (middle_node.y == neighbour2.y)) ||
        ((middle_node.x == neighbour2.x) && (middle_node.y == neighbour1.y)))
    {
        is_in_corner = true;
    }

    return is_in_corner;
}

int32_t node_uncornerize(node &comp_node, const node &neighbour1, const node &neighbour2)
{
    int32_t offset_x {0}, offset_y {0}, rotation;
    int32_t lost_neighbour_idx {0};

    if (std::abs(comp_node.x - neighbour1.x) > std::abs(comp_node.y - neighbour1.y))
    {
        offset_x = (neighbour1.x - comp_node.x) / 2;
    }
    else
    {
        offset_y = (neighbour1.y - comp_node.y) / 2;
    }

    if (std::abs(comp_node.x - neighbour2.x) > std::abs(comp_node.y - neighbour2.y))
    {
        int32_t n2_offset = (neighbour2.x - comp_node.x) / 2;
        if (std::abs(offset_x) < std::abs(n2_offset))
        {
            offset_x = n2_offset;
            lost_neighbour_idx = 1;
        }
    }
    else
    {
        int32_t n2_offset = (neighbour2.y - comp_node.y) / 2;
        if (std::abs(offset_y) < std::abs(n2_offset))
        {
            offset_y = n2_offset;
            lost_neighbour_idx = 1;
        }
    }

    if (offset_x > offset_y)
    {
        offset_y = 0;
        rotation = 0;
    }
    else
    {
        offset_x = 0;
        rotation = 90;
    }

    LOG_INFO("Moving node %s from (%d, %d) by offset (%d, %d)", comp_node.name, comp_node.x, comp_node.y, offset_x, offset_y);

    comp_node.x += offset_x;
    comp_node.y += offset_y;
    comp_node.rotation = rotation;

    return lost_neighbour_idx;
}
}

void layout(graph &g, std::vector<node> &nodes, const char *filename)
{
    int32_t n_node = nodes.size();
    ogdf::Graph G;
    ogdf::GraphAttributes GA(G,
        ogdf::GraphAttributes::nodeLabel | ogdf::GraphAttributes::nodeLabelPosition |
        ogdf::GraphAttributes::nodeGraphics | ogdf::GraphAttributes::nodeType |
        ogdf::GraphAttributes::edgeGraphics | ogdf::GraphAttributes::edgeType);
    ogdf::Array<ogdf::node> v(n_node);

    for (int32_t i = 0; i < n_node; ++i)
    {
        v[i] = G.newNode(i);
    }
    create_edges(G, v, g);
    for (auto n : G.nodes)
    {
        GA.width(n) /= 6;
        GA.height(n) /= 6;
    }

    ogdf::PlanarizationLayout pl;
    ogdf::SubgraphPlanarizer sp;
    auto *vei = new ogdf::VariableEmbeddingInserter();
    auto *ps = new ogdf::PlanarSubgraphFast<int32_t>;
    auto *emb = new ogdf::EmbedderMinDepthMaxFaceLayers();
    auto *ol = new ogdf::OrthoLayout();

    ps->runs(1000);
    vei->removeReinsert(ogdf::RemoveReinsertType::All);
    sp.setSubgraph(ps);
    sp.setInserter(vei);

    pl.setEmbedder(emb);
    ol->separation(20.0);
    ol->cOverhang(0.4);
    pl.setPlanarLayouter(ol);

    pl.call(GA);
    GA.scale(10);

    ogdf::Array<ogdf::NodeElement*> arr;

    G.allNodes(arr);

    int32_t max_x = 0;
    int32_t max_y = 0;
    for (int32_t i = 0; i < n_node; ++i)
    {
        const auto &n = arr[i];
        nodes[i].set_coord(GA.x(n), GA.y(n));
        if (nodes[i].comp_type != component_type::DotPoint)
        {
            GA.label(n) = nodes[i].name;
        }
        max_x = (max_x < nodes[i].x) ? nodes[i].x : max_x;
        max_y = (max_y < nodes[i].y) ? nodes[i].y : max_y;
    }

    for (int32_t i = 0; i < n_node; ++i)
    {
        if (static_cast<int32_t>(nodes[i].comp_type) < n_component_type)
        {
            int32_t neighbours[2];
            get_neighbours(g, i, &neighbours[0]);

            auto &comp_node = nodes[i];
            auto &neighbour1 = nodes[neighbours[0]];
            auto &neighbour2 = nodes[neighbours[1]];
            if (node_is_in_corner(comp_node, neighbour1, neighbour2))
            {
                node new_tmp_node("", "", component_type::DotPoint);
                new_tmp_node.set_coord(comp_node.x, comp_node.y);
                int32_t lost_neighbour_idx = node_uncornerize(comp_node, neighbour1, neighbour2);

                nodes.push_back(new_tmp_node);
                g.edge_of(i, neighbours[lost_neighbour_idx]) = 0;
                g.edge_of(neighbours[lost_neighbour_idx], i) = 0;
                g.edge_of(i, g.n_node) = 1;
                g.edge_of(g.n_node, i) = 1;
                g.edge_of(g.n_node, neighbours[lost_neighbour_idx]) = 1;
                g.edge_of(neighbours[lost_neighbour_idx], g.n_node) = 1;
                LOG_INFO("Added node: %s at %d, %d", nodes[g.n_node].name, nodes[g.n_node].x, nodes[g.n_node].y);
                g.n_node++;
            }
            else
            {
                int32_t rotation = 0;
                if ((comp_node.y == neighbour1.y) && (comp_node.y == neighbour1.y))
                {
                    rotation = 90;
                }
                comp_node.rotation = rotation;
            }
        }
    }

    for (int32_t i = 0; i < n_node; ++i)
    {
        const auto &n = arr[i];
        GA.xLabel(n) = nodes[i].x + 30 < max_x ? 30 : -30;
        GA.yLabel(n) = nodes[i].y + 30 < max_y ? 30 : -30;
    }

    std::string s(filename);
    s.append(".svg");
    ogdf::GraphIO::write(GA, s, ogdf::GraphIO::drawSVG);
}

