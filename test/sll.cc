#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <a3/sll.h>

namespace a3 {
namespace test {
namespace sll {

using namespace testing;

struct SLLNode {
    unsigned int data;            // NOLINT(misc-non-private-member-variables-in-classes)
    A3_SLL_LINK(SLLNode) link {}; // NOLINT(misc-non-private-member-variables-in-classes)

    explicit SLLNode(unsigned int d) : data { d } {}
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
    EXPECT_EQ(p->data, 1234U);
    EXPECT_THAT(A3_SLL_NEXT(p, link), IsNull());
    EXPECT_EQ(p, t);

    A3_SLL_POP(&list, link);
    EXPECT_TRUE(A3_SLL_IS_EMPTY(&list));

    delete p;
}

TEST_F(SLLTest, many_insertions) {
    for (unsigned int i = 1; i <= 128; i++) {
        auto* n = new SLLNode { i };
        A3_SLL_PUSH(&list, n, link);
    }

    unsigned int i = 128;
    A3_SLL_FOR_EACH (SLLNode, node, &list, link) { EXPECT_EQ(node->data, i--); }

    EXPECT_EQ(i, 0U);

    while (auto* p = A3_SLL_HEAD(&list)) {
        A3_SLL_POP(&list, link);
        delete p;
    }

    for (i = 1; i <= 128; i++) {
        auto* n = new SLLNode { i };
        A3_SLL_ENQUEUE(&list, n, link);
    }

    i = 1;
    A3_SLL_FOR_EACH (SLLNode, node, &list, link) { EXPECT_EQ(node->data, i++); }

    EXPECT_EQ(i, 129U);
    EXPECT_THAT(A3_SLL_NEXT(A3_SLL_HEAD(&list), link)->data, Eq(2U));

    while (auto* p = A3_SLL_HEAD(&list)) {
        A3_SLL_POP(&list, link);
        delete p;
    }
}

TEST_F(SLLTest, for_each_empty) {
    A3_SLL_FOR_EACH (SLLNode, node, &list, link) {
        EXPECT_THAT(false, IsTrue()) << "foreach should not loop when list is empty.";
    }
}

TEST_F(SLLTest, insert_after) {
    for (unsigned int i = 1; i <= 128; i++) {
        auto* n = new SLLNode { i };
        A3_SLL_ENQUEUE(&list, n, link);
    }

    auto* end = A3_SLL_END(&list, SLLNode, link);
    EXPECT_THAT(end, NotNull());
    EXPECT_THAT(end->data, Eq(128U));

    for (unsigned int i = 130; i <= 256; i++) {
        auto* n = new SLLNode { i };
        A3_SLL_ENQUEUE(&list, n, link);
    }

    auto* next = new SLLNode { 129 };
    A3_SLL_INSERT_AFTER(end, next, link);

    unsigned int i = 1;
    A3_SLL_FOR_EACH (SLLNode, node, &list, link) { EXPECT_EQ(node->data, i++); }
    EXPECT_THAT(i, Eq(257U));

    while (auto* p = A3_SLL_HEAD(&list)) {
        A3_SLL_POP(&list, link);
        delete p;
    }
}

TEST_F(SLLTest, remove_first) {
    for (unsigned int i = 1; i <= 128; i++) {
        auto* n = new SLLNode { i };
        A3_SLL_ENQUEUE(&list, n, link);
    }

    auto* first = A3_SLL_HEAD(&list);
    ASSERT_THAT(first, NotNull());
    EXPECT_THAT(first->data, Eq(1U));

    A3_SLL_REMOVE(&list, first, SLLNode, link);
    delete first;

    first = A3_SLL_HEAD(&list);
    ASSERT_THAT(first, NotNull());
    EXPECT_THAT(first->data, Eq(2U));

    unsigned int i = 2;
    while (auto* p = A3_SLL_HEAD(&list)) {
        A3_SLL_POP(&list, link);
        EXPECT_EQ(p->data, i++);
        delete p;
    }
}

TEST_F(SLLTest, remove_last) {
    for (unsigned int i = 1; i <= 128; i++) {
        auto* n = new SLLNode { i };
        A3_SLL_ENQUEUE(&list, n, link);
    }

    auto* last = A3_SLL_END(&list, SLLNode, link);
    ASSERT_THAT(last, NotNull());
    EXPECT_THAT(last->data, Eq(128U));

    A3_SLL_REMOVE(&list, last, SLLNode, link);
    delete last;

    last = A3_SLL_END(&list, SLLNode, link);
    ASSERT_THAT(last, NotNull());
    EXPECT_THAT(last->data, Eq(127U));

    unsigned int i = 1;
    while (auto* p = A3_SLL_HEAD(&list)) {
        A3_SLL_POP(&list, link);
        EXPECT_THAT(p->data, Eq(i++));
        EXPECT_THAT(p->data, Lt(128U));
        delete p;
    }
}

TEST_F(SLLTest, remove_mid) {
    for (unsigned int i = 1; i <= 128; i++) {
        auto* n = new SLLNode { i };
        A3_SLL_ENQUEUE(&list, n, link);
    }

    SLLNode* mid = nullptr;
    A3_SLL_FOR_EACH (SLLNode, n, &list, link) {
        if (n->data == 64U) {
            mid = n;
            break;
        }
    }
    ASSERT_THAT(mid, NotNull());
    EXPECT_THAT(mid->data, Eq(64U));

    A3_SLL_REMOVE(&list, mid, SLLNode, link);
    delete mid;

    SLLNode* before = nullptr;
    SLLNode* after  = nullptr;
    A3_SLL_FOR_EACH (SLLNode, n, &list, link) {
        if (n->data == 63U) {
            before = n;
            after  = A3_SLL_NEXT(before, link);
            break;
        }
    }
    ASSERT_THAT(before, NotNull());
    ASSERT_THAT(after, NotNull());
    EXPECT_THAT(before->data, Eq(63U));
    EXPECT_THAT(after->data, Eq(65U));

    unsigned int i = 1;
    while (auto* p = A3_SLL_HEAD(&list)) {
        A3_SLL_POP(&list, link);
        EXPECT_THAT(p->data, Eq(i++));
        EXPECT_THAT(p->data, Ne(64U));

        if (i == 64)
            i++;
        delete p;
    }
}

TEST_F(SLLTest, nested_for_each) {
    for (unsigned i = 1; i <= 128; i++) {
        auto* n = new SLLNode { i };
        A3_SLL_ENQUEUE(&list, n, link);
    }

    unsigned outer = 1;
    A3_SLL_FOR_EACH (SLLNode, node_outer, &list, link) {
        EXPECT_THAT(node_outer->data, Eq(outer++));

        unsigned inner = 1;
        A3_SLL_FOR_EACH (SLLNode, node_inner, &list, link)
            EXPECT_THAT(node_inner->data, Eq(inner++));
    }

    A3_SLL_FOR_EACH (SLLNode, node, &list, link)
        delete node;
}

} // namespace sll
} // namespace test
} // namespace a3
