#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/basic/graph_generators.h>
#include <ogdf/orthogonal/OrthoLayout.h>
#include <ogdf/planarity/EmbedderMinDepthMaxFaceLayers.h>
#include <ogdf/planarity/PlanarSubgraphFast.h>
#include <ogdf/planarity/PlanarizationLayout.h>
#include <ogdf/planarity/SubgraphPlanarizer.h>
#include <ogdf/planarity/VariableEmbeddingInserter.h>
#include <ogdf/basic/Array.h>


int main()
{
    ogdf::Graph G;
    ogdf::GraphAttributes GA(G,
        ogdf::GraphAttributes::nodeGraphics | ogdf::GraphAttributes::nodeType |
        ogdf::GraphAttributes::edgeGraphics | ogdf::GraphAttributes::edgeType);

    if (0)
    {
        ogdf::randomSimpleGraph(G, 10, 20);
    }
    else
    {
        constexpr int32_t n_crosspoints = 3;
        constexpr int32_t n_el = 4;
        constexpr int32_t n_node = n_crosspoints + n_el;
        /*
         *  ---0---
         * |   |   |
         * |   |   5
         * 3   4   2
         * |   |   6
         * |   |   |
         *  ---1---
         * */
        ogdf::Array<ogdf::node> v(n_node);

        for (int32_t i = 0; i < n_node; ++i)
        {
            v[i] = G.newNode();
        }
        G.newEdge(v[0], v[3]);
        G.newEdge(v[0], v[4]);
        G.newEdge(v[0], v[5]);
        G.newEdge(v[1], v[3]);
        G.newEdge(v[1], v[4]);
        G.newEdge(v[1], v[6]);
        G.newEdge(v[2], v[5]);
        G.newEdge(v[2], v[6]);
    }

    for (auto v : G.nodes)
    {
        GA.width(v) /= 2;
        GA.height(v) /= 2;
    }

    ogdf::PlanarizationLayout pl;

    ogdf::SubgraphPlanarizer crossMin;

    auto* ps = new ogdf::PlanarSubgraphFast<int>;
    ps->runs(1000);
    ogdf::VariableEmbeddingInserter *ves = new ogdf::VariableEmbeddingInserter;
    ves->removeReinsert(ogdf::RemoveReinsertType::All);

    crossMin.setSubgraph(ps);
    crossMin.setInserter(ves);

    ogdf::EmbedderMinDepthMaxFaceLayers *emb = new ogdf::EmbedderMinDepthMaxFaceLayers;
    pl.setEmbedder(emb);

    ogdf::OrthoLayout *ol = new ogdf::OrthoLayout;
    ol->separation(20.0);
    ol->cOverhang(0.4);
    pl.setPlanarLayouter(ol);

    pl.call(GA);

    ogdf::Array<ogdf::NodeElement*> arr;

    G.allNodes(arr);

    int32_t i = 0;
    for (auto &n : arr)
    {
        double x = GA.x(n);
        double y = GA.y(n);

        std::cout << i << ": " << x << ", " << y << "\n";
        ++i;
    }

    ogdf::GraphIO::write(GA, "output-ERDiagram.gml", ogdf::GraphIO::writeGML);
    ogdf::GraphIO::write(GA, "output-ERDiagram.svg", ogdf::GraphIO::drawSVG);

    return 0;
}

