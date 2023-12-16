#ifndef MINI_ITERATOR_DEQUE_H
#define MINI_ITERATOR_DEQUE_H

#include "mini_stl/iterator/mini_iterator_base.h"

namespace mini::iter {

/**
 * @brief Global function for determining deque buffer size.
 *
 * @param n n represents user custom buffer size if not equals 0
 * @param element_size Size of value type
 * @return size_t Buffer size, i.e., how many elements the buffer is storing
 */
inline size_t __deque_buf_size(size_t n, size_t element_size)
{
    return n != 0 ? n : (element_size < 512 ? 512 / element_size : 1);
}

template<typename T, typename Ref, typename Ptr, size_t BufferSize>
struct __deque_iterator {
    typedef __deque_iterator<T, T&, T*, BufferSize> iterator;
    typedef __deque_iterator<T, const T&, const T*, BufferSize> const_iterator;
    typedef __deque_iterator<T, Ref, Ptr, BufferSize> self;

    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ref reference;
    typedef Ptr pointer;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T** map_pointer;

    // static members
    static size_type buffer_size() { return __deque_buf_size(BufferSize, sizeof(T)); }

    // Connections to container
    T* cur;              // point to position just after current element in buffer zone
    T* first;            // point to begin of buffer zone
    T* last;             // point to end of buffer zone
    map_pointer node;    // point to current node (further point to a buffer) in map
    size_type map_size;  // # of node (pointer) in map

    // Set iterator status to a new node (map_pointer / buffer zone)
    void set_node(map_pointer new_node)
    {
        node = new_node;
        first = *new_node;
        last = first + difference_type(buffer_size());
    }

    // operators
    reference operator*() { return *cur; }

    pointer operator->() { return &(operator*()); }

    difference_type operator-(const self& other) const
    {
        // assume *self is "larger(after) than" other
        return difference_type(buffer_size()) * (node - other.node - 1) + (cur - first)
            + (other.last - other.cur);
    }

    self& operator++()
    {
        ++cur;
        if (cur == last) {  // check if reach tail of current buffer zone
            // jump to the first element of next buffer zone
            set_node(node + 1);  // TODO: what if (node + 1) exceeds map_size?
            cur = first;
        }
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
        if (cur == first) {  // check if reach head of current buffer zone
            // jump to the last element of previous buffer zone
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }

    self operator--(int)
    {
        self tmp = *this;
        --(*this);
        return tmp;
    }

    // allow multiple increment of iterator to achieve random access
    // move forward if n is negative
    self& operator+=(difference_type n)
    {
        difference_type offset = n + (cur - first);  // offset to first
        if (offset >= 0 && offset < difference_type(buffer_size())) {
            // target position in the same buffer
            cur += n;
        } else {
            // target position not in same buffer
            difference_type node_offset = offset > 0
                ? offset / difference_type(buffer_size())  // TODO: verify logic
                : -difference_type((-offset - 1) / buffer_size()) - 1;
            // move to correct buffer
            set_node(node + node_offset);
            // move to correct element
            cur = first + (offset - node_offset * difference_type(buffer_size()));
        }
        return *this;
    }

    // allow multiple decrement of iterator
    self operator-=(difference_type n) { return (*this) += -n; }

    self operator+(difference_type n) const
    {
        self tmp = *this;
        return (tmp += n);
    }

    self operator-(difference_type n) const
    {
        self tmp = *this;
        return (tmp -= n);
    }

    // access value of iterator that is n distance away from this iterator
    reference operator[](difference_type n) const { return *(*this + n); }

    bool operator==(const self& other) const { return cur == other.cur; }

    bool operator!=(const self& other) const { return !(*this == other); }

    bool operator<(const self& other) const
    {
        // 1. compare which buffer zone
        // 2. compare relative position of cur if in the same buffer zone
        return (node == other.node) ? (cur < other.cur) : (node < other.node);
    }
};

}  // namespace mini::iter

#endif
