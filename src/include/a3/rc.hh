/*
 * RC -- Reference counting.
 *
 * Copyright (c) 2021, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

/// \file rc.hh
/// # Reference Counting (C++)
/// A convenient C++ layer on top of rc.h.

#pragma once

#include <cassert>
#include <cstddef>
#include <utility>

#include <a3/rc.h>

namespace a3 {

template <typename T, typename C>
class Rc;

/// \brief A reference-counted type. Inherit from this to enable reference counting.
///
/// RefCounted objects should be accessed through Rc pointers. If non-standard allocation is used,
/// the type should override `operator delete`.
template <typename T, typename C = uint32_t>
class RefCounted {
private:
    A3_REFCOUNTED_T(C);

    static void destroy(RefCounted* o) { delete static_cast<T*>(o); }

protected:
    RefCounted() { A3_REF_INIT(this); }

    ~RefCounted() { assert(A3_REF_COUNT(this) == 0); }

    /// Increment the reference count.
    void ref() { A3_REF(this); }

    /// Decrement the reference count, and destroy if it reaches zero.
    void unref() { A3_UNREF_D(this, destroy); }

public:
    /// Get the current reference count.
    C ref_count() const { return A3_REF_COUNT(this); }
};

/// A pointer to a reference-counted type. The pointed-to type must inherit from RefCounted.
template <typename T, typename C = uint32_t>
class Rc {
private:
    RefCounted<T, C>* ptr;

    Rc() : ptr { nullptr } {}

public:
    /// Create an Rc pointing to the given target. Increments the reference count.
    explicit Rc(T* target) : ptr { target } {
        if (ptr)
            ptr->ref();
    }

    /// Create an Rc pointing to the same target as another one. Increments the reference count.
    Rc(const Rc& other) : ptr { other.ptr } {
        if (ptr)
            ptr->ref();
    }

    /// Move an Rc. Nulls out the other's pointer and does not change the reference count.
    Rc(Rc&& other) : ptr { other.ptr } { other.ptr = nullptr; }

    /// Copy another Rc into this one. Decrements the reference count of the existing target, if
    /// any, and increments the reference count of the new target.
    Rc& operator=(const Rc& other) {
        if (ptr)
            ptr->unref();
        ptr = other.ptr;
        if (ptr)
            ptr->ref();
        return *this;
    }

    /// Move another Rc into this one. Nulls out the other pointer, and decrements the reference
    /// count of the existing target, if any.
    Rc& operator=(Rc&& other) {
        if (ptr)
            ptr->unref();
        ptr       = other.ptr;
        other.ptr = nullptr;
    }

    ~Rc() {
        if (!ptr)
            return;

        assert(ptr->ref_count());
        ptr->unref();
    }

    /// Adopt an object without changing its reference count.
    static Rc adopt(T* target) {
        Rc ret;
        ret.ptr = target;
        return ret;
    }

    /// Instantiate a new target object and get an Rc pointing to it.
    template <typename... Args>
    static Rc<T, C> create(Args&&... args) {
        return Rc<T, C>::adopt(new T { std::forward<Args>(args)... });
    }

    /// Dereference the pointer.
    T& operator*() {
        assert(ptr);
        return *static_cast<T*>(ptr);
    }

    /// Access members on the target object.
    T* operator->() {
        assert(ptr);
        return static_cast<T*>(ptr);
    }
};

} // namespace a3
