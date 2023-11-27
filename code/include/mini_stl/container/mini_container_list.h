#ifndef MINI_CONTAINER_LIST_H
#define MINI_CONTAINER_LIST_H

#include "mini_stl/iterator/mini_iterator_list.h"
#include "mini_stl/memory/mini_memory.h"

namespace mini::ctnr {

/**
 * @brief A circular doubly-linked list.
 *
 * @tparam T value type
 * @tparam Alloc data allocator
 */
template<typename T, typename Alloc = mem::alloc>
class List {
protected:
    typedef iter::__list_node<T> list_node;

public:
    typedef list_node* link_type;

public:
    typedef T value_type;
    typedef iter::__list_iterator<T, T&, T*> iterator;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef value_type& reference;

    // iterators
    iterator begin() { return (link_type)(node_->next); }
    iterator end() { return node_; }

    // Element access
    reference front() { return *begin(); }
    reference back() { return *(--end()); }

    // capacity
    bool empty() const { return node_->next == node_; }
    size_type size() const { return (size_type)iter::distance(begin(), end()); }

protected:
    // node_ is pointed to a null node after the tail so as to satisfy STL's right-open interval req.
    link_type node_;
};

}  // namespace mini::ctnr

#endif
