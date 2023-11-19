#ifndef MINI_CONTAINER_VECTOR_H
#define MINI_CONTAINER_VECTOR_H

#include "mini_stl/memory/mini_memory_alloc.h"

namespace mini::container {

template<typename T, typename Alloc = mini::memory::alloc>
class vector {
    // TODO: Finish remaining necessary members of a custom vector
public:
    typedef T value_type;

protected:
    typedef mini::memory::simple_alloc<value_type, Alloc> data_allocator;
};

}  // namespace mini::container

#endif
