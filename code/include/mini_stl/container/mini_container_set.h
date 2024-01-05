#ifndef MINI_CONTAINER_SET_H
#define MINI_CONTAINER_SET_H

#include "mini_stl/functional/mini_functional.h"
#include "mini_stl/utility/mini_utility.h"

namespace mini::ctnr {

template<typename Key, typename Compare = func::less<Key>, typename Allocator = mem::alloc>
class set {
public:
    using key_type = Key;
    using value_type = Key;
    using key_compare = Compare;
    using value_compare = Compare;
    using allocator_type = Allocator;

private:
    using store_type = util::rb_tree<key_type, value_type, func::identity, key_compare, Allocator>;
    using self = set<Key, Compare, Allocator>;

public:
    using pointer = typename store_type::const_pointer;
    using const_pointer = typename store_type::const_pointer;
    using reference = typename store_type::const_reference;
    using const_reference = typename store_type::const_reference;
    using iterator = typename store_type::const_iterator;        // write operation is not allowed
    using const_iterator = typename store_type::const_iterator;  // write operation is not allowed
    using size_type = typename store_type::size_type;
    using difference_type = typename store_type::difference_type;

public:
    set()
        : rbt(Compare())
    {}

    explicit set(const Compare& comp)
        : rbt(comp)
    {}

    // TODO:
    // template<typename InputIterator>
    // set(InputIterator first, InputIterator last, const Compare& comp)
    //     : rbt(comp)
    // {
    //     rbt.insert_unique(first, last);
    // }

    // TODO:
    // set(const self& other)
    //     : rbt(other.rbt)
    // {}

public:
    // TODO:
    // self& operator=(const self& other) { rbt = other.rbt; }

    // iterators

    iterator begin() { return rbt.begin(); }

    const_iterator begin() const { return rbt.begin(); }

    iterator end() { return rbt.end(); }

    const_iterator end() const { return rbt.end(); }

    // capacity

    bool empty() const { return rbt.empty(); }

    size_type size() const { return rbt.size(); }

    size_type max_size() const { return rbt.max_size(); }

    // modifiers

    util::pair<iterator, bool> insert(const_reference value)
    {
        // insert_unique() returns a non-const iterator
        // need to convert it a const iterator we used here
        util::pair<typename store_type::iterator, bool> ret = rbt.insert_unique(value);
        return {ret.first, ret.second};
    }

    // set operations

    iterator find(const key_type& k) { return rbt.find(k); }

    // observers

    key_compare key_comp() const { return rbt.key_comp(); }

    value_compare value_comp() const { return rbt.key_comp(); }

private:
    store_type rbt;
};

}  // namespace mini::ctnr

#endif
