// Global utility functions for memory management tool

#ifndef MINI_MEMORY_UNINITIALIZED_H
#define MINI_MEMORY_UNINITIALIZED_H

#include "mini_stl/base/mini_base_type_traits.h"
#include "mini_stl/iterator/mini_iterator_base.h"
#include "mini_stl/memory/mini_memory_construct.h"

#include <algorithm>
#include <cstring>

namespace mini::mem {

//////////////////////////////////////////////////////////////////////////////////////
// uninitialized_copy()
// Usesage:
//      A container's range ctor takes 2 steps:
//          1. allocate enough space
//          2. invoke uninitialized_copy() to construct objects in-place
//////////////////////////////////////////////////////////////////////////////////////
template<typename InputIterator, typename ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last,
    ForwardIterator result, mini::type_traits::__false_type)
{
    // TODO: exception handling is skipped here:
    // Need to support 'commit or rollback' to meet stl standard
    // if any copy ctor throw exception, any already constructed
    // objs should be destroyed properly
    ForwardIterator cur = result;
    for (; first != last; ++first, ++cur) {
        construct(&(*cur), *first);
    }
    return cur;
}

template<typename InputIterator, typename ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux(
    InputIterator first, InputIterator last, ForwardIterator result, mini::type_traits::__true_type)
{
    return std::copy(first, last, result);
}

template<typename InputIterator, typename ForwardIterator, typename T>
inline ForwardIterator __uninitialized_copy(
    InputIterator first, InputIterator last, ForwardIterator result, T*)
{
    typedef typename mini::type_traits::__type_traits<T>::is_POD_type is_POD;
    return __uninitialized_copy_aux(first, last, result, is_POD());
}

template<typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy(
    InputIterator first, InputIterator last, ForwardIterator result)
{
    return __uninitialized_copy(first, last, result, mini::iter::value_type(first));
}

// specialized templates for char*
inline char* uninitialized_copy(const char* first, const char* last, char* result)
{
    std::memmove(result, first, last - first);
    return result + (last - first);
}

// specialized templates for wchar_t*
inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result)
{
    std::memmove(result, first, sizeof(wchar_t) * (last - first));
    return result + (last - first);
}

//////////////////////////////////////////////////////////////////////////////////////
// uninitialized_fill()
//////////////////////////////////////////////////////////////////////////////////////

template<typename ForwardIterator, typename T>
inline void __uninitialized_fill_aux(
    ForwardIterator first, ForwardIterator last, const T& value, mini::type_traits::__true_type)
{
    std::fill(first, last, value);
}

template<typename ForwardIterator, typename T>
inline void __uninitialized_fill_aux(
    ForwardIterator first, ForwardIterator last, const T& value, mini::type_traits::__false_type)
{
    // TODO: exception handling is skipped here:
    // Need to support 'commit or rollback' to meet stl standard
    // if any copy ctor throw exception, any already constructed
    // objs should be destroyed properly
    ForwardIterator cur = first;
    for (; cur != last; ++cur) {
        construct(&(*cur), value);
    }
}

template<typename ForwardIterator, typename T, typename T1>
inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value, T1*)
{
    typedef typename mini::type_traits::__type_traits<T1>::is_POD_type is_POD;
    __uninitialized_fill_aux(first, last, value, is_POD());
}

template<typename ForwardIterator, typename T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value)
{
    __uninitialized_fill(first, last, value, mini::iter::value_type(first));
}

//////////////////////////////////////////////////////////////////////////////////////
// uninitialized_fill_n()
//////////////////////////////////////////////////////////////////////////////////////

// POD type
template<class ForwardIterator, class Size, class T>
inline ForwardIterator __uninitialized_fill_n_aux(
    ForwardIterator first, Size n, const T& x, mini::type_traits::__true_type)
{
    return std::fill_n(first, n, x);
    // return fill_n(first, n, x);  // TODO: to be implemented in chapter 6
}

// non-POD type
template<class ForwardIterator, class Size, class T>
inline ForwardIterator __uninitialized_fill_n_aux(
    ForwardIterator first, Size n, const T& x, mini::type_traits::__false_type)
{
    // TODO: exception handling is skipped here:
    // Need to support 'commit or rollback' to meet stl standard
    // if any copy ctor throw exception, any already constructed
    // objs should be destroyed properly
    ForwardIterator cur = first;
    for (; n > 0; --n, ++cur) {
        construct(&(*cur), x);
    }
    return cur;
}

template<typename ForwardIterator, typename Size, typename T, typename T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*)
{
    typedef typename mini::type_traits::__type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

/**
 * @brief uninitialized_fill_n() Fill an uninitialized space with n values
 * @param first place where we want to initialize
 * @param n     size of space we want to initialize
 * @param x     initial value
 */
template<typename ForwardIterator, typename Size, typename T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
{
    return __uninitialized_fill_n(first, n, x, mini::iter::value_type(first));
}

}  // namespace mini::mem

#endif
