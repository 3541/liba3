/*
 * RC -- Reference counting.
 *
 * Copyright (c) 2021, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <cassert>
#include <cstddef>
#include <utility>

#include <a3/rc.h>

namespace a3 {

template <typename T, typename C>
class Rc;

template <typename T, typename C = uint32_t>
class RefCounted {
private:
    A3_REFCOUNTED_T(C);

    static void destroy(RefCounted* o) { delete static_cast<T*>(o); }

protected:
    RefCounted() { A3_REF_INIT(this); }

    ~RefCounted() { assert(A3_REF_COUNT(this) == 0); }

    void ref() { A3_REF(this); }

    void unref() { A3_UNREF_D(this, destroy); }

public:
    C ref_count() const { return _ref_count; }
};

template <typename T, typename C = uint32_t>
class Rc {
private:
    RefCounted<T, C>* ptr;

    Rc() : ptr { nullptr } {}

public:
    explicit Rc(T* target) : ptr { target } { ptr->ref(); }

    Rc(const Rc& other) : ptr { other.ptr } { ptr->ref(); }

    Rc(Rc&& other) : ptr { other.ptr } { other.ptr = nullptr; }

    Rc& operator=(const Rc& other) {
        ptr = other.ptr;
        ptr->ref();
        return *this;
    }

    Rc& operator=(Rc&& other) {
        ptr       = other.ptr;
        other.ptr = nullptr;
    }

    ~Rc() {
        if (!ptr)
            return;

        assert(ptr->ref_count());
        ptr->unref();
    }

    static Rc adopt(T* target) {
        Rc ret;
        ret.ptr = target;
        return ret;
    }

    template <typename... Args>
    static Rc<T, C> create(Args&&... args) {
        return Rc<T, C>::adopt(new T { std::forward<Args>(args)... });
    }

    T& operator*() {
        assert(ptr);
        return *static_cast<T*>(ptr);
    }

    T* operator->() {
        assert(ptr);
        return static_cast<T*>(ptr);
    }
};

} // namespace a3
