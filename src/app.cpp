#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/basic/graph_generators.h>
#include <ogdf/orthogonal/OrthoLayout.h>
#include <ogdf/planarity/EmbedderMinDepthMaxFaceLayers.h>
#include <ogdf/planarity/PlanarSubgraphFast.h>
#include <ogdf/planarity/PlanarizationLayout.h>
#include <ogdf/planarity/SubgraphPlanarizer.h>
#include <ogdf/planarity/VariableEmbeddingInserter.h>
#include <ogdf/basic/Array.h>

using namespace ogdf;

int main()
{
    Graph G;
    GraphAttributes GA(G,
      GraphAttributes::nodeGraphics | GraphAttributes::nodeType |
      GraphAttributes::edgeGraphics | GraphAttributes::edgeType);

    if (0)
    {
        randomSimpleGraph(G, 10, 20);
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
        Array<node> v(n_node);

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
    //if (!GraphIO::read(GA, G, "ERDiagram.gml", GraphIO::readGML)) {
    //    std::cerr << "Could not read ERDiagram.gml" << std::endl;
    //    return 1;
    //}

    for (node v : G.nodes)
    {
        GA.width(v) /= 2;
        GA.height(v) /= 2;
    }

    PlanarizationLayout pl;

    SubgraphPlanarizer crossMin;

    auto* ps = new PlanarSubgraphFast<int>;
    ps->runs(1000);
    VariableEmbeddingInserter *ves = new VariableEmbeddingInserter;
    ves->removeReinsert(RemoveReinsertType::All);

    crossMin.setSubgraph(ps);
    crossMin.setInserter(ves);

    EmbedderMinDepthMaxFaceLayers *emb = new EmbedderMinDepthMaxFaceLayers;
    pl.setEmbedder(emb);

    OrthoLayout *ol = new OrthoLayout;
    ol->separation(20.0);
    ol->cOverhang(0.4);
    pl.setPlanarLayouter(ol);

    pl.call(GA);

    Array<NodeElement*> arr;

    G.allNodes(arr);

    int i = 0;
    for (auto &n : arr)
    {
        double x = GA.x(n);
        double y = GA.y(n);

        std::cout << i << ": " << x << ", " << y << "\n";
        ++i;
    }

    GraphIO::write(GA, "output-ERDiagram.gml", GraphIO::writeGML);
    GraphIO::write(GA, "output-ERDiagram.svg", GraphIO::drawSVG);

    return 0;
}

