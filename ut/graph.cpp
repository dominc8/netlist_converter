#include "graph.hpp"

#include <gtest/gtest.h>

class graph_tests : public ::testing::Test
{
public:
    graph g;
};


TEST_F(graph_tests, count_neighbours)
{
    constexpr int32_t n_node = 5;
    constexpr int32_t n_component_node = 4;
    g.init(n_node, n_component_node);

    for (int32_t i = 0; i < n_component_node; ++i)
    {
        g.edge_of(0, i+1) = 1;
        g.edge_of(i+1, 0) = 1;
    }

    EXPECT_EQ(g.count_neighbours(0), n_component_node);
    EXPECT_EQ(g.count_neighbours(1), 1);
    EXPECT_EQ(g.count_neighbours(2), 1);
    EXPECT_EQ(g.count_neighbours(3), 1);
    EXPECT_EQ(g.count_neighbours(4), 1);
}

