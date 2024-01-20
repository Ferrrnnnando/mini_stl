#ifndef MINI_ALGORITHM_BASE_H
#define MINI_ALGORITHM_BASE_H

#include "mini_stl/base/mini_base_type_traits.h"
#include "mini_stl/iterator/mini_iterator_base.h"

namespace mini::algo {

// notes: 'associated type' is used here: T
// Cons: this simple find algo needs to know object type T
template<typename InputIterator, typename T>
InputIterator find(InputIterator first, InputIterator last, const T& value)
{
    while (first != last && (*first) != value) {
        ++first;
    }
    return first;
}

template<typename InputIterator, typename Function>
InputIterator for_each(InputIterator first, InputIterator last, Function& func)
{
    for (; first != last; ++first) {
        func(*first);
    }
    return first;
}

//// copy() begin ////

// Version 1: input iterator
template<typename InputIterator, typename OutputIterator>
inline OutputIterator __copy(
    InputIterator first, InputIterator last, OutputIterator result, iter::input_iterator_tag)
{
    for (; first != last; ++result, ++first) {
        *result = *first;
    }
    return result;
};

template<typename RandomAccessIterator, typename OutputIterator, typename Distance>
inline OutputIterator __copy_d(
    RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*)
{
    // decrement n is fast, compared to checking inequality between two iterators to determine break conditions
    for (Distance n = last - first; n > 0; --n, ++result, ++first) {
        *result = *first;
    }
    return result;
}

// Version 2: random access iterator
template<typename RandomAccessIterator, typename OutputIterator>
inline OutputIterator __copy(RandomAccessIterator first, RandomAccessIterator last,
    OutputIterator result, iter::random_access_iterator_tag)
{
    __copy_d(first, last, result, iter::distance_type(first));
}

template<typename T>
inline T* __copy_t(const T* first, const T* last, T* result, type_traits::__true_type)
{
    memmove(result, first, sizeof(T) * (last - first));
    return result + (last - first);
}

template<typename T>
inline T* __copy_t(const T* first, const T* last, T* result, type_traits::__false_type)
{
    return __copy_d(first, last, result, (ptrdiff_t*)0);
}

template<typename InputIterator, typename OutputIterator>
struct __copy_dispatch {
    OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result)
    {
        return __copy(first, last, result, iter::iterator_category(first));
    }
};

// partially specialized version 1: for non-const pointers
template<typename T>
struct __copy_dispatch<T*, T*> {
    T* operator()(T* first, T* last, T* result)
    {
        using t = typename type_traits::__type_traits<T>::has_trivial_assignment_operator;
        return __copy_t(first, last, result, t());
    }
};

// partially specialized version 2: for const poniters
template<typename T>
struct __copy_dispatch<const T*, T*> {
    T* operator()(const T* first, const T* last, T* result)
    {
        using t = typename type_traits::__type_traits<T>::has_trivial_assignment_operator;
        return __copy_t(first, last, result, t());
    }
};

// General copy entry
template<typename InputIterator, typename OutputIterator>
OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
{
    return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
}

// Special version for char* and wchar_t* pointers
inline char* copy(const char* first, const char* last, char* result)
{
    std::memmove(result, first, last - first);
    return result + (last - first);
}

inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result)
{
    std::memmove(
        result, first, sizeof(wchar_t) * (last - first));  // last arg: number of bytes to copy
    return result + (last - first);  // shift backward, no need deal with sizeof(wchar_t)
}

//// copy() begin ////

template<typename BidirIt1, typename BidirIt2>
BidirIt2 copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 d_last)
{
    while (first != last) {
        *(--d_last) = *(--last);
    }
    return d_last;
}

}  // namespace mini::algo

#endif
