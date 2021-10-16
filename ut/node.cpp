#include "node.hpp"

#include <gtest/gtest.h>

class node_tests : public ::testing::Test
{
public:
    void compare_results(int32_t n_nodes)
    {
        for (int32_t i = 0; i < n_nodes; ++i)
        {
            EXPECT_EQ(nodes[i].comp_type, ref_nodes[i].comp_type);
            EXPECT_STREQ(nodes[i].name, ref_nodes[i].name);
            EXPECT_STREQ(nodes[i].val, ref_nodes[i].val);
        }
    }

    std::vector<node> nodes;
    std::vector<node> ref_nodes;
};


TEST_F(node_tests, extract_node_from_line_view)
{
    constexpr int32_t n_nodes = 3;
    line_view view("p0", "p1", "val", "name", component_type::C);

    node_extract(nodes, view);

    ref_nodes.reserve(n_nodes);
    ref_nodes.emplace_back("val", "name", component_type::C);
    ref_nodes.emplace_back("val", "p0", component_type::DotPoint);
    ref_nodes.emplace_back("val", "p1", component_type::DotPoint);

    compare_results(n_nodes);
}

TEST_F(node_tests, ignore_node_with_duplicated_name)
{
    constexpr int32_t n_nodes = 3;
    line_view view("p0", "p1", "val", "name", component_type::C);
    line_view view_duplicate("x0", "x1", "val2", "name", component_type::L);

    node_extract(nodes, view);
    node_extract(nodes, view_duplicate);

    ref_nodes.reserve(n_nodes);
    ref_nodes.emplace_back("val", "name", component_type::C);
    ref_nodes.emplace_back("val", "p0", component_type::DotPoint);
    ref_nodes.emplace_back("val", "p1", component_type::DotPoint);

    compare_results(n_nodes);
}

