#ifndef MINI_CONTAINER_ADAPTER_PRIORITY_PRIORITY_QUEUE_H
#define MINI_CONTAINER_ADAPTER_PRIORITY_PRIORITY_QUEUE_H

#include "mini_stl/algorithm/mini_algorithm_heap.h"
#include "mini_stl/container/mini_container_vector.h"

namespace mini::ctnr {

template<typename T, typename Sequence = vector<T>,
    typename Compare = std::less<typename Sequence::value_type>>
class priority_queue {
public:
    typedef priority_queue<T, Sequence> self;
    typedef typename Sequence::value_type value_type;
    typedef typename Sequence::size_type size_type;
    typedef typename Sequence::reference reference;
    typedef typename Sequence::const_reference const_reference;

public:
    priority_queue()
        : c_()
        , comp_(){};

    explicit priority_queue(const Compare& comp)
        : c_()
        , comp_(comp)
    {}

    template<typename InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare& comp = Compare{})
        : c_(first, last)
        , comp_(comp)
    {
        algo::make_heap(c_.begin(), c_.end());
    }

public:
    bool empty() const { return c_.empty(); }

    size_type size() const { return c_.size(); }

    const_reference top() const { return c_.front(); }

    void push(const_reference value)
    {
        try {
            c_.push_back(value);
            algo::push_heap(c_.begin(), c_.end());
        } catch (...) {
            c_.clear();
        }
    }

    void pop()
    {
        try {
            algo::pop_heap(c_.begin(), c_.end());
            c_.pop_back();
        } catch (...) {
            c_.clear();
        }
    }

protected:
    Sequence c_;
    Compare comp_;  // compare for value
};

}  // namespace mini::ctnr

#endif
