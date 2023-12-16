#ifndef MINI_CONTAINER_DEQUE_H
#define MINI_CONTAINER_DEQUE_H

#include "mini_stl/algorithm/mini_algorithm.h"
#include "mini_stl/memory/mini_memory.h"

#include "mini_stl/iterator/mini_iterator_deque.h"

#include <sstream>

namespace mini::ctnr {

template<typename T, typename Alloc = mem::alloc, size_t BufferSize = 0>
class deque {
public:  // public typedefs
    typedef T value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef iter::__deque_iterator<T, T&, T*, BufferSize> iterator;

protected:  // internal typedefs
    typedef pointer* map_pointer;
    typedef mem::simple_alloc<value_type, Alloc> data_allocator;
    typedef mem::simple_alloc<pointer, Alloc> map_allocator;

public:
    deque(int n, const_reference value)
        : begin_()
        , end_()
        , map_(0)
        , map_size_(0)
    {
        fill_initialize(n, value);
    }

public:
    // Iterators
    iterator begin() const { return begin_; }
    iterator end() const { return end_; }

    // Element access
    reference operator[](size_type n) { return begin_.operator[](difference_type(n)); }

    reference front() { return *begin_; }

    reference back()
    {
        // end_.cur: position after last element
        return *(end_ - 1);  // TODO: check correctness
    }

    // Capacity
    size_type size() const { return end_ - begin_; }

    size_type max_size() const { return size_type(-1); }

    bool empty() const { return begin_ == end_; }

    // Modifiers
    void push_back(const_reference value)
    {
        if (end_.cur != (end_.last - 1)) {
            mem::construct(end_.cur, value);
            ++end_.cur;
        } else {
            // remaining space have only one space left or none
            push_back_aux(value);
        }
    }

    void push_front(const_reference value)
    {
        if (begin_.cur != begin_.first) {  // still have one or more space left
            mem::construct(begin_.cur - 1, value);
            --begin_.cur;
        } else {
            // there is no space left
            push_front_aux(value);
        }
    }

    void pop_back()
    {
        if (end_.cur != end_.first) {  // have one or more elements
            --end_.cur;
            mem::destroy(end_.cur);
        } else {
            // no elements in the last buffer zone
            pop_back_aux();
        }
    }

    void pop_front()
    {
        if (begin_.cur != begin_.last - 1) {  // have two or more elements
            mem::destroy(begin_.cur);
            ++begin_.cur;
        } else {
            // have one element left
            pop_front_aux();
        }
    }

    /**
     * @brief Erase all elements. Buffers except one are all freed.
     *
     * @attention One buffer zone(first buffer zone) is kept.
     */
    void clear()
    {
        // Dealing with all buffer zones except head and tail buffer zone
        // notes: buffer zones except head and tail buffer zone must be full
        for (map_pointer node = begin_.node + 1; node < end_.node; ++node) {
            mem::destroy(*node, *node + buffer_size());
            data_allocator::deallocate(*node, buffer_size());
        }

        if (begin_.node != end_.node) {  // if have 2 buffer zones (head and tail)
            mem::destroy(begin_.cur, begin_.last);
            mem::destroy(end_.first, end_.cur);
            // only deallocate tail buffer zone
            data_allocator::deallocate(end_.first, buffer_size());
        } else {                                 // if only have one buffer zone
            mem::destroy(begin_.cur, end_.cur);  // destroy all elements within this buffer
        }
        end_ = begin_;
    }

    /**
     * @brief Erase the element at a position.
     *
     * @param pos Position of element to be removed
     * @return iterator Iterator to erased position
     */
    iterator erase(iterator pos)
    {
        iterator next = pos;
        ++next;

        difference_type idx = pos - begin_;
        if (idx < (size() >> 1)) {  // equivalent to size() / 2
            // fewer elements before the removal point
            algo::copy_backward(begin_, pos, next);  // copy elements backward
            pop_front();                             // remove the first element
        } else {
            algo::copy(next, end_, pos);
            pop_back();
        }
        return begin_ + idx;
    }

    /**
     * @brief Erase elements within a range.
     *
     * @param first Start range of elements to remove
     * @param last Finish range of elements to remove
     * @return iterator Iterator to erased starting position
     */
    iterator erase(iterator first, iterator last)
    {
        if (first == begin_ && last == end_) {
            // if interval to be erased is the whole deque, call clear() directly
            clear();
            return end_;
        }

        difference_type n = last - first;
        difference_type elements_before = first - begin_;  // num elements before erase interval
        if (elements_before < (size() - n) / 2) {
            // elements before erase point is fewer, move elements at front backward
            algo::copy_backward(begin_, first, last);
            iterator new_begin = begin_ + n;
            mem::destroy(begin_, new_begin);
            // free redundant buffer space at the front after the movement
            for (map_pointer cur_node = begin_.node; cur_node < new_begin.node; ++cur_node) {
                data_allocator::deallocate(*cur_node, buffer_size());
            }
            begin_ = new_begin;  // new begin point of deque
        } else {
            // elements after erase point is fewer, move elements at the back to front
            algo::copy(last, end_, first);
            iterator new_end = end_ - n;
            mem::destroy(new_end, end_);
            for (map_pointer cur_node = new_end.node + 1; cur_node <= end_.node; ++cur_node) {
                data_allocator::deallocate(*cur_node, buffer_size());
            }
            end_ = new_end;
        }
        return begin_ + elements_before;
    }

    /**
     * @brief Insert an element at a position.
     *
     * @param pos Position to insert
     * @param value Value of element to insert
     * @return iterator Inserted position
     */
    iterator insert(iterator pos, const_reference value)
    {
        if (pos == begin_) {
            // insert point is the front, call push_front()
            push_front(value);
            return begin_;
        } else if (pos == end_) {
            // if insert point is the back, call push_back()
            push_back(value);
            return end_ - 1;
        } else {
            // insert point at middle
            return insert_aux(pos, value);
        }
    }

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

protected:  // internal methods
    size_type buffer_size() { return iterator::buffer_size(); }

    void fill_initialize(size_type n, const_reference value)
    {
        create_map_and_nodes(n);  // create internal deque structures
        map_pointer cur;
        try {
            // end_ node has special treatment, no need construct elements at all positions
            for (cur = begin_.node; cur < end_.node; ++cur) {
                // *cur: head of current buffer zone
                mem::uninitialized_fill(*cur, *cur + buffer_size(), value);
            }
            // end_ node may have extra reserved space, no need specialize initial value
            mem::uninitialized_fill(end_.first, end_.cur, value);
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            std::cerr << "void deque::fill_initialize() failed" << '\n';
        }
    }

    void create_map_and_nodes(size_type num_elements)
    {
        // at least one buffer zone is allocated
        size_type num_nodes = num_elements / buffer_size() + 1;  // number of buffer zone needed

        // how many nodes a map needs to manage: at least 8, at most required num + 2
        map_size_ = std::max(initial_map_size(), num_nodes + 2);
        map_ = map_allocator::allocate(map_size_);

        // points to middle area, the amount of noedes that user specify.
        // extra nodes at head and tail are for reservation for future use.
        // at least one extra buffer before the head and after tail of map
        map_pointer nbegin = map_ + (map_size_ - num_nodes) / 2;
        map_pointer nend = nbegin + num_nodes - 1;

        map_pointer cur;
        try {
            for (cur = nbegin; cur <= nend; ++cur) {
                *cur = allocate_node();
            }
        } catch (const std::exception& e) {
            --cur;  // current map node has not been allocated yet, no need deallocate
            for (; cur >= nbegin; --cur) {
                deallocate_node(*cur);
            }
            std::cerr << e.what() << '\n';
            std::cerr << "deque::create_map_and_nodes failed" << '\n';
        }

        // set correct values for member variables: iterators
        begin_.set_node(nbegin);
        end_.set_node(nend);
        begin_.cur = begin_.first;
        end_.cur = end_.first + num_elements % buffer_size();
    }

    void push_back_aux(const_reference value)
    {
        // 1. allocate a new buffer
        // 2. construct new element
        // 3. update iterators' status

        // may need to replace a new map under some conditions
        reserve_map_at_back();
        // allocate a node (buffer) at the back of current buffer
        *(end_.node + 1) = allocate_node();
        try {
            mem::construct(end_.cur, value);  // this step likely to fail
            end_.set_node(end_.node + 1);
            end_.cur = end_.first;
        } catch (const std::exception& e) {
            // rollback: deallocate just allocated node due to element construct failure
            deallocate_node(*(end_.node + 1));
            std::cerr << e.what() << '\n';
            std::cerr << "deque::push_back_aux() failed" << '\n';
        }
    }

    void push_front_aux(const_reference value)
    {
        reserve_map_at_front();
        *(begin_.node - 1) = allocate_node();

        try {
            begin_.set_node(begin_.node - 1);
            begin_.cur = begin_.last - 1;
            mem::construct(begin_.cur, value);  // construct element at tail of front buffer
        } catch (const std::exception& e) {
            // rollback: reset begin_ iterator, deallocate the node
            begin_.set_node(begin_.node + 1);
            begin_.cur = begin_.first;
            deallocate_node(*(begin_.node - 1));
            std::cerr << e.what() << '\n';
            std::cerr << "deque::push_front_aux() failed" << '\n';
        }
    }

    void pop_back_aux()
    {
        // no elements in the last buffer zone
        deallocate_node(end_.first);   // release the last buffer zone
        end_.set_node(end_.node - 1);  // adjust end_ node status
        end_.cur = end_.last - 1;      // move to the last element in previous buffer zone
        mem::destroy(end_.cur);        // destroy the last element
    }

    void pop_front_aux()
    {
        mem::destroy(begin_.cur);
        deallocate_node(begin_.first);
        begin_.set_node(begin_.node + 1);
        begin_.cur = begin_.first;
    }

    iterator insert_aux(iterator pos, const_reference value)
    {
        difference_type idx = pos - begin_;  // number of elements before insert point
        if (idx < size() / 2) {
            // number of elements before insert point is fewer: shift elements frontwards
            push_front(front());  // allocate map node/buffer under the hood if necessary

            // TODO: Verity correctness
            // shift original interval: [2nd element, insertion point) one slot forward
            algo::copy(begin_ + 2, pos, begin_ + 1);
        } else {
            // number of elements after insert point is fewer: shift elements backward
            push_back(back());  // allocate map node/buffer under the hood if necessary

            // TODO: Verity correctness
            // shift original interval: [insertion point, 2nd last element + 1) one slot backward
            algo::copy_backward(pos, end_ - 2, end_ - 1);
        }
        *pos = value;
        return pos;
    }

    void reserve_map_at_back(size_type nodes_to_add = 1)
    {
        if (nodes_to_add > (map_size_ - (end_.node - map_) - 1)) {
            reallocate_map(nodes_to_add, false);
        }
    }

    void reserve_map_at_front(size_type nodes_to_add = 1)
    {
        // reallocate if previous extra allocated front buffer is also used up
        if (nodes_to_add > begin_.node - map_) {
            reallocate_map(nodes_to_add, true);
        }
    }

    void reallocate_map(size_type nodes_to_add, bool add_at_front)
    {
        size_type old_num_nodes = end_.node - begin_.node + 1;
        size_type new_num_nodes = old_num_nodes + nodes_to_add;

        map_pointer new_nbegin;
        if (map_size_ > 2 * new_num_nodes) {
            // map_size still enough, just either side of map is full, shift to middle again
            new_nbegin = map_ + (map_size_ - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
            if (new_nbegin < begin_.node) {
                std::copy(begin_.node, end_.node + 1, new_nbegin);
            } else {
                std::copy_backward(begin_.node, end_.node + 1, new_nbegin + old_num_nodes);
            }
        } else {
            size_type new_map_size = map_size_ + std::max(map_size_, nodes_to_add) + 2;
            map_pointer new_map = map_allocator::allocate(new_map_size);
            new_nbegin =
                new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
            // copy original map content to this new map
            std::copy(begin_.node, end_.node + 1, new_nbegin);
            // free original map
            map_allocator::deallocate(map_, map_size_);
            // update map to new map
            map_ = new_map;
            map_size_ = new_map_size;
        }
        // update iterators
        begin_.set_node(new_nbegin);
        end_.set_node(new_nbegin + old_num_nodes - 1);
    }

    pointer allocate_node() { return data_allocator::allocate(buffer_size()); }

    void deallocate_node(pointer p) { data_allocator::deallocate(p, buffer_size()); }

    size_type initial_map_size() { return 8; }

protected:
    iterator begin_;      // first node of map
    iterator end_;        // last node of map
    map_pointer map_;     // map is a contiguous space, each element is a pointer to a buffer zone
    size_type map_size_;  // number of pointers map can hold
};

}  // namespace mini::ctnr

#endif
