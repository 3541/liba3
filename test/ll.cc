#include <gtest/gtest.h>

#include <a3/ll.h>

LL_DEFINE_STRUCTS(TestStruct);

struct TestStruct {
    size_t data;
    LL_NODE(TestStruct) {};

    explicit TestStruct(size_t d) : data { d } {}
};

LL_DECLARE_METHODS(TestStruct);
LL_DEFINE_METHODS(TestStruct);

class LLTest : public ::testing::Test {
protected:
    LL(TestStruct) list {};

    void SetUp() override {
        LL_INIT(TestStruct)(&list);
    }
};

TEST_F(LLTest, init) {
    EXPECT_EQ(list.head.next, &list.end);
    EXPECT_EQ(&list.head, list.end.prev);
}

TEST_F(LLTest, enqueue_dequeue) {
    auto t = new TestStruct { 1234 };
    LL_ENQUEUE(TestStruct)(&list, t);

    auto p = LL_PEEK(TestStruct)(&list);
    EXPECT_EQ(p->data, 1234);
    EXPECT_EQ(p->_ll_ptr.prev, &list.head);
    EXPECT_EQ(p->_ll_ptr.next, &list.end);
    EXPECT_EQ(list.head.next, &p->_ll_ptr);
    EXPECT_EQ(list.end.prev, &p->_ll_ptr);
    EXPECT_EQ(p, t);

    p = LL_DEQUEUE(TestStruct)(&list);
    EXPECT_EQ(p, t);
    EXPECT_EQ(list.head.next, &list.end);
    EXPECT_EQ(&list.head, list.end.prev);

    delete p;
}

TEST_F(LLTest, many_insertions) {
    for (size_t i = 0; i < 128; i++) {
        LL_ENQUEUE(TestStruct)(&list, new TestStruct { i });
    }

    auto* mid_node = LL_NODE_CONTAINER_OF(TestStruct)(list.end.prev);

    for (size_t i = 129; i < 513; i++) {
        LL_ENQUEUE(TestStruct)(&list, new TestStruct { i });
    }

    LL_INSERT_AFTER(TestStruct)(mid_node, new TestStruct { 128 });

    auto* node = LL_PEEK(TestStruct)(&list);
    size_t i = 0;
    while (node) {
        EXPECT_EQ(node->data, i);
        node = LL_NEXT(TestStruct)(&list, node);
        i++;
    }
}
