#ifndef MINI_ALGORITHM_HEAP_H
#define MINI_ALGORITHM_HEAP_H

#include "mini_stl/iterator/mini_iterator_base.h"

namespace mini::algo {

template<typename RandomAccessIterator, typename Distance, typename T>
void __push_heap(RandomAccessIterator first, Distance hole_idx, Distance top_idx, T value)
{
    // Idea: percolate up the heap, and update hole_idx on the way
    Distance parent = (hole_idx - 1) / 2;
    while (hole_idx > top_idx && *(first + parent) < value) {
        // not yet reach root node, and parent's value is smaller: move upwards
        *(first + hole_idx) = *(first + parent);  // swap with parent value
        hole_idx = parent;
        parent = (hole_idx - 1) / 2;  // jump to next parent node
    }
    *(first + hole_idx) = value;  // final hold_idx is found, put new value here
}

/**
 * @brief Adjust elements of an underlying container to satisfy heap requirements.
 *
 * @tparam RandomAccessIterator Container Iterator Category
 * @tparam Distance Distance Type
 * @tparam T Value Type
 * @param first Iterator to first node in current sub-heap
 * @param hole_idx Index of current node that is to be adjusted
 * @param len Range of current sub-heap (i.e., last - first)
 * @param value Value of current node that is to be adjusted
 */
template<typename RandomAccessIterator, typename Distance, typename T>
inline void __adjust_heap(RandomAccessIterator first, Distance hole_idx, Distance len, T value)
{
    // hole_idx: element that will travel downwards to update the heap requirements
    Distance top_idx = hole_idx;
    // represents child with larger value, initialized to right child
    Distance candidate_child = 2 * hole_idx + 2;
    while (candidate_child < len) {
        // compare two childs
        if (*(first + (candidate_child - 1)) > *(first + candidate_child)) {
            candidate_child--;  // update candidate to left child
        }
        *(first + hole_idx) = *(first + candidate_child);
        hole_idx = candidate_child;
        candidate_child = 2 * (candidate_child + 1);
    }
    if (candidate_child == len) {
        // no right child, only left child
        *(first + hole_idx) = *(first + (candidate_child - 1));
        hole_idx = candidate_child - 1;
    }
    // value is inserted at bottom, however, the value is not checked
    // with child's value during percolating downwards, so heap structure may
    // not be correct, we need to call push_heap to keep structure correct.
    __push_heap(first, hole_idx, top_idx, value);
}

template<typename RandomAccessIterator, typename Distance, typename T>
inline void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*)
{
    mini::algo::__push_heap(first, Distance(last - first - 1), Distance(0), T(*(last - 1)));
}

template<typename RandomAccessIterator>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last)
{
    // element to be pushed into heap is already put at the tail of the underlying container
    mini::algo::__push_heap_aux(first, last, iter::distance_type(first), iter::value_type(first));
}

template<typename RandomAccessIterator, typename Distance, typename T>
inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last,
    RandomAccessIterator result, T value, Distance*)
{
    *result = *first;  // copy the max value to last position: meaning that it's popped
    mini::algo::__adjust_heap(first, Distance(0), Distance(last - first), value);
}

template<typename RandomAccessIterator, typename T>
inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*)
{
    mini::algo::__pop_heap(first, last - 1, last - 1, T(*(last - 1)), iter::distance_type(first));
}

template<typename RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last)
{
    mini::algo::__pop_heap_aux(first, last, iter::value_type(first));
}

// Sort elements in a heap in increasing order.
// Sorted elements is not a heap anymore.
template<typename RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last)
{
    // Every call to pop_heap will put the current max value to the end of current range
    // Sorted in increasing order
    while (last - first > 1) {
        mini::algo::pop_heap(first, last--);
    }
}

template<typename RandomAccessIterator, typename T, typename Distance>
void __make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*)
{
    Distance len = last - first;
    if (len <= 1) {  // empty of only one element
        return;
    }

    Distance hole_idx = (len - 2) / 2;
    do {
        // adjust each sub-heap starting from last node of second-last layer
        // way up to root, following by decrementing idx by one.
        mini::algo::__adjust_heap(first, hole_idx, len, T(*(first + hole_idx)));
    } while (hole_idx--);  // stop after finishing adjusting root node
}

// Make a heap from elements of range [first, last) from underlying container.
template<typename RandomAccessIterator>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last)
{
    mini::algo::__make_heap(first, last, iter::value_type(first), iter::distance_type(first));
}

}  // namespace mini::algo

#endif
