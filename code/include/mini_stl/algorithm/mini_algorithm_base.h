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

/*
// A better implementation:

template<typename Iterator, typename T>
void func_impl(Iterator iter, T t){
    T tmp;  // T refers to object type that iterator pointing to
    ...
    // Do what func() originally needs to do
}

template<typename Iterator>
void func(Iterator iter){
    func_impl(iter, *iter);
}

int main(){
    int i;
    func(&i);
}

*/

}  // namespace mini::algo

#endif
