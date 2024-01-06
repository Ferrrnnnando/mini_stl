#ifndef MINI_UTILITY_HASHTABLE_H
#define MINI_UTILITY_HASHTABLE_H

#include "mini_stl/base/mini_define.h"
#include "mini_stl/container/mini_container_vector.h"
#include "mini_stl/iterator/mini_iterator.h"
#include "mini_stl/memory/mini_memory.h"
#include "mini_stl/utility/mini_utility_base.h"

namespace mini::util {

template<typename T>
struct __hashtable_node {
    __hashtable_node* next;
    T value;
};

// forward declarations

template<typename Key, typename Value, typename HashFunc, typename ExtractKey, typename EqualKey,
    typename Allocator = mem::alloc>
class hashtable;

template<typename Key, typename Value, typename HashFunc, typename ExtractKey, typename EqualKey,
    typename Allocator>
struct __hashtable_iterator;

template<typename Key, typename Value, typename HashFunc, typename ExtractKey, typename EqualKey,
    typename Allocator>
struct __hashtable_const_iterator;

template<typename Key, typename Value, typename HashFunc, typename ExtractKey, typename EqualKey,
    typename Allocator>
struct __hashtable_iterator {
    using hashtable_type = hashtable<Key, Value, HashFunc, ExtractKey, EqualKey, Allocator>;
    using node_type = __hashtable_node<Value>;
    using iterator = __hashtable_iterator<Key, Value, HashFunc, ExtractKey, EqualKey, Allocator>;
    using const_iterator =
        __hashtable_const_iterator<Key, Value, HashFunc, ExtractKey, EqualKey, Allocator>;

    using iterator_category = iter::forward_iterator_tag;
    using value_type = Value;
    using reference = Value&;
    using pointer = Value*;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    __hashtable_iterator() = default;

    __hashtable_iterator(node_type* _node, hashtable_type* _table)
        : node(_node)
        , table(_table)
    {}

    reference operator*() const { return node->value; }

    pointer operator->() const { return &(operator*()); }

    bool operator==(const iterator& other) const { return node == other.node; }

    bool operator!=(const iterator& other) const { return node != other.node; }

    // note: no backward iterator movement available

    iterator& operator++()
    {
        const node_type* old = node;
        node = node->next;
        if (!node) {
            size_type bucket_idx = table->bkt_num(old->value);
            while (!node && (++bucket_idx < table->bucket_count())) {
                node = table->buckets_[bucket_idx];
            }
        }
        return *this;
    }

    iterator operator++(int)
    {
        iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    node_type* node;
    hashtable_type* table;
};

template<typename Key, typename Value, typename HashFunc, typename ExtractKey, typename EqualKey,
    typename Allocator>
struct __hashtable_const_iterator {
    using hashtable_type = hashtable<Key, Value, HashFunc, ExtractKey, EqualKey, Allocator>;
    using node_type = __hashtable_node<Value>;
    using iterator = __hashtable_iterator<Key, Value, HashFunc, ExtractKey, EqualKey, Allocator>;
    using const_iterator =
        __hashtable_const_iterator<Key, Value, HashFunc, ExtractKey, EqualKey, Allocator>;

    using iterator_category = iter::forward_iterator_tag;
    using value_type = Value;
    using reference = const Value&;
    using pointer = const Value*;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    __hashtable_const_iterator() = default;

    __hashtable_const_iterator(const node_type* _node, const hashtable_type* _table)
        : node(_node)
        , table(_table)
    {}

    __hashtable_const_iterator(const iterator& other)
        : node(other._node)
        , table(other._table)
    {}

    reference operator*() const { return node->value; }

    pointer operator->() const { return &(operator*()); }

    bool operator==(const const_iterator& other) const { return node == other.node; }

    bool operator!=(const const_iterator& other) const { return node != other.node; }

    // note: no backward iterator movement available

    const_iterator& operator++()
    {
        const node_type* old = node;
        node = node->next;
        if (!node) {
            size_type bucket_idx = table->bkt_num(old->value);
            while (!node && (++bucket_idx < table->bucket_count())) {
                node = table->buckets_[bucket_idx];
            }
        }
        return *this;
    }

    const_iterator operator++(int)
    {
        const_iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    const node_type* node;
    const hashtable_type* table;
};

template<typename Key, typename Value, typename HashFunc, typename ExtractKey, typename EqualKey,
    typename Allocator>
class hashtable {
public:
    using key_type = Key;
    using value_type = Value;
    using const_reference = const Value&;
    using hasher = HashFunc;
    using key_equal = EqualKey;
    using key_extract = ExtractKey;
    using size_type = size_t;
    using iterator = __hashtable_iterator<Key, Value, HashFunc, ExtractKey, EqualKey, Allocator>;
    using const_iterator =
        __hashtable_const_iterator<Key, Value, HashFunc, ExtractKey, EqualKey, Allocator>;

    // allow iterator class to access protected and private members
    friend struct __hashtable_iterator<Key, Value, HashFunc, ExtractKey, EqualKey, Allocator>;
    friend struct __hashtable_const_iterator<Key, Value, HashFunc, ExtractKey, EqualKey, Allocator>;

private:
    using node_type = __hashtable_node<Value>;
    using link_type = node_type*;
    using node_allocator = mem::simple_alloc<node_type, Allocator>;
    using store_type = ctnr::vector<link_type, Allocator>;

public:
    hashtable(size_type n, const hasher& hash_func, const key_equal& equal_func)
        : hash_func_(hash_func)
        , equal_func_(equal_func)
        , get_key_func_(ExtractKey())
        , num_elements_(0)
    {
        initialize_buckets(n);
    }

public:
    // iterators

    iterator begin()
    {
        // return the first node from the first non-empty bucket
        for (size_type idx = 0; idx < buckets_.size(); ++idx) {
            if (buckets_[idx]) {
                return iterator(buckets_[idx], this);
            }
        }
        return end();
    };

    const_iterator begin() const
    {
        // return the first node from the first non-null bucket from hashmap
        for (size_type idx = 0; idx < buckets_.size(); ++idx) {
            if (buckets_[idx]) {
                return const_iterator(buckets_[idx], this);
            }
        }
        return end();
    };

    iterator end() { return iterator(0, this); };

    const_iterator end() const { return const_iterator(0, this); };

    // capacity

    size_type bucket_count() const { return buckets_.size(); }

    constexpr size_type max_bucket_count() const
    {
        return __mini_stl_prime_list[__mini_stl_num_primes - 1];
    }

    size_type size() const { return num_elements_; }

    bool empty() const { return num_elements_ == 0; }

    size_type elements_in_bucket(size_type bucket_idx) const
    {
        size_type res = 0;
        for (link_type node = buckets_[bucket_idx]; node; node = node->next) {
            ++res;
        }
        return res;
    }

    // lookup

    iterator find(const key_type& key)
    {
        // 1. find bucket index
        // 2. return the first node with matched value in this bucket
        size_type seek_bucket_idx = bkt_num_key(key);
        link_type node = buckets_[seek_bucket_idx];
        while (node) {
            if (equal_func_(key, get_key_func_(node->value))) {
                return iterator(node, this);
            }
            node = node->next;
        }
        return iterator(0, this);
    }

    size_type count(const key_type& key)
    {
        size_type seek_bucket_idx = bkt_num_key(key);
        size_type res = 0;
        link_type node = buckets_[seek_bucket_idx];
        while (node) {
            if (equal_func_(key, get_key_func_(node->value))) {
                ++res;
            }
            node = node->next;
        }
        return res;
    }

    // modifiers

    pair<iterator, bool> insert_unique(const_reference value)
    {
        resize(num_elements_ + 1);  // resize table if needed
        return insert_unique_no_resize(value);
    }

    iterator insert_equal(const_reference value)
    {
        resize(num_elements_ + 1);  // resize table if needed
        return insert_equal_no_resize(value);
    }

    void clear()
    {
        for (size_type idx = 0; idx < buckets_.size(); ++idx) {
            link_type first = buckets_[idx];
            while (first) {
                link_type next = first->next;
                delete_node(first);
                first = next;
            }
            buckets_[idx] = 0;
        }
        num_elements_ = 0;
    }

    void copy_from(const hashtable& other)
    {
        buckets_.clear();
        buckets_.reserve(other.buckets_.size());
        buckets_.insert(buckets_.end(), buckets_.size(), (link_type)0);

        try {
            for (size_type idx = 0; idx < other.buckets_.size(); ++idx) {
                if (const link_type node = other.buckets_[idx]) {
                    // copy the first node, assign it to buckets_
                    link_type copy = new_node(node->value);
                    buckets_[idx] = copy;

                    for (link_type next = node->next; next; node = next, next = node->next) {
                        copy->next = new_node(next->value);
                        copy = copy->next;
                    }
                }
            }
            num_elements_ = other.num_elements_;
        } catch (const std::exception& e) {
            clear();
            std::cerr << e.what() << '\n';
        }
    }

private:
    void initialize_buckets(size_type n)
    {
        const size_type n_buckets = next_size(n);
        buckets_.reserve(n_buckets);
        // initialize each bucket to NULL
        buckets_.insert(buckets_.end(), n_buckets, (link_type)0);
        num_elements_ = 0;
    }

    size_type next_size(size_type n) const { return __mini_stl_next_prime(n); }

    void resize(size_type n)
    {
        const size_type old_n = buckets_.size();
        if (n <= old_n) {
            return;
        }

        // rebuild table when new size is larger than old bucket size
        n = next_size(n);
        store_type new_buckets(n, (link_type)0);
        for (size_type idx = 0; idx < old_n; ++idx) {
            link_type first = buckets_[idx];
            while (first) {
                // get index in the new buckets
                size_type new_idx = bkt_num(first->value, n);

                // old buckets move to next node
                buckets_[idx] = first->next;

                // move first node to head of new_buckets
                first->next = new_buckets[new_idx];
                new_buckets[new_idx] = first;

                first = buckets_[idx];
            }
        }
        buckets_.swap(new_buckets);  // new_buckets will be freed when leave
    }

    pair<iterator, bool> insert_unique_no_resize(const_reference value)
    {
        size_type idx = bkt_num(value, bucket_count());
        link_type first = buckets_[idx];

        for (link_type node = first; node; node = node->next) {
            if (equal_func_(get_key_func_(node->value), get_key_func_(value))) {
                // duplicated key exists, return
                return {iterator(node, this), false};
            }
        }

        link_type tmp_node = new_node(value);
        tmp_node->next = first;
        buckets_[idx] = tmp_node;
        ++num_elements_;
        return {iterator(tmp_node, this), true};
    }

    // insertion must success since duplcated key is allowed
    iterator insert_equal_no_resize(const_reference value)
    {
        size_type idx = bkt_num(value, bucket_count());
        link_type first = buckets_[idx];

        for (link_type node = first; node; node = node->next) {
            if (equal_func_(get_key_func_(node->value), get_key_func_(value))) {
                // have duplicated key, immediately insert
                link_type tmp_node = new_node(value);
                tmp_node->next = node->next;
                node->next = tmp_node;
                ++num_elements_;
                return iterator(tmp_node, this);
            }
        }

        link_type tmp_node = new_node(value);
        tmp_node->next = first;
        buckets_[idx] = tmp_node;
        ++num_elements_;
        return iterator(tmp_node, this);
    }

    size_type bkt_num(value_type value) const { return bkt_num_key(get_key_func_(value)); }

    size_type bkt_num(value_type value, size_type n) const
    {
        return bkt_num_key(get_key_func_(value), n);
    }

    size_type bkt_num_key(key_type key) const { return bkt_num_key(key, bucket_count()); }

    size_type bkt_num_key(key_type key, size_type n) const { return hash_func_(key) % n; }

    link_type new_node(const_reference value)
    {
        link_type node = node_allocator::allocate();

        // initialize new node's members
        node->next = 0;
        try {
            mem::construct(&(node->value), value);
            return node;
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            node_allocator::deallocate(node);
            return 0;
        }
    }

    void delete_node(link_type node)
    {
        mem::destroy(&(node->value));
        node_allocator::deallocate(node);
    }

private:
    hasher hash_func_;
    key_equal equal_func_;
    key_extract get_key_func_;
    store_type buckets_;
    size_type num_elements_;
    iterator begin_;
    iterator end_;
};

}  // namespace mini::util

#endif
