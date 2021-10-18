#include "layoutizer.hpp"
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
}

void layout(const graph &g, std::vector<node> &nodes, const char *filename)
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
        const auto &n = arr[i];
        GA.xLabel(n) = nodes[i].x + 30 < max_x ? 30 : -30;
        GA.yLabel(n) = nodes[i].y + 30 < max_y ? 30 : -30;
    }

    std::string s(filename);
    s.append(".svg");
    ogdf::GraphIO::write(GA, s, ogdf::GraphIO::drawSVG);
}

