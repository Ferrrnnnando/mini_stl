#ifndef MINI_ALGORITHM_BASE_H
#define MINI_ALGORITHM_BASE_H

namespace mini::algo {

// notes: 'associated type' is used here: T
// Cons: this simple find algo needs to know object type T
template<typename InputIterator, typename T>
InputIterator find(InputIterator first, InputIterator last, const T& value)
{
    while (first != last && (*first) != value) {
        ++first;
    }
    return first;
}

template<typename InputIterator, typename Function>
InputIterator for_each(InputIterator first, InputIterator last, Function& func)
{
    for (; first != last; ++first) {
        func(*first);
    }
    return first;
}

template<typename InputIt, typename OutputIt>
OutputIt copy(InputIt first, InputIt last, OutputIt d_first)
{
    while (first != last) {
        *d_first = *first;
        ++first;
        ++d_first;
    }
    return d_first;
}

template<typename BidirIt1, typename BidirIt2>
BidirIt2 copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 d_last)
{
    while (first != last) {
        *(--d_last) = *(--last);
    }
    return d_last;
}

}  // namespace mini::algo

#endif
