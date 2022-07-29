#include <cstddef>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <a3/ll.h>
#include <a3/util.h>

namespace a3 {
namespace test {
namespace ll {

using namespace testing;

struct LLNode {
    size_t data;                // NOLINT(misc-non-private-member-variables-in-classes)
    A3_LL_LINK(LLNode) link {}; // NOLINT(misc-non-private-member-variables-in-classes)

    explicit LLNode(size_t d) : data { d } {}
};

class LLTest : public Test {
protected:
    A3_LL(NodeList, LLNode) list {}; // NOLINT(misc-non-private-member-variables-in-classes)

    void SetUp() override { A3_LL_INIT(&list, link); }
};

TEST_F(LLTest, init) { EXPECT_THAT(A3_LL_IS_EMPTY(&list), IsTrue()); }

TEST_F(LLTest, push_pop) {
    auto* t = new LLNode { 1234 };
    A3_LL_PUSH(&list, t, link);

    auto* p = A3_LL_HEAD(&list);
    EXPECT_EQ(p->data, 1234U);
    EXPECT_THAT(A3_LL_NEXT(p, link), IsNull());
    EXPECT_EQ(p, t);

    A3_LL_POP(&list, link);
    EXPECT_TRUE(A3_LL_IS_EMPTY(&list));

    delete p;
}

TEST_F(LLTest, enqueue_dequeue) {
    auto* t = new LLNode { 1234 };
    A3_LL_ENQUEUE(&list, t, link);

    auto* p = A3_LL_HEAD(&list);
    EXPECT_EQ(p->data, 1234ULL);
    EXPECT_EQ(p, t);

    A3_LL_DEQUEUE(&list, link);
    EXPECT_THAT(A3_LL_IS_EMPTY(&list), IsTrue());

    delete p; // NOLINT(clang-analyzer-cplusplus.NewDelete)
}

TEST_F(LLTest, many_insertions) {
    for (size_t i = 0; i < 128; i++) {
        auto* n = new LLNode { i };
        A3_LL_ENQUEUE(&list, n, link);
    }

    auto* mid_node = A3_LL_END(&list, LLNode, link);

    for (size_t i = 129; i < 513; i++) {
        auto* n = new LLNode { i };
        A3_LL_ENQUEUE(&list, n, link);
    }

    auto* n = new LLNode { 128 };
    A3_LL_INSERT_AFTER(mid_node, n, link);

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

TEST_F(LLTest, iterate_backwards) {
    for (unsigned int i = 0; i <= 128; i++) {
        auto* n = new LLNode { i };
        A3_LL_ENQUEUE(&list, n, link);
    }

    unsigned int i = 128;
    A3_LL_FOR_EACH_REV(LLNode, item, &list, link) { EXPECT_THAT(item->data, Eq(i--)); }

    A3_LL_FOR_EACH(LLNode, node, &list, link) { delete node; }
}

TEST_F(LLTest, for_each_empty) {
    A3_LL_FOR_EACH (LLNode, node, &list, link) {
        EXPECT_THAT(false, IsTrue()) << "foreach should not loop when list is empty.";
    }
}

TEST_F(LLTest, for_each_rev_empty) {
    A3_LL_FOR_EACH_REV (LLNode, node, &list, link) {
        EXPECT_THAT(false, IsTrue()) << "foreach should not loop when list is empty.";
    }
}

TEST_F(LLTest, insert_after) {
    for (unsigned int i = 1; i <= 128; i++) {
        auto* n = new LLNode { i };
        A3_LL_ENQUEUE(&list, n, link);
    }

    auto* end = A3_LL_END(&list, LLNode, link);
    EXPECT_THAT(end, NotNull());
    EXPECT_THAT(end->data, Eq(128U));

    for (unsigned int i = 130; i <= 256; i++) {
        auto* n = new LLNode { i };
        A3_LL_ENQUEUE(&list, n, link);
    }

    auto* next = new LLNode { 129 };
    A3_LL_INSERT_AFTER(end, next, link);

    unsigned int i = 1;
    A3_LL_FOR_EACH(LLNode, node, &list, link) { EXPECT_EQ(node->data, i++); }
    EXPECT_THAT(i, Eq(257U));

    while (auto* p = A3_LL_HEAD(&list)) {
        A3_LL_POP(&list, link);
        delete p;
    }
}

TEST_F(LLTest, insert_before) {
    for (unsigned int i = 1; i <= 128; i++) {
        if (i == 127)
            continue;

        auto* n = new LLNode { i };
        A3_LL_ENQUEUE(&list, n, link);
    }

    auto* end = A3_LL_END(&list, LLNode, link);
    EXPECT_THAT(end, NotNull());
    EXPECT_THAT(end->data, Eq(128U));

    for (unsigned int i = 129; i <= 256; i++) {
        auto* n = new LLNode { i };
        A3_LL_ENQUEUE(&list, n, link);
    }

    auto* prev = new LLNode { 127 };
    A3_LL_INSERT_BEFORE(end, prev, link);

    unsigned int i = 1;
    A3_LL_FOR_EACH(LLNode, node, &list, link) { EXPECT_EQ(node->data, i++); }
    EXPECT_THAT(i, Eq(257U));

    while (auto* p = A3_LL_HEAD(&list)) {
        A3_LL_POP(&list, link);
        delete p;
    }
}

TEST_F(LLTest, remove_first) {
    for (unsigned int i = 1; i <= 128; i++) {
        auto* n = new LLNode { i };
        A3_LL_ENQUEUE(&list, n, link);
    }

    auto* first = A3_LL_HEAD(&list);
    ASSERT_THAT(first, NotNull());
    EXPECT_THAT(first->data, Eq(1U));

    A3_LL_REMOVE(first, link);
    delete first;

    first = A3_LL_HEAD(&list);
    ASSERT_THAT(first, NotNull());
    EXPECT_THAT(first->data, Eq(2U));

    unsigned int i = 2;
    while (auto* p = A3_LL_HEAD(&list)) {
        A3_LL_POP(&list, link);
        EXPECT_EQ(p->data, i++);
        delete p;
    }
}

TEST_F(LLTest, remove_last) {
    for (unsigned int i = 1; i <= 128; i++) {
        auto* n = new LLNode { i };
        A3_LL_ENQUEUE(&list, n, link);
    }

    auto* last = A3_LL_END(&list, LLNode, link);
    ASSERT_THAT(last, NotNull());
    EXPECT_THAT(last->data, Eq(128U));

    A3_LL_REMOVE(last, link);
    delete last;

    last = A3_LL_END(&list, LLNode, link);
    ASSERT_THAT(last, NotNull());
    EXPECT_THAT(last->data, Eq(127U));

    unsigned int i = 1;
    while (auto* p = A3_LL_HEAD(&list)) {
        A3_LL_POP(&list, link);
        EXPECT_THAT(p->data, Eq(i++));
        EXPECT_THAT(p->data, Lt(128U));
        delete p;
    }
}

TEST_F(LLTest, remove_mid) {
    for (unsigned int i = 1; i <= 128; i++) {
        auto* n = new LLNode { i };
        A3_LL_ENQUEUE(&list, n, link);
    }

    LLNode* mid = nullptr;
    A3_LL_FOR_EACH(LLNode, n, &list, link) {
        if (n->data == 64U) {
            mid = n;
            break;
        }
    }
    ASSERT_THAT(mid, NotNull());
    EXPECT_THAT(mid->data, Eq(64U));

    A3_LL_REMOVE(mid, link);
    delete mid;

    LLNode* before = nullptr;
    LLNode* after  = nullptr;
    A3_LL_FOR_EACH(LLNode, n, &list, link) {
        if (n->data == 63U) {
            before = n;
            after  = A3_LL_NEXT(before, link);
            break;
        }
    }
    ASSERT_THAT(before, NotNull());
    ASSERT_THAT(after, NotNull());
    EXPECT_THAT(before->data, Eq(63U));
    EXPECT_THAT(after->data, Eq(65U));

    unsigned int i = 1;
    while (auto* p = A3_LL_HEAD(&list)) {
        A3_LL_POP(&list, link);
        EXPECT_THAT(p->data, Eq(i++));
        EXPECT_THAT(p->data, Ne(64U));

        if (i == 64)
            i++;
        delete p;
    }
}

TEST_F(LLTest, next_orphan) {
    auto  l = std::make_unique<LLNode>(42);
    auto* n = A3_LL_NEXT(l, link);
    EXPECT_THAT(n, IsNull());
}

TEST_F(LLTest, remove_only) {
    auto l = std::make_unique<LLNode>(42);
    A3_LL_ENQUEUE(&list, l.get(), link);
    A3_LL_REMOVE(l, link);

    EXPECT_THAT(A3_LL_IS_EMPTY(&list), IsTrue())
        << "List should be empty after removing only element.";

    auto p = std::make_unique<LLNode>(43);
    A3_LL_ENQUEUE(&list, p.get(), link);
    A3_LL_FOR_EACH(LLNode, n, &list, link) { EXPECT_THAT(n->data, Eq(43U)); }
}

} // namespace ll
} // namespace test
} // namespace a3
