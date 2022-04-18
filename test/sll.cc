#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <a3/sll.h>
#include <a3/util.h>

namespace a3 {
namespace test {
namespace sll {

using namespace testing;

struct SLLNode {
    size_t  data;             // NOLINT(misc-non-private-member-variables-in-classes)
    A3SLink link { nullptr }; // NOLINT(misc-non-private-member-variables-in-classes)

    explicit SLLNode(size_t d) : data { d } {}
};

class SLLTest : public Test {
protected:
    A3SLL list {}; // NOLINT(misc-non-private-member-variables-in-classes)

    void SetUp() override { a3_sll_init(&list); }
};

TEST_F(SLLTest, init) { EXPECT_TRUE(a3_sll_is_empty(&list)); }

TEST_F(SLLTest, push_pop) {
    auto* t = new SLLNode { 1234 };
    a3_sll_push(&list, &t->link);

    auto* p = A3_CONTAINER_OF(a3_sll_peek(&list), SLLNode, link);
    EXPECT_EQ(p->data, 1234ULL);
    EXPECT_FALSE(p->link.next);
    EXPECT_EQ(a3_sll_peek(&list), &p->link);
    EXPECT_EQ(p, t);

    p = A3_CONTAINER_OF(a3_sll_pop(&list), SLLNode, link);
    EXPECT_EQ(p, t);
    EXPECT_TRUE(a3_sll_is_empty(&list));

    delete p;
}

TEST_F(SLLTest, many_insertions) {
    for (size_t i = 1; i <= 128; i++)
        a3_sll_push(&list, &(new SLLNode { i })->link);

    size_t i = 128;
    A3_SLL_FOR_EACH(SLLNode, node, &list, link) { EXPECT_EQ(node->data, i--); }

    EXPECT_EQ(i, 0ULL);

    while (auto* p = a3_sll_pop(&list))
        delete A3_CONTAINER_OF(p, SLLNode, link);

    for (i = 1; i <= 128; i++)
        a3_sll_enqueue(&list, &(new SLLNode { i })->link);

    i = 1;
    A3_SLL_FOR_EACH(SLLNode, node, &list, link) { EXPECT_EQ(node->data, i++); }

    EXPECT_EQ(i, 129ULL);

    while (auto* p = a3_sll_dequeue(&list))
        delete A3_CONTAINER_OF(p, SLLNode, link);
}

TEST_F(SLLTest, for_each_empty) {
    A3_SLL_FOR_EACH(SLLNode, node, &list, link) {
        EXPECT_THAT(false, IsTrue()) << "foreach should not loop when list is empty.";
    }
}

} // namespace sll
} // namespace test
} // namespace a3
