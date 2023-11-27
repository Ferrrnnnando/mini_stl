#ifndef MINI_ITERATOR_LIST_H
#define MINI_ITERATOR_LIST_H

#include "mini_stl/iterator/mini_iterator_base.h"

namespace mini::iter {

/**
 * @brief List node
 *
 * @attention Elements of doubly-linked list, bidirectional iterator is needed
 * @tparam T Data type
 */
template<typename T>
struct __list_node {
    typedef void* void_pointer;

    void_pointer prev;  // can use __list_node<T>*
    void_pointer next;
    T data;
};

template<typename T, typename Ref, typename Ptr>
struct __list_iterator : Iterator<bidirectional_iterator_tag, T, ptrdiff_t, Ptr, Ref> {
    typedef __list_iterator<T, T&, T*> iterator;
    typedef __list_iterator<T, Ref, Ptr> self;

    // other necessary typedefs defined in base class
    typedef bidirectional_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef ptrdiff_t difference_type;
    typedef __list_node<T>* link_type;
    typedef size_t size_type;

    link_type node_;

    // constructors
    __list_iterator() {}
    __list_iterator(link_type x)
        : node_(x)
    {}
    __list_iterator(const iterator& x)
        : node_(x.node_)
    {}

    // operators
    bool operator==(const self& rhs) const { return node_ == rhs.node_; }
    bool operator!=(const self& rhs) const { return node_ != rhs.node_; }
    reference operator*() const { return node_->data; }
    pointer operator->() const { return &(operator*()); }

    self& operator++()
    {
        node_ = (link_type)(node_->next);
        return *this;
    }
    self operator++(int)
    {
        self tmp = *this;
        ++(*this);
        return tmp;
    }
    self& operator--()
    {
        node_ = (link_type)(node_->prev);
        return *this;
    }
    self operator--(int)
    {
        self tmp = *this;
        --(*this);
        return tmp;
    }
};

}  // namespace mini::iter

#endif
