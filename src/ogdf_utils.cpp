#include "ogdf_utils.hpp"
#include "component_type.hpp"
#include "stdint.h"

#include <ogdf/basic/Array.h>
#include <ogdf/basic/Graph.h>
#include <ogdf/fileformats/SvgPrinter.h>
#include <ogdf/orthogonal/OrthoLayout.h>
#include <ogdf/planarity/EmbedderMinDepthMaxFaceLayers.h>
#include <ogdf/planarity/PlanarizationLayout.h>
#include <ogdf/planarity/PlanarSubgraphFast.h>
#include <ogdf/planarity/SubgraphPlanarizer.h>
#include <ogdf/planarity/VariableEmbeddingInserter.h>

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

void save_layout_svg(const graph &g, const std::vector<node> &nodes, const char *filename)
{
    ogdf::Graph G;
    ogdf::GraphAttributes GA(G,
        ogdf::GraphAttributes::nodeLabel | ogdf::GraphAttributes::nodeLabelPosition |
        ogdf::GraphAttributes::nodeGraphics | ogdf::GraphAttributes::nodeType |
        ogdf::GraphAttributes::edgeGraphics | ogdf::GraphAttributes::edgeType);
    GA.directed() = false;

    int32_t max_x = 0;
    int32_t max_y = 0;
    int32_t n_node = g.n_node;
    ogdf::Array<ogdf::node> v(n_node);

    for (int32_t i = 0; i < n_node; ++i)
    {
        const auto &n = nodes[i];
        max_x = (max_x < n.x) ? n.x : max_x;
        max_y = (max_y < n.y) ? n.y : max_y;
    }

    for (int32_t i = 0; i < n_node; ++i)
    {
        v[i] = G.newNode(i);
    }

    for (int32_t i = 0; i < n_node; ++i)
    {
        for (int32_t j = 0; j < i; ++j)
        {
            if (g.edge_of(i, j))
            {
                G.newEdge(v[i], v[j]);
            }
        }
    }

    ogdf::Array<ogdf::NodeElement*> arr;
    G.allNodes(arr);

    for (int32_t i = 0; i < n_node; ++i)
    {
        constexpr int32_t label_offset = 25;
        const auto &n = arr[i];
        float scale = 1.0F / 4;
        if (static_cast<int32_t>(nodes[i].comp_type) < n_component_type)
        {
            scale = 1.5F;
        }
        GA.width(n) *= scale;
        GA.height(n) *= scale;
        GA.label(n) = nodes[i].name;
        GA.x(n) = nodes[i].x;
        GA.y(n) = nodes[i].y;
        GA.xLabel(n) = nodes[i].x + label_offset < max_x ? label_offset : -label_offset;
        GA.yLabel(n) = nodes[i].y + label_offset < max_y ? label_offset : -label_offset;
    }

    std::string s(filename);
    s.append(".svg");

    ogdf::GraphIO::SVGSettings svg_settings;
    svg_settings.fontSize(16);
    ogdf::SvgPrinter svg_printer(GA, svg_settings);
    std::ofstream os(s);
    svg_printer.draw(os);
}

void ogdf_layout(graph &g, std::vector<node> &nodes)
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
    GA.scale(4);

    ogdf::Array<ogdf::NodeElement*> arr;

    G.allNodes(arr);

    for (int32_t i = 0; i < n_node; ++i)
    {
        const auto &n = arr[i];
        nodes[i].set_coord(GA.x(n), GA.y(n));
    }
}

