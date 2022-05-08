#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <a3/sll.h>

namespace a3 {
namespace test {
namespace sll {

using namespace testing;

struct SLLNode {
    size_t data;                // NOLINT(misc-non-private-member-variables-in-classes)
    A3_S_LINK(SLLNode) link {}; // NOLINT(misc-non-private-member-variables-in-classes)

    explicit SLLNode(size_t d) : data { d } {}
};

class SLLTest : public Test {
protected:
    A3_SLL(NodeList, SLLNode) list {}; // NOLINT(misc-non-private-member-variables-in-classes)

    void SetUp() override { A3_SLL_INIT(&list); }
};

TEST_F(SLLTest, init) { EXPECT_TRUE(A3_SLL_IS_EMPTY(&list)); }

TEST_F(SLLTest, push_pop) {
    auto* t = new SLLNode { 1234 };
    A3_SLL_PUSH(&list, t, link);

    auto* p = A3_SLL_HEAD(&list);
    EXPECT_EQ(p->data, 1234ULL);
    EXPECT_FALSE(p->link.next);
    EXPECT_EQ(p, t);

    A3_SLL_POP(&list, link);
    EXPECT_TRUE(A3_SLL_IS_EMPTY(&list));

    delete p;
}

TEST_F(SLLTest, many_insertions) {
    for (size_t i = 1; i <= 128; i++) {
        auto* n = new SLLNode { i };
        A3_SLL_PUSH(&list, n, link);
    }

    size_t i = 128;
    A3_SLL_FOR_EACH(SLLNode, node, &list, link) { EXPECT_EQ(node->data, i--); }

    EXPECT_EQ(i, 0ULL);

    while (auto* p = A3_SLL_HEAD(&list)) {
        A3_SLL_POP(&list, link);
        delete p;
    }

    for (i = 1; i <= 128; i++) {
        auto* n = new SLLNode { i };
        A3_SLL_ENQUEUE(&list, n, link);
    }

    i = 1;
    A3_SLL_FOR_EACH(SLLNode, node, &list, link) { EXPECT_EQ(node->data, i++); }

    EXPECT_EQ(i, 129ULL);

    while (auto* p = A3_SLL_HEAD(&list)) {
        A3_SLL_POP(&list, link);
        delete p;
    }
}

TEST_F(SLLTest, for_each_empty) {
    A3_SLL_FOR_EACH(SLLNode, node, &list, link) {
        EXPECT_THAT(false, IsTrue()) << "foreach should not loop when list is empty.";
    }
}

} // namespace sll
} // namespace test
} // namespace a3
