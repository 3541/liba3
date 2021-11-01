#include <gtest/gtest.h>

#include <a3/sll.h>
#include <a3/util.h>

struct SLLNode {
    size_t data;
    A3SLink link { NULL };

    explicit SLLNode(size_t d) : data { d } {}
};


class SLLTest : public ::testing::Test {
protected:
    A3SLL list;

    void SetUp() override { a3_sll_init(&list); }
};

TEST_F(SLLTest, init) { EXPECT_FALSE(list.head); }

TEST_F(SLLTest, push_pop) {
    auto* t = new SLLNode { 1234 };
    a3_sll_push(&list, &t->link);

    auto* p = A3_CONTAINER_OF(a3_sll_peek(&list), SLLNode, link);
    EXPECT_EQ(p->data, 1234ULL);
    EXPECT_FALSE(p->link.next);
    EXPECT_EQ(list.head, &p->link);
    EXPECT_EQ(p, t);

    p = A3_CONTAINER_OF(a3_sll_pop(&list), SLLNode, link);
    EXPECT_EQ(p, t);
    EXPECT_FALSE(list.head);

    delete p;
}

TEST_F(SLLTest, many_insertions) {
    for (size_t i = 1; i <= 128; i++)
        a3_sll_push(&list, &(new SLLNode { i })->link);

    size_t i = 128;
    A3_SLL_FOREACH(node, &list)
        EXPECT_EQ(A3_CONTAINER_OF(node, SLLNode, link)->data, i--);

    EXPECT_EQ(i, 0ULL);

    while (auto* p = a3_sll_pop(&list))
        delete A3_CONTAINER_OF(p, SLLNode, link);

    for (i = 1; i <= 128; i++)
        a3_sll_enqueue(&list, &(new SLLNode { i })->link);

    i = 1;
    A3_SLL_FOREACH(node, &list)
        EXPECT_EQ(A3_CONTAINER_OF(node, SLLNode, link)->data, i++);

    EXPECT_EQ(i, 129ULL);

    while (auto* p = a3_sll_dequeue(&list))
        delete A3_CONTAINER_OF(p, SLLNode, link);
}
