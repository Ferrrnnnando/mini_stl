#ifndef MINI_ITER_LIST_H
#define MINI_ITER_LIST_H

#include "mini_stl/container/mini_container_list.h"

namespace mini::iterator {

template<typename Item>
struct ListIter {
    using value_type = Item;
    using pointer_type = value_type*;
    using reference_type = value_type&;

    pointer_type ptr;

    ListIter(pointer_type p = 0)
        : ptr(p)
    {}
    // using default copy ctor
    // using default operator=

    reference_type operator*() { return *ptr; }
    pointer_type operator->() { return ptr; }

    // pre-increment operator
    ListIter& operator++()
    {
        ptr = ptr->next();
        return *this;
    }
    // post-increment operator
    ListIter operator++(int)
    {
        ListIter tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator==(const ListIter& rhs) const { return ptr == rhs.ptr; }
    bool operator!=(const ListIter& rhs) const { return ptr != rhs.ptr; }
};

}  // namespace mini::iterator

#endif
