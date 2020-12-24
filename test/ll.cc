#include <gtest/gtest.h>

#include <a3/ll.h>

LL_DEFINE_STRUCTS(LLNode);

struct LLNode {
    size_t data;
    LL_NODE(LLNode) {};

    explicit LLNode(size_t d) : data { d } {}
};

LL_DECLARE_METHODS(LLNode);
LL_DEFINE_METHODS(LLNode);

class LLTest : public ::testing::Test {
protected:
    LL(LLNode) list {};

    void SetUp() override { LL_INIT(LLNode)(&list); }
};

TEST_F(LLTest, init) {
    EXPECT_EQ(list.head.next, &list.end);
    EXPECT_EQ(&list.head, list.end.prev);
}

TEST_F(LLTest, enqueue_dequeue) {
    auto* t = new LLNode { 1234 };
    LL_ENQUEUE(LLNode)(&list, t);

    auto* p = LL_PEEK(LLNode)(&list);
    EXPECT_EQ(p->data, 1234ULL);
    EXPECT_EQ(p->_ll_ptr.prev, &list.head);
    EXPECT_EQ(p->_ll_ptr.next, &list.end);
    EXPECT_EQ(list.head.next, &p->_ll_ptr);
    EXPECT_EQ(list.end.prev, &p->_ll_ptr);
    EXPECT_EQ(p, t);

    p = LL_DEQUEUE(LLNode)(&list);
    EXPECT_EQ(p, t);
    EXPECT_EQ(list.head.next, &list.end);
    EXPECT_EQ(&list.head, list.end.prev);

    delete p;
}

TEST_F(LLTest, many_insertions) {
    for (size_t i = 0; i < 128; i++)
        LL_ENQUEUE(LLNode)(&list, new LLNode { i });

    auto* mid_node = LL_NODE_CONTAINER_OF(LLNode)(list.end.prev);

    for (size_t i = 129; i < 513; i++)
        LL_ENQUEUE(LLNode)(&list, new LLNode { i });

    LL_INSERT_AFTER(LLNode)(mid_node, new LLNode { 128 });

    size_t i = 0;
    for (auto* node = LL_PEEK(LLNode)(&list); node;
         node       = LL_NEXT(LLNode)(&list, node), i++)
        EXPECT_EQ(node->data, i);

    EXPECT_EQ(i, 513ULL);

    for (auto* node = LL_DEQUEUE(LLNode)(&list); node;
         node       = LL_DEQUEUE(LLNode)(&list))
        delete node;
}
