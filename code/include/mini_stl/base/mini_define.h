#ifndef MINI_DEFINE_H
#define MINI_DEFINE_H

#include <stddef.h>
#include <algorithm>

namespace mini {

static const size_t __mini_stl_num_primes = 25;

static const unsigned long __mini_stl_prime_list[__mini_stl_num_primes] = {
    53, 97, 193, 389, 769,                                 //
    1543, 3079, 6151, 12289, 24593,                        //
    49157, 98317, 196613, 393241, 786433,                  //
    1572869, 3145739, 6291469, 12582917, 25165843,         //
    50331653, 100663319, 201326611, 402653189, 805306457,  //
};

/**
 * @brief Find the next prime from prime list which is nearest to n, and greater than n.
 *
 * @param n A prime number.
 * @return unsigned Nearest greater prime number to n.
 */
inline unsigned long __mini_stl_next_prime(unsigned long n)
{
    auto first = __mini_stl_prime_list;
    auto last = __mini_stl_prime_list + __mini_stl_num_primes;
    auto pos = std::lower_bound(first, last, n);
    return pos == last ? *(last - 1) : *pos;
}

}  // namespace mini

#endif
