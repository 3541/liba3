#include <utility>

#include <gtest/gtest.h>

#define A3_REF_PUBLIC
#include <a3/rc.hh>
#include <a3/util.hh>

using std::move;

using a3::Rc;
using a3::RefCounted;

class TestObject : public RefCounted<TestObject> {
    A3_PINNED(TestObject);

private:
    size_t  value { 0 };
    size_t& construct_count;
    size_t& destruct_count;

public:
    explicit TestObject(size_t v, size_t& cc, size_t& dc) :
        value { v }, construct_count { cc }, destruct_count { dc } {
        construct_count++;
    }

    ~TestObject() { destruct_count++; }

    size_t get_value() const { return value; }
};

struct TestObjectC {
    A3_REFCOUNTED;
};

TEST(Rc, c_interface) {
    auto* o = new TestObjectC;

    A3_REF_INIT(o);
    EXPECT_EQ(A3_REF_COUNT(o), 1U);

    A3_REF(o);
    EXPECT_EQ(A3_REF_COUNT(o), 2U);

    bool d_called = false;
    auto d        = [&d_called](TestObjectC*) { d_called = true; };

    A3_UNREF_D(o, d);
    EXPECT_EQ(A3_REF_COUNT(o), 1U);
    EXPECT_FALSE(d_called);

    A3_UNREF_D(o, d);
    EXPECT_EQ(A3_REF_COUNT(o), 0U);
    EXPECT_TRUE(d_called);

    delete o;
}

TEST(Rc, basic) {
    size_t c_count = 0;
    size_t d_count = 0;

    auto* o = new TestObject { 42, c_count, d_count };
    EXPECT_EQ(o->get_value(), 42ULL);
    EXPECT_EQ(o->ref_count(), 1U);
    EXPECT_EQ(c_count, 1U);
    EXPECT_EQ(d_count, 0U);

    o->ref();
    EXPECT_EQ(o->ref_count(), 2U);

    o->unref();
    EXPECT_EQ(o->ref_count(), 1U);
    EXPECT_EQ(d_count, 0U);

    o->unref();
    EXPECT_EQ(d_count, 1U);
}

TEST(Rc, wrapper_adopt) {
    size_t c_count = 0;
    size_t d_count = 0;

    {
        auto* o = new TestObject { 42, c_count, d_count };
        EXPECT_EQ(o->get_value(), 42U);
        EXPECT_EQ(o->ref_count(), 1U);
        EXPECT_EQ(c_count, 1U);
        EXPECT_EQ(d_count, 0U);
        Rc<TestObject> r = Rc<TestObject>::adopt(o);
        EXPECT_EQ(o->get_value(), 42U);
        EXPECT_EQ(o->ref_count(), 1U);
        EXPECT_EQ(c_count, 1U);
        EXPECT_EQ(d_count, 0U);
    }

    EXPECT_EQ(d_count, 1U);
}

TEST(Rc, wrapper_construct) {
    size_t c_count = 0;
    size_t d_count = 0;

    {
        auto r = Rc<TestObject>::create(42U, c_count, d_count);
        EXPECT_EQ(r->get_value(), 42U);
        EXPECT_EQ(r->ref_count(), 1U);
        EXPECT_EQ(c_count, 1U);
        EXPECT_EQ(d_count, 0U);
    }

    EXPECT_EQ(d_count, 1U);
}

TEST(Rc, wrapper_clone_and_move) {
    size_t c_count = 0;
    size_t d_count = 0;

    {
        auto r = Rc<TestObject>::create(42U, c_count, d_count);
        EXPECT_EQ(r->get_value(), 42U);
        EXPECT_EQ(r->ref_count(), 1U);
        EXPECT_EQ(c_count, 1U);
        EXPECT_EQ(d_count, 0U);

        {
            auto r1 = r;
            EXPECT_EQ(r1->get_value(), 42U);
            EXPECT_EQ(r->ref_count(), r1->ref_count());
            EXPECT_EQ(r->ref_count(), 2U);

            auto r2 = Rc<TestObject> { r1 };
            EXPECT_EQ(r2->get_value(), 42U);
            EXPECT_EQ(r->ref_count(), r1->ref_count());
            EXPECT_EQ(r->ref_count(), 3U);

            auto r3 = std::move(r2);
            EXPECT_FALSE(r2); // NOLINT(bugprone-use-after-move, clang-analyzer-cplusplus.Move)
            EXPECT_EQ(r3->get_value(), 42U);
            EXPECT_EQ(r->ref_count(), r3->ref_count());
            EXPECT_EQ(r->ref_count(), 3U);
        }

        EXPECT_EQ(r->ref_count(), 1U);
        EXPECT_EQ(d_count, 0U);
    }

    EXPECT_EQ(d_count, 1U);
}
