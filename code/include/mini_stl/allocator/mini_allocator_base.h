#ifndef MINI_ALLOCATOR_BASE_H
#define MINI_ALLOCATOR_BASE_H

#include <climits>  // for UNIT_MAX
#include <cstddef>  // for ptrdiff_t, size_t
#include <cstdlib>  // for exit()
#include <iostream>
#include <new>  // for placement new

namespace mini::memory {

template<typename T>
inline T* _allocate(ptrdiff_t size, T*)
{
    // Use: force C++ throws std::bad_alloc exception when memory is not enough during allocation
    // std::set_new_handler
    //      user can set custom handler when memory allocation fail (not enough)
    //      0 means nullptr, force C++ thinks that there is no user provided handler
    std::set_new_handler(0);
    // global operator new is called to allocate do the heap memory allocation, no object is contructed yet
    T* p = (T*)(::operator new((size_t)(size * sizeof(T))));
    // T* p = (T*)(::operator new(size * sizeof(T)));     // TODO: size*sizeof() already gives unsigned long
    if (p == 0) {
        std::cerr << "out of memory" << std::endl;
        exit(1);  // failure status
    }
    return p;
}

template<typename T>
inline void _deallocate(T* buffer)
{
    // placement operator delete (a special overload of global operator new)
    ::operator delete(buffer);
}

template<typename T>
inline void _construct(T* p, const T& value)
{
    // Use placement new operator hidden in the global namespace
    // directly construct object in place, skip the step of memory allocation
    // "operator new" vs. "keyword new"
    // "keyword new": 2 steps: memory allcation, construct objects
    // notes: keyword can't be overloaded, while operator can

    // TODO: understand syntax?
    new (p) T(value);  // placement new: invoke ctor of T directly
    // ::operator new(p) T(value);  // invalid
}

template<typename T>
inline void _destroy(T* p)
{
    // invoke T's destructor
    p->~T();
}

template<typename T>
class allocator {
public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    // rebind allocator of type U
    template<typename U>  // TODO: wt use?
    struct rebind {
        typedef allocator<U> other;
    };

    pointer allocate(size_type n, const void* hint = 0)
    {
        return _allocate((difference_type)n, (pointer)0);
    }

    void deallocate(pointer p, size_type n) { _deallocate(p); }

    void construct(pointer p, const_reference value) { _construct(p, value); }

    void destroy(pointer p) { _destroy(p); }

    pointer address(reference x) { return (pointer)&x; }

    const_pointer const_address(const_reference x) { return (const_pointer)&x; }

    size_type max_size() const { return size_type(UINT_MAX / sizeof(T)); }
};

}  // namespace mini::memory

#endif
