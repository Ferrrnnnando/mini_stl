#ifndef MINI_CONTAINER_LIST_TOY_H
#define MINI_CONTAINER_LIST_TOY_H

#include <iostream>

namespace mini::container {

template<typename T>
class ListItem {
public:
    using value_type = T;

    ListItem(value_type value, ListItem* next = nullptr)
        : value_(value)
        , next_(next)
    {}

    void set_value(value_type value) { value_ = value; }
    void set_next(ListItem* next) { next_ = next; }
    value_type value() const { return value_; }
    ListItem* next() const { return next_; }

private:
    value_type value_;
    ListItem* next_;  // singly linked list
};

template<typename T>
class List {
public:
    using value_type = T;
    using pointer_type = value_type*;
    using reference_type = value_type&;

public:
    void insert_front(value_type value)
    {
        auto* item = new ListItem<value_type>(value, front_);
        front_ = item;
        size_++;
    }
    void insert_end(value_type value)
    {
        auto* item = new ListItem<value_type>(value, nullptr);
        ListItem<value_type>* last = front_;
        while (last->next() != end_) {
            last = last->next();
        }
        last->set_next(item);
        end_ = item->next();
        size_++;
    }
    void display(std::ostream& os = std::cout)
    {
        for (ListItem<value_type>* ptr = front_; ptr != end_; ptr = ptr->next()) {
            os << ptr->value() << " ";
        }
        os << std::endl;
    }
    ListItem<value_type>* front() { return front_; }
    ListItem<value_type>* end() { return end_; }

private:
    ListItem<value_type>* front_ = nullptr;
    // always pointing to null, indicating the end of a container, just like stl
    ListItem<value_type>* end_ = nullptr;
    long size_ = 0;
};

}  // namespace mini::container

#endif
