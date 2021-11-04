#include "layoutizer.hpp"

#include <gtest/gtest.h>

class layoutizer_tests : public ::testing::Test
{
protected:
    void check_node_in_corner(const node &n, const node &n1, const node &n2, bool expected_flag)
    {
        bool is_in_corner = node_is_in_corner(n, n1, n2);
        EXPECT_EQ(is_in_corner, expected_flag);
    }
    void check_neighbours(const graph &g, int32_t node_idx, int32_t neighbour1, int32_t neighbour2)
    {
        int32_t neighbours[2];
        get_neighbours(g, node_idx, &neighbours[0]);
        bool are_proper_neighbours = ((neighbour1 == neighbours[0]) && (neighbour2 == neighbours[1])) ||
                                     ((neighbour1 == neighbours[1]) && (neighbour2 == neighbours[0]));
        EXPECT_EQ(are_proper_neighbours, true);
    }
};


TEST_F(layoutizer_tests, node_is_not_in_corner)
{
    node n1, n2, n3;

    /*
     * n3
     * n2
     * n1
     * */
    n1.set_coord(0, 0);
    n2.set_coord(0, 10);
    n3.set_coord(0, 20);
    check_node_in_corner(n2, n1, n3, false);
    check_node_in_corner(n2, n3, n1, false);

    /*
     * n1 n2 n3
     * */
    n1.set_coord(0, 0);
    n2.set_coord(10, 0);
    n3.set_coord(20, 0);
    check_node_in_corner(n2, n1, n3, false);
    check_node_in_corner(n2, n3, n1, false);
}

TEST_F(layoutizer_tests, node_is_in_corner)
{
    node n1, n2, n3;

    /*
     * n2 n3
     * n1
     * */
    n1.set_coord(0, 10);
    n2.set_coord(0, 20);
    n3.set_coord(30, 20);
    check_node_in_corner(n2, n1, n3, true);
    check_node_in_corner(n2, n3, n1, true);

    /*
     * n1 n2
     *    n3
     * */
    n1.set_coord(0, 20);
    n2.set_coord(10, 20);
    n3.set_coord(10, 15);
    check_node_in_corner(n2, n1, n3, true);
    check_node_in_corner(n2, n3, n1, true);

    /*
     *    n1
     * n3 n2
     * */
    n1.set_coord(30, 20);
    n2.set_coord(30, 0);
    n3.set_coord(10, 0);
    check_node_in_corner(n2, n1, n3, true);
    check_node_in_corner(n2, n3, n1, true);

    /*
     * n1
     * n2 n3
     * */
    n1.set_coord(0, 30);
    n2.set_coord(0, 10);
    n3.set_coord(20, 10);
    check_node_in_corner(n2, n1, n3, true);
    check_node_in_corner(n2, n3, n1, true);
}

TEST_F(layoutizer_tests, get_neighbours)
{
    /*
     * 0 - 1
     * |   |
     * 2 - 3
     * */
    graph g;
    g.init(4, 2);
    g.edge_of(0, 1) = 1;
    g.edge_of(0, 2) = 1;
    g.edge_of(1, 0) = 1;
    g.edge_of(2, 0) = 1;
    g.edge_of(3, 1) = 1;
    g.edge_of(3, 2) = 1;
    g.edge_of(1, 3) = 1;
    g.edge_of(2, 3) = 1;

    check_neighbours(g, 0, 1, 2);
    check_neighbours(g, 1, 0, 3);
    check_neighbours(g, 2, 0, 3);
    check_neighbours(g, 3, 1, 2);
}

