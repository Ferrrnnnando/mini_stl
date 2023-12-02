#ifndef MINI_CONTAINER_LIST_H
#define MINI_CONTAINER_LIST_H

#include "mini_stl/iterator/mini_iterator_list.h"
#include "mini_stl/memory/mini_memory.h"

#include <sstream>

namespace mini::ctnr {

/**
 * @brief A circular doubly-linked list.
 *
 * @attention Can be represented using a single node pointer.
 * @attention Constant time insertion and removal, given iterator position
 * @attention Adding, removing, and moving will not invalidate iterators.
 *            An iterator is invalidated only when corresponding element is deleted.
 * @tparam T value type
 * @tparam Alloc data allocator
 */
template<typename T, typename Alloc = mem::alloc>
class list {
protected:
    typedef iter::__list_node<T> list_node;
    typedef mem::simple_alloc<list_node, Alloc> list_node_allocator;

public:
    typedef T value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef iter::__list_iterator<T, T&, T*> iterator;
    typedef const iterator const_iterator;
    // TODO: reverse iterator
    typedef list<T, Alloc> self;
    typedef list_node* link_type;

public:
    list() { empty_initialize(); }

    ~list() { clear(); }

public:
    // Iterators
    iterator begin() const { return iterator(node_->next); }

    iterator end() const { return iterator(node_); }

    // Element access
    reference front() { return *begin(); }

    reference back() { return *(--end()); }

    // Capacity
    bool empty() const { return node_->next == node_; }

    size_type size() const { return (size_type)iter::distance(begin(), end()); }

    // Modifiers
    void push_back(const_reference value) { insert_at(end(), value); }

    void push_front(const_reference value) { insert_at(begin(), value); }

    void pop_back()
    {
        iterator it = end();
        erase_at(--it);  // cannot directly decrement end()
    }

    void pop_front() { erase_at(begin()); }

    iterator insert(iterator pos, const_reference value) { insert_at(pos, value); }

    iterator erase(iterator pos) { return erase_at(pos); }

    /**
     * @brief Erase all nodes in the list.
     *
     * @attention All nodes' memory are destroyed and deallocated.
     */
    void clear()
    {
        link_type cur = node_->next;
        while (cur != node_) {
            link_type tmp = cur;
            cur = cur->next;
            destroy_node(tmp);
        }
        reset();
    }

    /**
     * @brief Remove all nodes having this value
     *
     * @param value Value of node to match
     */
    void remove(const_reference value)
    {
        iterator cur = begin();
        iterator last = end();
        while (cur != last) {
            iterator next = cur;
            ++next;
            if (*cur == value) {
                erase(cur);
            }
            cur = next;
        }
    }

    /**
     * @brief Transfers all elements from one list to another.
     *
     * @attention Transfers all elements from 'other' into *this.
     * @attention No action taken if 'other' and *this are same list.
     * @attention Container 'other' becomes empty after the operation.
     *
     * @param pos Destination position in current list
     * @param other Source list
     */
    void splice(iterator pos, self& other)
    {
        if (other.empty()) {
            return;
        }
        if (this == &other) {
            return;
        }
        transfer(pos, other.begin(), other.end());
    }

    /**
     * @brief Transfer an element to a designated position.
     *
     * @attention 'pos' & 'i' can refer to same list or different lists.
     * @param pos Destination position
     * @param i Source position
     */
    void splice(iterator pos, self&, iterator i)
    {
        iterator j = i;
        ++j;
        if (j == pos || i == pos) {
            return;
        }
        transfer(pos, i, j);
    }

    /**
     * @brief Transfer a range, i.e. [first, last), of nodes to a designated position.
     *
     * @attention 'pos' should not in range [first, last).
     * @param pos Destination position
     * @param first Source range start position (inclusively)
     * @param last Source range end position (exclusively)
     */
    void splice(iterator pos, self&, iterator first, iterator last)
    {
        // TODO: check 'pos' cannot be within '[first, last)'
        if (first == last) {
            return;
        }
        transfer(pos, first, last);
    }

    /**
     * @brief Merge a source list into current list.
     *
     * @attention Lists must be sorted before invoke this function.
     * @attention 'other' container becomes empty after merged into *this.
     * @param other Source list
     */
    void merge(self& other)
    {
        iterator cur1 = begin();
        iterator last1 = end();
        iterator cur2 = other.begin();
        iterator last2 = other.end();

        while (cur1 != last1 && cur2 != last2) {
            if (*cur2 < *cur1) {
                iterator next = cur2;
                transfer(cur1, cur2, ++next);
                cur2 = next;  // next: stores next element of list2
            } else {
                ++cur1;
            }
        }

        if (cur2 != last2) {
            // append remaining portion of list2 to end of list1
            transfer(last1, cur2, last2);
        }
    }

    /**
     * @brief Reverse list nodes.
     *
     * @attention No action when list is empty or there is only one node
     */
    void reverse()
    {
        if (size() <= 1) {
            return;
        }

        iterator cur = begin();
        ++cur;
        while (cur != end()) {
            iterator old = cur;
            ++cur;
            transfer(begin(), old, cur);
        }
    }

    /**
     * @brief Exchange contents of the container with those of 'other'.
     *
     * @attention No move, copy or swap operations on inidividual elements.
     *            All iterators and references are still valid.
     * @param other
     */
    void swap(self& other)
    {
        iterator old_begin = begin();
        // transfer all nodes from 'other' to the head of current list
        splice(begin(), other);

        // transfer all original nodes of current list to 'other'
        splice(other.begin(), *this, old_begin, end());
    }

    /**
     * @brief Sort the list in ascending order of node's value field.
     *
     * @attention List uses bidirectional iterator, so it cannot use std::sort,
     *            which requires random access iterator.
     * @attention Iterative merge sort is used. Time: O(NlogN), Extra space: O(1).
     * @attention Quicksort is used.
     */
    void sort()
    {
        if (size() <= 1) {
            return;
        }

        self carry;
        self counter[64];
        int fill = 0;

        while (!empty()) {  // repeat until all nodes in current list are used up in sorting
            carry.splice(carry.begin(), *this, begin());
            int i = 0;
            while (i < fill && !counter[i].empty()) {
                counter[i].merge(carry);
                carry.swap(counter[i++]);
            }
            carry.swap(counter[i]);  // stores all nodes in current counter, empty counter
            if (i == fill) {         // update largest index of filled counter
                ++fill;
            }
        }

        for (int i = 1; i < fill; i++) {
            counter[i].merge(counter[i - 1]);
        }
        // counter[fill - 1] contains completed final sorted array
        swap(counter[fill - 1]);
    }

    /**
     * @brief Get the list node values in the form of std::string.
     *
     * @return std::string String value of list nodes
     */
    std::string dump()
    {
        std::stringstream ss;
        iterator cur = begin();
        while (cur != end()) {
            ss << (*cur) << " ";
            ++cur;
        }

        std::string s = ss.str();
        if (s.empty()) {
            return "";
        }
        s.pop_back();
        return s;
    }

protected:
    //// Initialization

    // reset list status to only having one empty node
    void reset()
    {
        node_->next = node_;
        node_->prev = node_;
    }

    void empty_initialize()
    {
        node_ = get_node();
        reset();
    }

    //// Allocation and deallocation

    // allocate a node
    link_type get_node() { return list_node_allocator::allocate(); }

    // allocate and create a node with initial value
    link_type create_node(const_reference value)
    {
        link_type p = get_node();           // allocate whole node
        mem::construct(&(p->data), value);  // construct T at data field of node
        return p;
    }

    // dellocate a node
    void put_node(link_type p) { list_node_allocator::deallocate(p); }

    // destroy and deallocate a node
    void destroy_node(link_type p)
    {
        mem::destroy(&(p->data));  // destroy data field
        put_node(p);               // deallocate whole node
    }

    //// Modifiers helpers

    // insert a node with initial value at a designated position
    iterator insert_at(iterator pos, const_reference value)
    {
        link_type p = create_node(value);

        // connect the node just before pos with p
        (pos.node_->prev)->next = p;
        p->prev = pos.node_->prev;

        // connect the node at pos with p
        p->next = pos.node_;
        pos.node_->prev = p;

        return p;
    }

    // erase a node at a designated position
    iterator erase_at(iterator pos)
    {
        link_type prev_node = pos.node_->prev;
        link_type next_node = pos.node_->next;

        // connect node before pos & node after pos
        prev_node->next = next_node;
        next_node->prev = prev_node;
        destroy_node(pos.node_);
        return iterator(next_node);
    }

    // Move elements in [first, last) interval to another position.
    // Note: 'pos' and '[first, last)' can refer to different lists.
    // Note: 'pos' should not in range [first, last).
    void transfer(iterator pos, iterator first, iterator last)
    {
        if (pos == last) {
            return;
        }
        (last.node_->prev)->next = pos.node_;
        (first.node_->prev)->next = last.node_;
        (pos.node_->prev)->next = first.node_;
        link_type tmp = pos.node_->prev;
        pos.node_->prev = last.node_->prev;
        last.node_->prev = first.node_->prev;
        first.node_->prev = tmp;
    }

protected:
    // A list node pointer, which points to a empty node after the tail
    // so as to satisfy STL's right-open interval req.
    link_type node_;
};

}  // namespace mini::ctnr

#endif
