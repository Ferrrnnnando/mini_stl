#ifndef MINI_CONTAINER_UNORDERED_SET_H
#define MINI_CONTAINER_UNORDERED_SET_H

#include "mini_stl/functional/mini_functional.h"
#include "mini_stl/utility/mini_utility_hashtable.h"

namespace mini::ctnr {

template<typename Value, typename HashFunc, typename EqualKey, typename Allocator = mem::alloc>
class unordered_set {
private:
    using store_type = util::hashtable<Value, Value, HashFunc, func::identity, EqualKey, Allocator>;

public:
    using key_type = typename store_type::key_type;
    using value_type = typename store_type::value_type;
    using hasher = typename store_type::hasher;
    using key_equal = typename store_type::key_equal;

    using size_type = typename store_type::size_type;
    using difference_type = typename store_type::difference_type;
    using pointer = typename store_type::const_pointer;
    using const_pointer = typename store_type::const_pointer;
    using reference = typename store_type::const_reference;
    using const_reference = typename store_type::const_reference;

    using iterator = typename store_type::const_iterator;
    using const_iterator = typename store_type::const_iterator;

public:
    explicit unordered_set(size_type bucket_count = 100, const hasher& hash_func = hasher(),
        const key_equal& key_equal_func = key_equal())
        : table_(bucket_count, hash_func, key_equal_func)
    {}

public:
    // capacity

    size_type size() const { return table_.size(); }

    size_type max_size() const { return table_.max_size(); }

    bool empty() const { return table_.empty(); }

    size_type bucket_count() const { return table_.bucket_count(); }

    size_type max_bucket_count() const { return table_.max_bucket_count(); }

    size_type elements_in_bucket(size_type bucket_idx) const
    {
        return table_.elements_in_bucket(bucket_idx);
    }

    // iterators

    iterator begin() const { return table_.begin(); }

    iterator end() const { return table_.end(); }

    // lookup

    iterator find(const key_type& key) const { return table_.find(key); }

    size_type count(const key_type& key) const { return table_.count(key); }

    // modifiers

    util::pair<iterator, bool> insert(const_reference value)
    {
        auto p = table_.insert_unique(value);
        return {p.first, p.second};
    }

    template<typename InputIterator>
    void insert(InputIterator first, InputIterator last)
    {
        table_.insert_unique(first, last);
    }

    void clear() { table_.clear(); }

    // observers

    hasher get_hash_func() { return table_.get_hash_func(); }

    key_equal get_key_equal_func() { return table_.get_key_equal_func(); }

private:
    store_type table_;
};

}  // namespace mini::ctnr

#endif
