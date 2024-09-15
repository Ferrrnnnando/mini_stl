#ifndef MINI_ITERATOR_BACK_INSERT_ITERATOR_H
#define MINI_ITERATOR_BACK_INSERT_ITERATOR_H

#include "mini_stl/iterator/mini_iterator_base.h"

namespace mini::iter {

/**
 * @brief Assignment of this iterator would insert value to the back of container.
 * @attention Only containers that provide push_back() are supported.
 *
 * @tparam Container
 */
template<typename Container>
class back_insert_iterator {
public:
    using iterator_category = output_iterator_tag;
    using value_type = typename Container::value_type;
    using reference_type = typename Container::reference;
    using pointer = typename Container::pointer;
    using difference_type = void;
    using self_type = back_insert_iterator<Container>;

public:
    explicit back_insert_iterator(Container& container)
        : container_(&container)
    {}

    // assignment operation becomes back insertion operation
    self_type& operator=(const value_type& value)
    {
        container_->push_back(value);
        return *this;
    }

    // closed operators
    self_type& operator*() { return *this; }

    self_type& operator++() { return *this; }

    self_type& operator++(int) { return *this; }

private:
    Container* container_{};
};

template<typename Container>
inline back_insert_iterator<Container> back_inserter(Container& c)
{
    return back_insert_iterator<Container>(c);
}

}  // namespace mini::iter

#endif
