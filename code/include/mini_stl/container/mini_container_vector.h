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
    typedef const value_type* const_iterator;
    typedef value_type& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef mini::memory::simple_alloc<value_type, Alloc> allocator_type;

public:
    vector()
        : begin_(0)
        , end_(0)
        , end_of_storage_(0)
    {}

    explicit vector(size_type n) { fill_initialize(n, value_type()); }

    vector(size_type n, const_reference value) { fill_initialize(n, value); }

    vector(int n, const_reference value) { fill_initialize(n, value); }

    vector(long n, const_reference value) { fill_initialize(n, value); }

    ~vector() { destroy_and_deallocate(); }

public:
    // Element access
    reference front() { return *begin(); }

    reference back() { return *(end() - 1); }

    reference operator[](size_type n) { return *(begin() + n); }

    reference at(size_type n)
    {
        if (n >= size()) {
            throw std::out_of_range("mini::container::vector: out_of_range failure: n >= size()");
        }
        return *(begin() + n);
    }

    pointer data() { return &(*begin()); }

    // iterators
    iterator begin() { return begin_; }

    iterator end() { return end_; }

    // capacity
    size_type size() { return size_type(end() - begin()); }

    size_type capacity() { return size_type(end_of_storage_ - begin()); }

    bool empty() const noexcept { return begin_ == end_; }

    void reserve(size_type new_cap)
    {
        if (new_cap <= capacity()) {
            return;
        }

        if (new_cap > max_size()) {
            throw std::length_error("new_cap > max_size()");
        }

        iterator new_start = data_allocator::allocate(new_cap);
        iterator new_finish = new_start;

        // For a single statement like this: probably won't need a try catch to rollback
        try {
            new_finish = memory::uninitialized_copy(begin_, end_, new_start);
        } catch (const std::exception& e) {
            // rollback to original
            memory::destroy(new_start, new_finish);
            data_allocator::deallocate(new_start, new_cap);
            std::cerr << e.what() << '\n';
            throw;
        }

        destroy_and_deallocate();
        begin_ = new_start;
        end_ = new_finish;
        end_of_storage_ = new_start + new_cap;
    }

    /**
     * @brief Return maximum number of elements the container is able to hold due to system or
     *        library implementation limitations.
     *
     * @return size_type
     */
    size_type max_size() const noexcept { return std::numeric_limits<difference_type>::max(); }

    // // TODO:
    // void shrink_to_fit(){
    //     vector<T, Alloc>(begin(), end(), end()).swap(*this);
    // }

    // Modifiers
    void clear() { erase(begin(), end()); }

    void push_back(const_reference value)
    {
        if (end_ != end_of_storage_) {  // no need to allocate space
            memory::construct(end_, value);
            ++end_;
        } else {
            insert_aux(end(), value);
        }
    }

    void pop_back()
    {
        --end_;
        memory::destroy(end_);
    }

    iterator erase(iterator pos)
    {
        // remove non-tail elements: move all elements behind it to front
        if (pos + 1 != end()) {
            // memory::copy(pos + 1, end(), pos);  // TODO: global copy() to be implemented later
            std::copy(pos + 1, end(), pos);
        }
        --end_;
        memory::destroy(end_);
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
        // iterator i = memory::copy(last, end_, first);  // TODO: global copy() to be implemented later
        iterator i = std::copy(last, end_, first);
        memory::destroy(i, end_);
        end_ = end_ - (last - first);
        return first;
    }

    // TODO
    iterator insert(const_iterator pos, const_reference value) { return NULL; }

    // // TODO
    // template<typename InputIt>
    // iterator insert(const_iterator pos, InputIt first, InputIt last)
    // {
    //     return NULL;
    // }

    /**
     * @brief Starting at position 'pos', insert 'n' number of elements with value 'value'
     *
     * @param pos Position to begin_ insertion
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

        if (size_type(end_of_storage_ - end_) >= n) {  // reserved space enough for insertion
            const size_type num_elements_after = end_ - pos;
            iterator old_finish = end_;
            if (num_elements_after > n) {  // number of elements after insert pos > n
                // |--- n ---|
                // |-- elements after--|--------> unintialized
                // pos               end_

                // copy last n element of current vector at position end_
                memory::uninitialized_copy(end_ - n, end_, end_);
                end_ += n;
                std::copy_backward(pos, old_finish - n, old_finish);
                std::fill(pos, pos + n, value);
            } else {  // number of existing elements after insert pos <= n
                // |---       n         ---|
                // |-- elements after--|--------> unintialized
                // pos               end_

                memory::uninitialized_fill_n(end_, n - num_elements_after, value);
                end_ += (n - num_elements_after);
                memory::uninitialized_copy(pos, old_finish, end_);
                end_ += num_elements_after;
                std::fill(pos, old_finish, value);
            }
        } else {  // reserved space less than number of new elements going to insert
            const size_type old_size = size();
            const size_type len = old_size + std::max(old_size, n);

            // allocate new memory with enough size
            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;

            try {
                new_finish = memory::uninitialized_copy(begin_, pos, new_start);
                new_finish = memory::uninitialized_fill_n(new_finish, n, value);
                new_finish = memory::uninitialized_copy(pos, end_, new_finish);
            } catch (const std::exception& e) {
                /// failure: rollback
                memory::destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                std::cerr << e.what() << '\n';
                throw;
            }

            destroy_and_deallocate();
            begin_ = new_start;
            end_ = new_finish;
            end_of_storage_ = new_start + len;
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

    // TODO: Verify correctness
    void swap(vector<T, Alloc>& rhs)
    {
        // TODO: relace std::swap
        std::swap(begin_, rhs.begin_);
        std::swap(end_, rhs.end_);
        std::swap(end_of_storage_, rhs.end_of_storage_);
    }

    // TODO: support C++11 features
    // emplace
    // emplace_back

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
        if (end_ != end_of_storage_) {
            // construct an element at the end having same value as the last element
            memory::construct(end_, *(end_ - 1));
            ++end_;
            // shift backward all elements starting from insert position
            std::copy_backward(pos, end_ - 2, end_ - 1);
            *pos = value;
        } else {
            // No reserved space available

            // doubles original size, first half to store original data, another half for future reserve
            const size_type old_size = size();
            const size_type len = (old_size != 0 ? old_size * 2 : 1);

            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;

            try {  // copy data from original memory space to new memory space
                new_finish = memory::uninitialized_copy(begin_, pos, new_start);
                memory::construct(new_finish, value);
                ++new_finish;
                new_finish = memory::uninitialized_copy(pos, end_, new_finish);
            } catch (...) {
                // commit or rollback semantics
                memory::destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw;
            }

            // destroy and free original memory
            destroy_and_deallocate();

            // update internal iterators to new position
            begin_ = new_start;
            end_ = new_finish;
            end_of_storage_ = new_start + len;
        }
    }

    void fill_initialize(size_type n, const_reference value)
    {
        begin_ = allocate_and_fill(n, value);
        end_ = begin_ + n;
        end_of_storage_ = end_;
    }

    iterator allocate_and_fill(size_type n, const_reference value)
    {
        iterator res = data_allocator::allocate(n);
        memory::uninitialized_fill_n(res, n, value);
        return res;
    }

    void destroy_and_deallocate()
    {
        destroy();
        deallocate();
    }

    void destroy() { memory::destroy(begin(), end()); }

    void deallocate()
    {
        if (begin_) {
            data_allocator::deallocate(begin_, end_of_storage_ - begin_);
        }
    }

protected:
    typedef allocator_type data_allocator;
    iterator begin_;           // position of first element
    iterator end_;             // position after the last element
    iterator end_of_storage_;  // capacity of vector(include reserved but not used spaces)
};

}  // namespace mini::container

#endif
