#ifndef MINI_ALGORITHM_BASE_H
#define MINI_ALGORITHM_BASE_H

namespace mini::algo {

template<typename InputIterator, typename T>
InputIterator find(InputIterator first, InputIterator last, const T& value)
{
    while (first != last && (*first).value() != value) {
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

}  // namespace mini::algo

#endif
