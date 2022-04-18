#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <a3/ll.h>
#include <a3/util.h>

namespace a3 {
namespace test {
namespace ll {

using namespace testing;

struct LLNode {
    size_t data;    // NOLINT(misc-non-private-member-variables-in-classes)
    A3LL   link {}; // NOLINT(misc-non-private-member-variables-in-classes)

    explicit LLNode(size_t d) : data { d } {}
};

class LLTest : public Test {
protected:
    A3LL list {}; // NOLINT(misc-non-private-member-variables-in-classes)

    void SetUp() override { a3_ll_init(&list); }
};

TEST_F(LLTest, init) {
    EXPECT_EQ(list.next, list.prev);
    EXPECT_EQ(list.next, &list);
}

TEST_F(LLTest, enqueue_dequeue) {
    auto* t = new LLNode { 1234 };
    a3_ll_enqueue(&list, &t->link);

    auto* p = A3_CONTAINER_OF(a3_ll_peek(&list), LLNode, link);
    EXPECT_EQ(p->data, 1234ULL);
    EXPECT_EQ(p->link.prev, &list);
    EXPECT_EQ(p->link.next, &list);
    EXPECT_EQ(list.next, &p->link);
    EXPECT_EQ(list.prev, &p->link);
    EXPECT_EQ(p, t);

    p = A3_CONTAINER_OF(a3_ll_dequeue(&list), LLNode, link);
    EXPECT_EQ(p, t);
    EXPECT_EQ(list.next, list.prev);
    EXPECT_EQ(list.next, &list);

    delete p; // NOLINT(clang-analyzer-cplusplus.NewDelete)
}

TEST_F(LLTest, many_insertions) {
    for (size_t i = 0; i < 128; i++)
        a3_ll_enqueue(&list, &(new LLNode { i })->link);

    auto* mid_node = A3_CONTAINER_OF(list.prev, LLNode, link);

    for (size_t i = 129; i < 513; i++)
        a3_ll_enqueue(&list, &(new LLNode { i })->link);

    a3_ll_insert_after(&mid_node->link, &(new LLNode { 128 })->link);

    size_t i = 0;
    A3_LL_FOR_EACH(LLNode, node, &list, link) {
        auto data = node->data;
        EXPECT_EQ(data, i);
        if (data != i)
            break;
        i++;
    }

    EXPECT_EQ(i, 513ULL);

    A3_LL_FOR_EACH(LLNode, node, &list, link) { delete node; }
}

TEST_F(LLTest, for_each_empty) {
    A3_LL_FOR_EACH(LLNode, node, &list, link) {
        EXPECT_THAT(false, IsTrue()) << "foreach should not loop when list is empty.";
    }
}

} // namespace ll
} // namespace test
} // namespace a3
