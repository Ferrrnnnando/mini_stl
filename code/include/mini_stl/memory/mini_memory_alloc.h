#ifndef MINI_MEMORY_ALLOC_H
#define MINI_MEMORY_ALLOC_H

#if 0
#include <new>
#define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#include <iostream>
#define __THROW_BAD_ALLOC                      \
    std::cerr << "out of memory" << std::endl; \
    exit(1)
#endif

#include <cstdlib>

namespace mini::mem {

/**
 * @brief 1st level allocator
 * @attention Use C's malloc(), free() and realloc() to do memory allocation
 * @attention A mechanism like C++ new-handler (i.e. set_new_handler()) is implemented.
 *            Can't directly use C++ new-handler mechanism, which requires ::operator new
 * @attention It's user's responsibility to set a oom(out-of-memory) handler
 * @attention Generally slower than default alloc discussed later
 * @tparam inst
 */
template<int inst>
class __malloc_alloc_template {
private:
    // user-specific allocation handler when out-of-memory
    static void* oom_malloc(size_t);
    static void* oom_realloc(void*, size_t);

    // function pointer that returns void and takes no paramters
    static void (*__malloc_alloc_oom_handler)();  // user-specific oom handler

public:
    static void* allocate(size_t n)
    {
        void* result = malloc(n);
        if (result == 0) {
            result = oom_malloc(n);
        }
        return result;
    }

    static void deallocate(void* ptr, size_t /* n */) { free(ptr); }

    static void* reallocate(void* ptr, size_t /* old_size */, size_t new_size)
    {
        void* result = realloc(ptr, new_size);
        if (result == 0) {
            result = oom_realloc(ptr, new_size);
        }
        return result;
    }

    // Simulate C++'s set_new_handler(). Using this API, we can specify custom out-of-memory handler
    static void (*set_malloc_handler(void (*func)()))()
    {
        void (*old_func)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = func;
        return (old_func);
    }
};

// handler defaults to nullptr, for client side to set
template<int inst>
void (*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

// call user specific oom handler, then use malloc to allocate memory of size n
template<int inst>
void* __malloc_alloc_template<inst>::oom_malloc(size_t n)
{
    void (*my_handler)();
    void* result;

    for (;;) {  // infinite loop
        my_handler = __malloc_alloc_oom_handler;
        if (my_handler == 0) {  // check if my_handler is set or not
            __THROW_BAD_ALLOC;
        }
        (*my_handler)();     // caller's custom out-of-memory handler
        result = malloc(n);  // try allocate memory
        if (result) {
            return result;
        }
    }
}

// call user specific oom handler, then use realloc to allocate memory of size n again
template<int inst>
void* __malloc_alloc_template<inst>::oom_realloc(void* ptr, size_t n)
{
    void (*my_handler)();
    void* result;

    for (;;) {  // forever loop
        my_handler = __malloc_alloc_oom_handler;
        if (my_handler == 0) {  // check if my_handler is set or not
            __THROW_BAD_ALLOC;
        }
        (*my_handler)();           // caller custom out-of-memory handler
        result = realloc(ptr, n);  // try allocate memory
        if (result) {
            return result;
        }
    }
}

// directly set 'inst' to 0: this non-type template paramter is not used in our case
using malloc_alloc = __malloc_alloc_template<0>;

/**
 * @brief Sub/Second-level allocator
 * @attention Reduce fragments when allocating small memory blocks
 *            by using memory pool techniques to reduce overhead
 * @attention Consider measures to take when memory is not enough
 *
 * 16 free lists(linked list) of different size are implemented to form a memory pool.
 * If the requested memory block size is larger than 128 bytes, first-level
 * allocator is called instead. In case of oom, oom handler of first-level
 * allocator is invoked.
 */
template<bool threads, int inst>
class __default_alloc_template {
public:
    enum { __ALIGN = 8 };                           // each block size should be a multiple of 8
    enum { __MAX_BYTES = 128 };                     // size of each block in the 16th free-list
    enum { __NFREELISTS = __MAX_BYTES / __ALIGN };  // number of free-lists (16)

public:
    // typical interfaces of an allocator
    static void* allocate(size_t n);
    static void deallocate(void* p, size_t n);
    static void* reallocate(void* p, size_t old_size, size_t new_size);

private:
    // round up number of bytes to the next multiple of 8
    static size_t ROUND_UP(size_t bytes) { return (bytes + __ALIGN - 1) & ~(__ALIGN - 1); }

    // Save memory space, different purpose when using different field
    union obj {
        union obj* free_list_link;
        char client_data[1];
    };
    // 16 free-lists
    static obj* volatile free_list[__NFREELISTS];
    // freelist index start from 1 - 16
    static size_t FREELSIT_INDEX(size_t bytes) { return (bytes + __ALIGN - 1) / __ALIGN - 1; }

    // return an obj of size n, prob add a small block of size n to the free_list
    static void* refill(size_t n);
    // allocate a chunk of space for a number of ('n_objs') block of size 'size'
    static char* chunk_alloc(size_t size, int& n_objs);

private:
    // chunk allocation states
    static char* start_free;  // memory pool's starting position
    static char* end_free;    // memory pool's starting position
    static size_t heap_size;
};

// Initialize static data member values
template<bool threads, int inst>
char* __default_alloc_template<threads, inst>::start_free = 0;
template<bool threads, int inst>
char* __default_alloc_template<threads, inst>::end_free = 0;
template<bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;
template<bool threads, int inst>
typename __default_alloc_template<threads, inst>::obj* volatile __default_alloc_template<threads,
    inst>::free_list[__NFREELISTS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// static member functions definition
template<bool threads, int inst>
void* __default_alloc_template<threads, inst>::allocate(size_t n)
{
    obj* volatile* my_free_list;  // pointer of pointer
    obj* result;

    // call 1st level allocator if size larger than 128
    if (n > (size_t)__MAX_BYTES) {
        return malloc_alloc::allocate(n);
    }
    // find a suitable free_list among 16 free_lists
    my_free_list = free_list + FREELSIT_INDEX(n);
    result = *my_free_list;
    if (result == 0) {  // nullptr
        // no free list is found, refill free list
        void* r = refill(ROUND_UP(n));
        return r;
    }
    // adjust free list
    *my_free_list = result->free_list_link;
    return result;
}

template<bool threads, int inst>
void __default_alloc_template<threads, inst>::deallocate(void* p, size_t n)
{
    if (!p) {
        return;
    }
    obj* q = (obj*)p;  // avoid modifying value of p (the address)
    if (n > (size_t)__MAX_BYTES) {
        malloc_alloc::deallocate(p, n);
        return;
    }
    obj* volatile* my_free_list;
    my_free_list = free_list
        + FREELSIT_INDEX(n);  // find corresponding free-list that manages blocks of size n

    // Adjust current free list to recycle the block
    // 1. link q to current pos of free list
    q->free_list_link = (*my_free_list);
    // 2. set free-list point to q
    (*my_free_list) = q;
}

template<bool threads, int inst>
void* __default_alloc_template<threads, inst>::refill(size_t n)
{
    obj* volatile* my_free_list;
    obj* result;
    obj *current_obj, *next_obj;
    int i;

    int n_objs = 20;  // default value: get 20 new nodes (blocks)
    // call chunk_alloc(), try to get n_objs of block to be new nodes of free list
    char* chunk = chunk_alloc(n, n_objs);

    if (n_objs == 1) {  // only get 1 block, return this block to user directly
        return chunk;
    }
    // adjust free list to accept new nodes from chunk
    my_free_list = free_list + FREELSIT_INDEX(n);

    // first block is returned to user for his use,
    // so free_list should point to the next block
    result = (obj*)chunk;                          // this block is returned to user
    *my_free_list = next_obj = (obj*)(chunk + n);  // point free list to next empty block

    // connect each node from chunk to create a linked list
    for (int i = 1;; i++) {  // starting from index 1, since block 0-th is returned to user
        current_obj = next_obj;
        next_obj = (obj*)((char*)current_obj + n);  // cast a member ptr to union ptr
        if (i == n_objs - 1) {                      // last block
            current_obj->free_list_link = 0;        // last block points to NULL
            break;
        } else {
            current_obj->free_list_link = next_obj;  // link 2 blocks together, create free-list
        }
    }
    return result;  // only the first block is returned to user
}

template<bool threads, int inst>
char* __default_alloc_template<threads, inst>::chunk_alloc(size_t n, int& n_objs)
{
    char* result;
    size_t total_bytes = n * n_objs;
    size_t bytes_left = end_free - start_free;  // remaining space of the memory pool

    if (bytes_left >= total_bytes) {  // case 1. enough for all n_objs blocks
        result = start_free;
        start_free += total_bytes;
        return result;
    } else if (bytes_left >= n) {  // case 2. enough for one or more blocks
        n_objs = bytes_left / n;
        total_bytes = n * n_objs;  // updated total bytes required
        result = start_free;
        start_free += total_bytes;
        return result;
    } else {  // case 3. don't even have space for a single block
        // control how much to get from heap
        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
        if (bytes_left > 0) {  // clean up remaining little space
            // still have some little remaining space, allocate to suitable free list
            // find a suitable free list
            obj* volatile* my_free_list = free_list + FREELSIT_INDEX(bytes_left);
            // adjust free list, insert the remaining space into the head of the free list
            ((obj*)start_free)->free_list_link = *my_free_list;
            *my_free_list = (obj*)start_free;
        }
        // get some space from heap, then add to memory pool
        start_free = (char*)malloc(bytes_to_get);
        if (start_free == 0) {  // heap has not enough space, malloc() failed

            // Notes: we are not going to allocate a smaller block to the user,
            // since user requires the block with size n; instead, we may steal some
            // large enough free blocks from other suitable free-lists

            for (int i = n; i <= __MAX_BYTES; i += __ALIGN) {
                obj* volatile* my_free_list = free_list + FREELSIT_INDEX(i);
                obj* p = *my_free_list;
                if (p != 0) {  // still some free blocks inside my_free_list
                    *my_free_list = p->free_list_link;
                    start_free = (char*)p;
                    end_free = start_free + i;
                    return chunk_alloc(n, n_objs);
                }
            }
            // No any space can be obtained, even from heap and other free-list
            end_free = 0;
            // call 1st level allocator, see if oom handler can help
            // no oom handler in sub-allocator here.
            // exception may be thrown, or ease the problem of memory shortage
            start_free = (char*)malloc_alloc::allocate(bytes_to_get);
        }

        // now enough space is found, update heap_size and start, end info
        heap_size += bytes_to_get;
        end_free = start_free + bytes_to_get;

        // Call itself to correct the value of n_objs since
        // 'bytes_to_get' may not be large enough for n_obj number of blocks of size n
        return chunk_alloc(n, n_objs);
    }
}

#ifdef __USE_MALLOC
// Set alloc as malloc
typedef __malloc_alloc_template<0> malloc_alloc;
typedef malloc_alloc alloc;
#else
// Set alloc as second-level allocator
#define __NODE_ALLOCATOR_THREADS false
typedef __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0> alloc;
#endif

/**
 * @brief A thin allocator wrapper(1st/2nd level allocator) to satisfy STL standard interface
 *
 * @tparam T value type
 * @tparam Alloc 1st level allocator or sub-allocator
 *
 * @attention Each static method is just a simple forwarding functions to actual method of Alloc
 */
template<typename T, typename Alloc>
class simple_alloc {
public:
    // necessary associated types
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

public:
    static T* allocate(size_t n) { return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T)); }
    static T* allocate(void) { return (T*)Alloc::allocate(sizeof(T)); }
    static void deallocate(T* p, size_t n)
    {
        if (!p) {
            return;
        }
        if (n != 0) {
            Alloc::deallocate(p, n * sizeof(T));
        }
    }
    static void deallocate(T* p) { Alloc::deallocate(p, sizeof(T)); }
};

}  // namespace mini::mem

#endif
