#ifndef MINI_UTILITY_RBTREE_H
#define MINI_UTILITY_RBTREE_H

#include "mini_stl/functional/mini_functional.h"
#include "mini_stl/iterator/mini_iterator.h"
#include "mini_stl/memory/mini_memory.h"

#include "mini_stl/utility/mini_utility_base.h"

namespace mini::util {

using __rb_tree_color_type = bool;
const __rb_tree_color_type __rb_tree_red = false;
const __rb_tree_color_type __rb_tree_black = true;

struct __rb_tree_node_base;
inline void __rb_tree_rotate_left(__rb_tree_node_base*, __rb_tree_node_base*&);
inline void __rb_tree_rotate_right(__rb_tree_node_base*, __rb_tree_node_base*&);
inline void __rb_tree_rebalance(__rb_tree_node_base*, __rb_tree_node_base*&);

struct __rb_tree_node_base {
    using color_type = __rb_tree_color_type;
    using node_base_ptr = __rb_tree_node_base*;

    color_type color;
    node_base_ptr parent;
    node_base_ptr left;
    node_base_ptr right;

    /**
     * @brief Get node pointer with minimum value rooted at p.
     *
     * @param p Node pointer
     * @return node_base_ptr Node pointer with miminum value, return NULL when p is NULL
     */
    static node_base_ptr minimum(node_base_ptr p)
    {
        while (p && p->left) {
            p = p->left;
        }
        return p;
    }

    /**
     * @brief Get node pointer with maximum value rooted at p.
     *
     * @param p Node pointer
     * @return node_base_ptr Node pointer with maximum value, return NULL when p is NULL
     */
    static node_base_ptr maximum(node_base_ptr p)
    {
        while (p && p->right) {
            p = p->right;
        }
        return p;
    }
};

template<typename T>
struct __rb_tree_node : public __rb_tree_node_base {
    using link_type = __rb_tree_node<T>*;
    T value;
};

struct __rb_tree_iterator_base {
    using node_base_ptr = typename __rb_tree_node_base::node_base_ptr;
    using iterator_category = iter::bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;

    node_base_ptr node;

    void increment()
    {
        if (node->right) {
            // case 1: have right child -> go down its left subtree continuously
            node = node->right;
            while (node->left) {
                node = node->left;
            }
        } else {
            // case 2: no right child -> go up to search for appropriate parent
            // until current node is a left child of parent
            node_base_ptr parent = node->parent;
            while (node == parent->right) {
                node = parent;
                parent = parent->parent;
            }
            if (node->right != parent) {  // TODO: when this condition be true?
                node = parent;
            }
        }
    }

    void decrement()
    {
        if (node->color == __rb_tree_red && node == node->parent->parent) {
            // TODO: understand this case
            node = node->right;
        } else if (node->left) {
            node = node->left;
            while (node->right) {
                // TODO: verify
                node = node->right;
            }
        } else {
            node_base_ptr parent = node->parent;
            while (node == parent->left) {
                node = parent;
                parent = parent->parent;
            }
            node = parent;
        }
    }
};

template<typename T, typename Ref, typename Ptr>
struct __rb_tree_iterator : public __rb_tree_iterator_base {
    using value_type = T;
    using reference = Ref;
    using pointer = Ptr;
    using iterator = __rb_tree_iterator<T, T&, T*>;
    using const_iterator = __rb_tree_iterator<T, const T&, const T*>;
    using self = __rb_tree_iterator<T, Ref, Ptr>;
    using link_type = typename __rb_tree_node<T>::link_type;

    __rb_tree_iterator() = default;

    __rb_tree_iterator(link_type x) { node = x; }

    __rb_tree_iterator(const iterator& other) { node = other.node; }

    __rb_tree_iterator(const const_iterator& other) { node = other.node; }

    bool operator==(const self& other) const { return node == other.node; }

    bool operator!=(const self& other) const { return node != other.node; }

    reference operator*() const { return ((link_type)node)->value; }

    pointer operator->() const { return &operator*(); }

    self& operator++()
    {
        increment();
        return *this;
    }

    self operator++(int)
    {
        self tmp = *this;
        increment();
        return tmp;
    }

    self& operator--()
    {
        decrement();
        return *this;
    }

    self operator--(int)
    {
        self tmp = *this;
        decrement();
        return tmp;
    }
};

template<typename Key, typename Value, typename KeyOfValue, typename Compare = func::less<Key>,
    typename Alloc = mem::alloc>
class rb_tree {
protected:
    using void_pointer = void*;
    using base_link_type = typename __rb_tree_iterator_base::node_base_ptr;
    using node_type = __rb_tree_node<Value>;
    using color_type = __rb_tree_color_type;
    using node_allocator = mem::simple_alloc<node_type, Alloc>;  // allocate a node at a time
    using self = rb_tree<Key, Value, KeyOfValue, Compare, Alloc>;

public:
    using key_type = Key;
    using value_type = Value;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using link_type = node_type*;
    using iterator = __rb_tree_iterator<value_type, reference, pointer>;
    using const_iterator = __rb_tree_iterator<value_type, const_reference, const_pointer>;

public:
    rb_tree(const Compare& comp = Compare())
        : node_count_(0)
        , key_compare_(comp)
    {
        init();
    }

    // // copy constructor
    // rb_tree(const self& other)
    // {
    //     // TODO: verify
    //     node_count_ = other.node_count_;
    //     header_ = other.header_;
    //     key_compare_ = other.key_compare_;
    // }

    ~rb_tree()
    {
        clear();
        put_node(header_);
    }

    // self& operator=(const self& other)
    // {
    //     // TODO: verify
    //     node_count_ = other.node_count_;
    //     header_ = other.header_;
    //     key_compare_ = other.key_compare_;
    // }

public:
    iterator begin() const { return leftmost(); }

    iterator end() const
    {
        // TODO: header_ is a node, can be converted to iterator???
        return header_;
    }

    Compare key_comp() const { return key_compare_; }

    bool empty() const { return node_count_ == 0; }

    // TODO: count how many nodes with a key
    // size_type count(const key_type& k) const { }

    size_type size() const { return node_count_; }

    size_type max_size() const { return size_type(-1); }

    void clear() {}

    // allow duplicated node's keys
    iterator insert_equal(const_reference val)
    {
        link_type parent = header_;
        link_type node = root();
        while (node) {
            parent = node;
            // going left when key is smaller
            node = key_compare_(KeyOfValue()(val), key(node)) ? left(node) : right(node);
        }
        return __insert(node, parent, val);
    }

    // ensure unique node's key
    template<typename ReturnType = pair<iterator, bool>>
    ReturnType insert_unique(const_reference val)
    {
        // 1. search for insertion point
        link_type parent = header_;
        link_type node = root();
        bool comp = true;
        while (node) {
            parent = node;
            comp = key_compare_(KeyOfValue()(val), key(node));
            node = comp ? left(node) : right(node);
        }  // after leaving loop, parent is a leaf node, and node is a null node

        iterator prev_node_iter(parent);
        if (comp) {                           // node's key is smaller, insert at left
            if (prev_node_iter == begin()) {  // if insertion point's parent is the leftmost node
                // node refers to insertion point, parent refers to parent of node
                return ReturnType(__insert(node, parent, val), true);
            } else {
                --prev_node_iter;
            }
        }

        // check if previous node's key is strictly smaller than current node's key
        if (key_compare_(key(prev_node_iter.node), KeyOfValue()(val))) {
            return ReturnType(__insert(node, parent, val), true);
        }

        //  new key must be collide with existing key, do not insert this node
        return ReturnType(prev_node_iter, false);
    }

    iterator find(const key_type& k)
    {
        link_type y = header_;
        link_type x = root();

        while (x) {
            !key_compare_(key(x), k) ? y = x, x = left(x) : x = right(x);
        }
        iterator j(y);
        return (j == end()) || (key_compare_(k, key(j.node))) ? end() : j;
    }

protected:
    link_type& root() const { return (link_type&)(header_->parent); }

    link_type& leftmost() const { return (link_type&)(header_->left); }

    link_type& rightmost() const { return (link_type&)(header_->right); }

    link_type get_node() { return node_allocator::allocate(); }

    void put_node(link_type p) { node_allocator::deallocate(p); }

    /**
     * @brief Allocate and create a node with initial value.
     *
     * @param val Initial value assigned to node.
     * @return link_type Newly created node.
     */
    link_type create_node(const_reference val)
    {
        link_type node = get_node();
        try {
            mem::construct(&(node->value), val);
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            put_node(node);
        }
        return node;
    }

    /**
     * @brief Clone a node.
     *
     * @param node Node to be cloned.
     * @attention Only the node's value and color are cloned.
     * @attention New node's childs point to NULL.
     * @return link_type
     */
    link_type clone_node(link_type node)
    {
        link_type new_node = create_node(node->value);
        new_node->color = node->color;
        new_node->left = 0;
        new_node->right = 0;
        return new_node;
    }

    /**
     * @brief Destroy and deallocate a node.
     *
     * @param p Node pointer.
     */
    void destroy_node(link_type p)
    {
        mem::destroy(&(p->value));
        put_node(p);
    }

private:
    /**
     * @brief Insert a node.
     *
     * @param x Insertion point.
     * @param y Parent of insertion point x.
     * @param val Value of node to be inserted.
     * @return iterator Insertion position.
     */
    iterator __insert(base_link_type _x, base_link_type _y, const_reference val)
    {
        // note: x is often NULL when y is leaf node
        link_type x = (link_type)_x;
        link_type y = (link_type)_y;
        link_type z = create_node(val);

        if (y == header_ || x || key_compare_(KeyOfValue()(val), key(y))) {
            left(y) = z;  // when y is header, then equivalent to: leftmost() = z
            if (y == header_) {
                root() = z;
                rightmost() = z;
            } else if (y == leftmost()) {
                leftmost() = z;
            }
        } else {
            // insert as right child of parent
            right(y) = z;
            if (y == rightmost()) {
                rightmost() = z;
            }
        }

        // set parent, childs of newly created node
        // color is assigned in __rb_tree_rebalance() later
        parent(z) = y;
        left(z) = 0;
        right(z) = 0;

        // rebalance tree
        __rb_tree_rebalance(z, header_->parent);
        ++node_count_;
        return iterator(z);  // return an iterator pointing to newly created node
    }

    link_type __copy(link_type src, link_type des) {}

    void __erase(link_type node) {}

    // Init a tree, no nodes created.
    void init()
    {
        // header_ is a special node
        // notes: header and root will both be parent of each other
        header_ = get_node();
        // header is red, diff from root
        color(header_) = __rb_tree_red;
        root() = 0;
        // point header_'s child to itself: an impl techniques
        leftmost() = header_;   // same as: header_->left = header_
        rightmost() = header_;  // same as: header_->right = header_
    }

protected:
    size_type node_count_;
    link_type header_;
    Compare key_compare_;  // a functor for comparing keys

protected:
    static link_type& left(link_type node) { return (link_type&)(node->left); }

    static link_type& right(link_type node) { return (link_type&)(node->right); }

    static link_type& parent(link_type node) { return (link_type&)(node->parent); }

    static reference value(link_type node) { return node->value; }

    static const key_type& key(link_type node) { return KeyOfValue()(value(node)); }

    static color_type& color(link_type node) { return node->color; }

    static link_type& left(base_link_type node) { return (link_type&)(node->left); }

    static link_type& right(base_link_type node) { return (link_type&)(node->right); }

    static link_type& parent(base_link_type node) { return (link_type&)(node->parent); }

    static reference value(base_link_type node) { return ((link_type)node)->value; }

    static const key_type& key(base_link_type node) { return KeyOfValue()(value((link_type)node)); }

    static color_type& color(base_link_type node) { return ((link_type)node)->color; }

    static link_type minimum(link_type node) { return __rb_tree_node_base::minimum(node); }

    static link_type maximum(link_type node) { return __rb_tree_node_base::maximum(node); }
};

/**
 * @brief Rebalance rbtree to satisfy its properties, e.g. color, depth.
 *
 * @param node Node inserted.
 * @param root Root of the completed tree.
 *
 * @attention Any insertion operation should be followed by a rebalance.
 */
inline void __rb_tree_rebalance(__rb_tree_node_base* node, __rb_tree_node_base*& root)
{
    using base_link_type = __rb_tree_node_base*;
    node->color = __rb_tree_red;  // new node must be red

    // repeat until not consecutive red
    while (node != root && node->parent->color == __rb_tree_red) {
        // parent node is the left child of grandparent
        if (node->parent == node->parent->parent->left) {
            base_link_type y = node->parent->parent->right;  // set y be uncle
            if (y && y->color == __rb_tree_red) {            // have uncle, and uncle is red
                // Case 3: S is red
                node->parent->color = __rb_tree_black;
                y->color = __rb_tree_black;
                node->parent->parent->color = __rb_tree_red;
                node = node->parent->parent;
            } else {
                // no uncle, or uncle is black

                // Case 2: S is black & Inner insertion
                if (node == node->parent->right) {
                    // inner insertion requires an extra left rotation
                    node = node->parent;
                    __rb_tree_rotate_left(node, root);
                }

                // Case 1: S is black & Outer insertion
                node->parent->color = __rb_tree_black;
                node->parent->parent->color = __rb_tree_red;
                __rb_tree_rotate_right(node->parent->parent, root);
            }
        }

        // parent node is right child of grandparent
        // note: symmetic to above left side case
        else {
            base_link_type y = node->parent->parent->left;
            if (y && y->color == __rb_tree_red) {  // have uncle, and uncle is red
                node->parent->color = __rb_tree_black;
                y->color = __rb_tree_black;
                node->parent->parent->color = __rb_tree_red;
                node = node->parent->parent;
            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    __rb_tree_rotate_right(node, root);
                }

                node->parent->color = __rb_tree_black;
                node->parent->parent->color = __rb_tree_red;
                __rb_tree_rotate_left(node->parent->parent, root);
            }
        }
    }
    root->color = __rb_tree_black;  // root is always black
}

/**
 * @brief Left rotate a subtree about a pivot node.
 *
 * @param x Pivot of rotation.
 * @param root Root of the completed tree.
 */
inline void __rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root)
{
    // notes: procedure is highly similar to __rb_tree_rotate_right()

    // x is rotation pivot
    // y is right child of x
    __rb_tree_node_base* y = x->right;

    // interchange x and y childs:
    // hook y's left child to x's right child
    x->right = y->left;
    if (y->left) {  // y->left maybe NULL
        y->left->parent = x;
    }

    // rotation: y connects to x's parent
    y->parent = x->parent;

    // make y compleltely substitute x:
    // y receives all the relations of x to parents
    if (x == root) {
        root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    // finish rotation: reset connections between x and y
    // now x becomes y's left child
    y->left = x;
    x->parent = y;
}

/**
 * @brief Right rotate a subtree about a pivot node.
 *
 * @param x Pivot of rotation.
 * @param root Root of the completed tree.
 */
inline void __rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root)
{
    // notes: procedure is highly similar to __rb_tree_rotate_left()

    // x is rotation pivot
    // y is left child of x
    __rb_tree_node_base* y = x->left;

    // interchange x and y childs:
    // hook y's right child to x's left child
    x->left = y->right;
    if (y->right) {
        y->right->parent = x;
    }

    // rotation: y connects to x's parent
    y->parent = x->parent;

    // make y completely substitue x:
    // y receives all the relations to parents
    if (x == root) {
        root = y;
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }

    // finish rotation: reset connections between x and y
    // now x becomes y's right child
    y->right = x;
    x->parent = y;
}

}  // namespace mini::util

#endif
