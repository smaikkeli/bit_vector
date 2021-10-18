#ifndef TEST_RLE_MANAGE_HPP
#define TEST_RLE_MANAGE_HPP

#include <cstdint>
#include <iostream>

#include "../deps/googletest/googletest/include/gtest/gtest.h"

template<class alloc, class node, class leaf>
void node_split_rle_test() {
    alloc* a = new alloc();
    leaf* l = a->template allocate_leaf<leaf>(32, (~uint32_t(0)) >> 1, false);
    node* n = a->template allocate_node<node>();
    n->append_child(l);
    n->insert(300, true, a);

    EXPECT_EQ(n->child_count(), 2u);
    EXPECT_EQ(n->size(), ((~uint32_t(0)) >> 1) + 1u);
    EXPECT_EQ(n->p_sum(), 1u);
    EXPECT_EQ(n->at(300), true);
    for (size_t i = 0; i < n->size(); i += 10000) {
        EXPECT_EQ(n->at(i), false);
        EXPECT_EQ(n->rank(i), (i > 300) * 1u);
    }
    EXPECT_EQ(n->select(1), 300u);

    n->deallocate(a);
    a->deallocate_leaf(n);
    delete a;
}


#endif