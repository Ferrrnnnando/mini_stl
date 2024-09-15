#ifndef MINI_MEMORY_CONSTRUCT_H
#define MINI_MEMORY_CONSTRUCT_H

#include "mini_stl/base/mini_base_type_traits.h"
#include "mini_stl/iterator/mini_iterator_base.h"

#include <new>  // include placement new

namespace mini::mem {

template<typename T, typename... Args>
inline void construct(T* ptr, Args&&... args)
{
    new (ptr) T(std::forward<Args>(args)...);
}

// Version 1: accept raw pointer
template<typename T>
inline void destroy(T* ptr)
{
    ptr->~T();  // T::~T() dtor is invoked
}

// Value type has trivial destructor: do nothing
template<typename ForwardIterator>
inline void __destroy_aux(
    ForwardIterator first, ForwardIterator last, mini::type_traits::__true_type)
{}

// Value type has non-trivial destructor: invoke dtor of each element one by one
template<typename ForwardIterator>
inline void __destroy_aux(
    ForwardIterator first, ForwardIterator last, mini::type_traits::__false_type)
{
    for (; first < last; ++first) {
        destroy(&(*first));
    }
}

template<typename ForwardIterator, typename T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*)
{
    typedef typename mini::type_traits::__type_traits<T>::has_trivial_destructor trivial_destructor;
    __destroy_aux(first, last, trivial_destructor());
}

// Version 2: accept 2 iterators
template<typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last)
{
    __destroy(first, last, mini::iter::value_type(first));
}

// Specilized template for char* and wchar*
inline void destroy(char*, char*) {}
inline void destroy(wchar_t*, wchar_t*) {}

}  // namespace mini::mem

#endif
