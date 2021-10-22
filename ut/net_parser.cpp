#include "net_parser.hpp"

#include <gtest/gtest.h>

class net_parser_tests : public ::testing::Test
{
public:
    line_view view;
};

TEST_F(net_parser_tests, parse_empty_line)
{
    char line[] = "";
    bool ret = parse_line(&line[0], &view);
    EXPECT_EQ(ret, false);
}

TEST_F(net_parser_tests, parse_whitespace_line)
{
    char line[] = "   \t   \t   ";
    bool ret = parse_line(&line[0], &view);
    EXPECT_EQ(ret, false);
}

TEST_F(net_parser_tests, parse_wrong_line)
{
    char line[] = "11 0 1 2";
    bool ret = parse_line(&line[0], &view);
    EXPECT_EQ(ret, false);
}

TEST_F(net_parser_tests, parse_unknown_type)
{
    char token[] = "00";
    bool ret = parse_type(&token[0], &view.comp_type);
    EXPECT_EQ(ret, false);
    EXPECT_EQ(view.comp_type, component_type::Unknown);
}

TEST_F(net_parser_tests, parse_good_type)
{
    char token[] = "V0";
    bool ret = parse_type(&token[0], &view.comp_type);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(view.comp_type, component_type::V);
}

TEST_F(net_parser_tests, parse_good_str)
{
    char token[] = "v1234";
    char str[8];
    bool ret = parse_str(&token[0], &str[0], sizeof(str));
    EXPECT_EQ(ret, true);
    EXPECT_STREQ(str, token);
}

TEST_F(net_parser_tests, parse_too_long_str)
{
    char token[] = "v12341324";
    char str[8];
    bool ret = parse_str(&token[0], &str[0], sizeof(str));
    EXPECT_EQ(ret, false);
}

TEST_F(net_parser_tests, parse_bad_line_comp)
{
    char line[] = "R0";
    bool ret = parse_line(&line[0], &view);
    EXPECT_EQ(ret, false);
    EXPECT_EQ(view.comp_type, component_type::R);
    EXPECT_STREQ(view.name, "R0");
}

TEST_F(net_parser_tests, parse_bad_line_comp_p0)
{
    char line[] = "C0 v1";
    bool ret = parse_line(&line[0], &view);
    EXPECT_EQ(ret, false);
    EXPECT_EQ(view.comp_type, component_type::C);
    EXPECT_STREQ(view.name, "C0");
    EXPECT_STREQ(&view.p0[0], "v1");
}

TEST_F(net_parser_tests, parse_bad_line_comp_p0_p1)
{
    char line[] = "L2 v1 0";
    bool ret = parse_line(&line[0], &view);
    EXPECT_EQ(ret, false);
    EXPECT_EQ(view.comp_type, component_type::L);
    EXPECT_STREQ(view.name, "L2");
    EXPECT_STREQ(&view.p0[0], "v1");
    EXPECT_STREQ(&view.p1[0], "0");
}

TEST_F(net_parser_tests, parse_good_line)
{
    char line[] = "V1 0 1 2";
    bool ret = parse_line(&line[0], &view);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(view.comp_type, component_type::V);
    EXPECT_STREQ(view.name, "V1");
    EXPECT_STREQ(&view.p0[0], "0");
    EXPECT_STREQ(&view.p1[0], "1");
    EXPECT_STREQ(&view.val[0], "2");
}


class net_parser_file_tests : public ::testing::Test
{
public:
    void create_test_file(const char *filename, const char *data)
    {
        FILE *f = fopen(filename, "w");
        ASSERT_NE(f, nullptr);
        fprintf(f, "%s", data);
        fclose(f);
    }

    void compare_results(int32_t n_views)
    {
        for (int32_t i = 0; i < n_views; ++i)
        {
            EXPECT_EQ(views[i].comp_type, ref_views[i].comp_type);
            EXPECT_STREQ(views[i].name, ref_views[i].name);
            EXPECT_STREQ(views[i].p0, ref_views[i].p0);
            EXPECT_STREQ(views[i].p1, ref_views[i].p1);
            EXPECT_STREQ(views[i].val, ref_views[i].val);
        }
    }

    std::vector<line_view> views;
    std::vector<line_view> ref_views;
};


TEST_F(net_parser_file_tests, parse_simple_file)
{
    constexpr char filename[] = "simple_file.txt";
    create_test_file(filename, "  V1 r 0 1\n\n\n"
                               " R1    r c 10k\n"
                               "C1  c l    10n\n"
                               "L1 l 0 5.3   \n\n");

    FILE *f = fopen(filename, "r");
    ASSERT_NE(f, nullptr);

    views = parse_file(f);

    fclose(f);

    constexpr int32_t n_views = 4;

    ref_views.reserve(n_views);

    ref_views.emplace_back("r", "0", "1", "V1", component_type::V);
    ref_views.emplace_back("r", "c", "10k", "R1", component_type::R);
    ref_views.emplace_back("c", "l", "10n", "C1", component_type::C);
    ref_views.emplace_back("l", "0", "5.3", "L1", component_type::L);

    compare_results(n_views);
}

class net_parser_node_tests : public ::testing::Test
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

    std::vector<line_view> views;
    std::vector<node> nodes;
    std::vector<node> ref_nodes;
};


TEST_F(net_parser_node_tests, single_view)
{
    constexpr int32_t n_views = 1;
    views.reserve(n_views);

    views.emplace_back("vdd", "0", "1", "V1", component_type::V);

    nodes = parse_views(views);

    constexpr int32_t n_nodes = 1;

    ref_nodes.reserve(n_nodes);

    ref_nodes.emplace_back("1", "V1", component_type::V);

    compare_results(n_nodes);
}

TEST_F(net_parser_node_tests, multiple_unique_views)
{
    constexpr int32_t n_views = 4;
    views.reserve(n_views);

    views.emplace_back("vdd", "0", "1", "V1", component_type::V);
    views.emplace_back("r", "c", "10k", "R1", component_type::R);
    views.emplace_back("c", "l", "10n", "C1", component_type::C);
    views.emplace_back("l", "0", "5.3", "L1", component_type::L);

    nodes = parse_views(views);

    constexpr int32_t n_nodes = 9;

    ref_nodes.reserve(n_nodes);

    ref_nodes.emplace_back("1", "V1", component_type::V);
    ref_nodes.emplace_back("1", "vdd", component_type::DotPoint);
    ref_nodes.emplace_back("1", "0", component_type::DotPoint);
    ref_nodes.emplace_back("10k", "R1", component_type::R);
    ref_nodes.emplace_back("10k", "r", component_type::DotPoint);
    ref_nodes.emplace_back("10k", "c", component_type::DotPoint);
    ref_nodes.emplace_back("10n", "C1", component_type::C);
    ref_nodes.emplace_back("10n", "l", component_type::DotPoint);
    ref_nodes.emplace_back("5.3", "L1", component_type::L);

    compare_results(n_nodes);
}

TEST_F(net_parser_node_tests, multiple_nonunique_views)
{
    constexpr int32_t n_views = 4;
    views.reserve(n_views);

    views.emplace_back("vdd", "0", "1", "V1", component_type::V);
    views.emplace_back("vdd", "0", "1", "V1", component_type::V);
    views.emplace_back("r", "c", "10k", "R1", component_type::R);
    views.emplace_back("r", "c", "10M", "R1", component_type::R);
    views.emplace_back("c", "l", "10n", "C1", component_type::C);
    views.emplace_back("c", "a", "10n", "C1", component_type::C);
    views.emplace_back("l", "0", "5.3", "L1", component_type::L);
    views.emplace_back("k", "0", "5.3", "L1", component_type::L);

    nodes = parse_views(views);

    constexpr int32_t n_nodes = 9;

    ref_nodes.reserve(n_nodes);

    ref_nodes.emplace_back("1", "V1", component_type::V);
    ref_nodes.emplace_back("1", "vdd", component_type::DotPoint);
    ref_nodes.emplace_back("1", "0", component_type::DotPoint);
    ref_nodes.emplace_back("10k", "R1", component_type::R);
    ref_nodes.emplace_back("10k", "r", component_type::DotPoint);
    ref_nodes.emplace_back("10k", "c", component_type::DotPoint);
    ref_nodes.emplace_back("10n", "C1", component_type::C);
    ref_nodes.emplace_back("10n", "l", component_type::DotPoint);
    ref_nodes.emplace_back("5.3", "L1", component_type::L);

    compare_results(n_nodes);
}

class net_parser_graph_tests : public ::testing::Test
{
public:
    void compare_results()
    {
        ASSERT_EQ(g.n_node, ref_g.n_node);
        ASSERT_EQ(g.n_all_node, ref_g.n_all_node);

        int32_t mat_size = g.n_all_node * g.n_all_node;
        auto cmp = memcmp(g.matrix, ref_g.matrix, mat_size * sizeof(*g.matrix));
        EXPECT_EQ(cmp, 0);
    }

    graph g;
    graph ref_g;
    std::vector<line_view> views;
    std::vector<node> nodes;
};


TEST_F(net_parser_graph_tests, proper_graph)
{
    constexpr int32_t n_views = 4;
    views.reserve(n_views);

    views.emplace_back("r", "0", "1", "V1", component_type::V);
    views.emplace_back("vdd", "0", "1", "V1", component_type::V);
    views.emplace_back("r", "c", "10k", "R1", component_type::R);
    views.emplace_back("r", "c", "10M", "R1", component_type::R);
    views.emplace_back("c", "l", "10n", "C1", component_type::C);
    views.emplace_back("c", "a", "10n", "C1", component_type::C);
    views.emplace_back("l", "0", "5.3", "L1", component_type::L);
    views.emplace_back("k", "0", "5.3", "L1", component_type::L);

    nodes = parse_views(views);

    g = parse_nodes(nodes, views);

    constexpr int32_t n_nodes = 8;
    constexpr int32_t n_component_node = 4;
    constexpr int32_t n_all_node = n_nodes + n_component_node;
    uint8_t ref_matrix[n_all_node][n_all_node] =
    {
        { 0, 1, 1, 0, 0, 0, 0, 0 }, //V1
        { 1, 0, 0, 1, 0, 0, 0, 0 }, //r
        { 1, 0, 0, 0, 0, 0, 0, 1 }, //0
        { 0, 1, 0, 0, 1, 0, 0, 0 }, //R1
        { 0, 0, 0, 1, 0, 1, 0, 0 }, //c
        { 0, 0, 0, 0, 1, 0, 1, 0 }, //C1
        { 0, 0, 0, 0, 0, 1, 0, 1 }, //l
        { 0, 0, 1, 0, 0, 0, 1, 0 }, //L1
    };
    ref_g.n_node = n_nodes;
    ref_g.n_all_node = n_all_node;
    ref_g.matrix = &ref_matrix[0][0];
    compare_results();
}

