#ifndef MINI_ITERATOR_BASE_H
#define MINI_ITERATOR_BASE_H

#include <cstddef>

namespace mini::iterator {

/**
 * @brief Five iterator categories
 * @attention Inheritance relationship represents relative "strongness" between each category.
 *            Weaker category(base class) can substitue stronger category, but efficiency is
 *            not guaranteed.
 */
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

/**
 * @brief Standard iterator class for new iterator to inherit to ensure completeness of all associated types
 *
 * @tparam Category
 * @tparam T
 * @tparam Distance
 * @tparam Pointer
 * @tparam Reference
 */
template<typename Category, typename T, typename Distance = ptrdiff_t, typename Pointer = T*,
    typename Reference = T&>
struct Iterator {
    typedef Category iterator_category;
    typedef T value_type;
    typedef Distance difference_type;
    typedef Pointer pointer;
    typedef Reference reference;
};

// Primary template for traits class
template<typename Iterator>
struct iterator_traits {
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::value_type value_type;
    typedef typename Iterator::difference_type difference_type;
    typedef typename Iterator::pointer pointer;
    typedef typename Iterator::reference reference;
};

// Specialized version of traits class for native pointer
template<typename T>
struct iterator_traits<T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef T& reference;
};

// Specialized version of traits class for native pointer-to-const
template<typename T>
struct iterator_traits<const T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef T& reference;
};

// Get iterator category given an iterator
template<typename Iterator>
inline typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator&)
{
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();
}

// Get distance type given an iterator
template<typename Iterator>
inline typename iterator_traits<Iterator>::difference_type* distance_type(const Iterator&)
{
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

// Get value_type given an iterator
template<typename Iterator>
inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator&)
{
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

////// distance function //////
template<typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type __distance(
    InputIterator first, InputIterator last, input_iterator_tag)
{
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

template<typename RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type __distance(
    RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
{
    return last - first;
}

template<typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type __distance(
    InputIterator first, InputIterator last)
{
    return __distance(first, last, iterator_category(first));
}
////// distance function end //////

////// advance function //////
template<typename InputIterator, typename Distance>
inline void __advance(InputIterator& iter, Distance n, input_iterator_tag)
{
    for (; n > 0; --n) {
        ++iter;
    }
}

template<typename BidirectionalIterator, typename Distance>
inline void __advance(BidirectionalIterator& iter, Distance n, bidirectional_iterator_tag)
{
    if (n > 0) {
        for (; n > 0; --n) {
            ++iter;
        }
    } else {
        for (; n < 0; ++n) {
            --iter;
        }
    }
}

template<typename RandomAccessIterator, typename Distance>
inline void __advance(RandomAccessIterator& iter, Distance n, random_access_iterator_tag)
{
    iter += n;
}

template<typename InputIterator, typename Distance>
inline void advance(InputIterator& iter, Distance n)
{
    __advance(iter, n, iterator_category(iter));
}
////// advance function end //////

}  // namespace mini::iterator

#endif
