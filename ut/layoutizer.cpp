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

