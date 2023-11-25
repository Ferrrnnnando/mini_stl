#ifndef MINI_CONTAINER_VECTOR_H
#define MINI_CONTAINER_VECTOR_H

#include "mini_stl/memory/mini_memory.h"

namespace mini::container {

template<typename T, typename Alloc = mini::memory::alloc>
class vector {
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef value_type* iterator;
    typedef value_type& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

public:
    vector()
        : start(0)
        , finish(0)
        , end_of_storage(0)
    {}

    explicit vector(size_type n) { fill_initialize(n, value_type()); }

    vector(size_type n, const_reference value) { fill_initialize(n, value); }

    vector(int n, const_reference value) { fill_initialize(n, value); }

    vector(long n, const_reference value) { fill_initialize(n, value); }

    ~vector()
    {
        memory::destroy(start, finish);
        deallocate();
    }

public:
    iterator begin() { return start; }

    iterator end() { return finish; }

    reference front() { return *begin(); }

    reference back() { return *(end() - 1); }

    size_type size() { return size_type(end() - begin()); }

    size_type capacity() { return size_type(end_of_storage - begin()); }

    bool empty() { return start == finish; }

    reference operator[](size_type n) { return *(begin() + n); }

    void push_back(const_reference value)
    {
        if (finish != end_of_storage) {  // no need to allocate space
            memory::construct(finish, value);
            ++finish;
        } else {
            insert_aux(end(), value);
        }
    }

    void pop_back()
    {
        --finish;
        memory::destroy(finish);
    }

    iterator erase(iterator pos)
    {
        // remove non-tail elements: move all elements behind it to front
        if (pos + 1 != end()) {
            // memory::copy(pos + 1, end(), pos);  // TODO: global copy() to be implemented later
            std::copy(pos + 1, end(), pos);
        }
        --finish;
        memory::destroy(finish);
        return pos;
    }

    /**
     * @brief Erase all elements within range [first, last)
     *
     * @param first range starting position (inclusively)
     * @param last range ending position (exclusively)
     * @return iterator range starting position
     */
    iterator erase(iterator first, iterator last)
    {
        // iterator i = memory::copy(last, finish, first);  // TODO: global copy() to be implemented later
        iterator i = std::copy(last, finish, first);
        memory::destroy(i, finish);
        finish = finish - (last - first);
        return first;
    }

    /**
     * @brief Starting at position 'pos', insert 'n' number of elements with value 'value'
     *
     * @param pos Position to start insertion
     * @param n Number of elements to insert
     * @param value Value of element to insert
     *
     * @attention The uninitialized allocated space needs to initialized using 'uninitialzied_copy'
     *            or 'uninitialized_fill_n' first, before doing other object copying actions,
     *            otherwise, assignment operator of objects may not be ready.
     */
    void insert(iterator pos, size_type n, const_reference value)
    {
        if (n == 0) {
            return;
        }

        if (size_type(end_of_storage - finish) >= n) {  // reserved space enough for insertion
            const size_type num_elements_after = finish - pos;
            iterator old_finish = finish;
            if (num_elements_after > n) {  // number of elements after insert pos > n
                // |--- n ---|
                // |-- elements after--|--------> unintialized
                // pos               finish

                // copy last n element of current vector at position finish
                memory::uninitialized_copy(finish - n, finish, finish);
                finish += n;
                std::copy_backward(pos, old_finish - n, old_finish);
                std::fill(pos, pos + n, value);
            } else {  // number of existing elements after insert pos <= n
                // |---       n         ---|
                // |-- elements after--|--------> unintialized
                // pos               finish

                memory::uninitialized_fill_n(finish, n - num_elements_after, value);
                finish += (n - num_elements_after);
                memory::uninitialized_copy(pos, old_finish, finish);
                finish += num_elements_after;
                std::fill(pos, old_finish, value);
            }
        } else {  // reserved space less than number of new elements going to insert
            const size_type old_size = size();
            const size_type len = old_size + std::max(old_size, n);

            // allocate new memory with enough size
            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;

            try {
                new_finish = memory::uninitialized_copy(start, pos, new_start);
                new_finish = memory::uninitialized_fill_n(new_finish, n, value);
                new_finish = memory::uninitialized_copy(pos, finish, new_finish);
            } catch (const std::exception& e) {
                /// failure: rollback
                memory::destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                std::cerr << e.what() << '\n';
                throw;
            }

            // commit success: destroy old memory
            memory::destroy(start, finish);
            deallocate();

            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
    }

    void resize(size_type new_size, const_reference value = value_type())
    {
        if (new_size < size()) {
            // content is reduced to first n elements
            erase(begin() + new_size, end());
        } else {
            insert(end(), new_size - size(), value);
        }
    }

    void clear() { erase(begin(), end()); }

protected:
    /**
     * @brief Insert a value at a position
     *
     * @param pos position to insert
     * @param value element to insert
     */
    void insert_aux(iterator pos, const_reference value)
    {
        // Have reserved space: not yet reached capacity
        if (finish != end_of_storage) {
            // construct an element at the end having same value as the last element
            memory::construct(finish, *(finish - 1));
            ++finish;
            // shift backward all elements starting from insert position
            std::copy_backward(pos, finish - 2, finish - 1);
            *pos = value;
        } else {
            // No reserved space available

            // doubles original size, first half to store original data, another half for future reserve
            const size_type old_size = size();
            const size_type len = (old_size != 0 ? old_size * 2 : 1);

            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;

            try {  // copy data from original memory space to new memory space
                new_finish = memory::uninitialized_copy(start, pos, new_start);
                memory::construct(new_finish, value);
                ++new_finish;
                new_finish = memory::uninitialized_copy(pos, finish, new_finish);
            } catch (...) {
                // commit or rollback semantics
                memory::destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw;
            }

            // destroy and free original memory
            memory::destroy(begin(), end());
            deallocate();

            // update internal iterators to new position
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
    }

    void deallocate()
    {
        if (start) {
            data_allocator::deallocate(start, end_of_storage - start);
        }
    }

    void fill_initialize(size_type n, const_reference value)
    {
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }

    iterator allocate_and_fill(size_type n, const_reference value)
    {
        iterator res = data_allocator::allocate(n);
        memory::uninitialized_fill_n(res, n, value);
        return res;
    }

protected:
    typedef mini::memory::simple_alloc<value_type, Alloc> data_allocator;
    iterator start;           // position of first element
    iterator finish;          // position after the last element
    iterator end_of_storage;  // capacity of vector(include reserved but not used spaces)
};

}  // namespace mini::container

#endif
