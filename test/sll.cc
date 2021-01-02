#include <gtest/gtest.h>

#include <a3/sll.h>

struct SLLNode {
    size_t data;
    SLL_NODE(SLLNode);

    explicit SLLNode(size_t d) : data { d } {}
};

SLL_DEFINE_STRUCTS(SLLNode);

SLL_DECLARE_METHODS(SLLNode);
SLL_DEFINE_METHODS(SLLNode);

class SLLTest : public ::testing::Test {
protected:
    SLL(SLLNode) list {};

    void SetUp() override { SLL_INIT(SLLNode)(&list); }
};

TEST_F(SLLTest, init) { EXPECT_FALSE(list.head); }

TEST_F(SLLTest, push_pop) {
    auto* t = new SLLNode { 1234 };
    SLL_PUSH(SLLNode)(&list, t);

    auto* p = SLL_PEEK(SLLNode)(&list);
    EXPECT_EQ(p->data, 1234ULL);
    EXPECT_FALSE(p->_sll_next);
    EXPECT_EQ(list.head, p);
    EXPECT_EQ(p, t);

    p = SLL_POP(SLLNode)(&list);
    EXPECT_EQ(p, t);
    EXPECT_FALSE(list.head);

    delete p;
}

TEST_F(SLLTest, many_insertions) {
    for (size_t i = 1; i <= 128; i++)
        SLL_PUSH(SLLNode)(&list, new SLLNode { i });

    size_t i = 128;
    for (auto* node = SLL_PEEK(SLLNode)(&list); node && i;
         node       = SLL_NEXT(SLLNode)(node), i--)
        EXPECT_EQ(node->data, i);

    EXPECT_EQ(i, 0ULL);

    for (auto* node = SLL_POP(SLLNode)(&list); node;
         node       = SLL_POP(SLLNode)(&list))
        delete node;

    for (i = 1; i <= 128; i++)
        SLL_ENQUEUE(SLLNode)(&list, new SLLNode { i });

    i = 1;
    for (auto* node = SLL_PEEK(SLLNode)(&list); node;
         node       = SLL_NEXT(SLLNode)(node), i++)
        EXPECT_EQ(node->data, i);

    EXPECT_EQ(i, 129ULL);

    for (auto* node = SLL_POP(SLLNode)(&list); node;
         node       = SLL_POP(SLLNode)(&list))
        delete node;
}
