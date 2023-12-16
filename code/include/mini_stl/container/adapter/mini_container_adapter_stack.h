#ifndef MINI_CONTAINER_ADAPTER_STACK_H
#define MINI_CONTAINER_ADAPTER_STACK_H

#include "mini_stl/container/mini_container_deque.h"

namespace mini::ctnr {

template<typename T, typename Sequence = deque<T>>
class stack {
public:
    typedef stack<T, Sequence> self;
    typedef typename Sequence::value_type value_type;
    typedef typename Sequence::size_type size_type;
    typedef typename Sequence::reference reference;
    typedef typename Sequence::const_reference const_reference;

public:
    stack() = default;

    bool empty() const { return c_.empty(); }

    size_type size() const { return c_.size(); }

    reference top() { return c_.back(); }

    const_reference top() const { return c_.back(); }

    void push(const_reference value) { c_.push_back(value); }

    void pop() { c_.pop_back(); }

protected:
    Sequence c_;
};

}  // namespace mini::ctnr

#endif
