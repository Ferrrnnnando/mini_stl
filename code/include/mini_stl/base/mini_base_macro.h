#ifndef MINI_MACRO_H
#define MINI_MACRO_H

/**
 * @brief Indicates the function/variable is not in use within the code
 *
 */
#define MINI_UNUSED(x) (void)(x)

#define MINI_NO_DISCARD [[nodiscard]]

/**
 * @brief Disallows a class to be copied and assigned
 *
 */
#define MINI_DISALLOW_COPY_AND_ASSIGN(TypeName) \
public:                                         \
    TypeName(const TypeName&) = delete;         \
    TypeName& operator=(const TypeName&) = delete

/**
 * @brief Disallows a class to be moved
 *
 */
#define MINI_DISALLOW_MOVE(TypeName) \
public:                              \
    TypeName(TypeName&&) = delete;   \
    TypeName& operator=(TypeName&&) = delete

/**
 * @brief Disallows a class to be copied and moved
 *
 */
#define MINI_DISALLOW_COPY_AND_MOVE(TypeName) \
    MINI_DISALLOW_MOVE(TypeName);             \
    MINI_DISALLOW_COPY_AND_ASSIGN(TypeName)

#endif  //MINI_MACRO_H
