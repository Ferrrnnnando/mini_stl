#ifndef MINI_CONTAINER_MAP_H
#define MINI_CONTAINER_MAP_H

#include "mini_stl/functional/mini_functional.h"
#include "mini_stl/utility/mini_utility.h"

namespace mini::ctnr {

template<typename Key, typename Value, typename Compare = func::less<Key>,
    typename Allocator = mem::alloc>
class map {
public:
    using key_type = Key;
    using value_type = Value;
    using mapped_type = Value;
    using element_type = util::pair<const Key, Value>;
    using key_compare_type = Compare;
    using store_type =
        util::rb_tree<key_type, element_type, func::select1st, key_compare_type, Allocator>;

    class value_compare {
        value_compare(const Compare& comp)
            : compare_(comp)
        {}

        bool operator()(const element_type& x, const element_type& y) { return compare_(x, y); }

        Compare compare_;
    };
    using value_compare_type = value_compare;

public:
    using pointer = typename store_type::const_pointer;
    using const_pointer = typename store_type::const_pointer;
    using reference = typename store_type::const_reference;
    using const_reference = typename store_type::const_reference;
    using iterator = typename store_type::iterator;
    using const_iterator = typename store_type::const_iterator;
    using size_type = typename store_type::size_type;
    using difference_type = typename store_type::difference_type;

public:
    map()
        : store_(Compare())
    {}

    explicit map(const Compare& comp)
        : store_(comp)
    {}

public:
    // operators

    Value& operator[](const key_type& k)
    {
        return (*(insert(element_type(k, Value())).first)).second;
    }

    // accessors

    iterator begin() { return store_.begin(); }

    const_iterator begin() const { return store_.begin(); }

    iterator end() { return store_.end(); }

    const_iterator end() const { return store_.end(); }

    // capacity

    bool empty() const { return store_.empty(); }

    size_type size() const { return store_.size(); }

    size_type max_size() const { return store_.max_size(); }

    // modifiers

    util::pair<iterator, bool> insert(const_reference value) { return store_.insert_unique(value); }

    // lookup

    iterator find(const key_type& k) { return store_.find(k); }

    // observers

    key_compare_type key_comp() const { return store_.key_comp(); }

    value_compare_type value_comp() const
    {
        // value comparator uses the same comparator as key comparator
        return value_compare_type(key_comp());
    }

private:
    store_type store_;
};

}  // namespace mini::ctnr

#endif
