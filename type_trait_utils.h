#pragma once

#include <type_traits>

template<bool B>
using bool_constant_type = std::integral_constant<bool, B>;

#define DEF_HAS_MEMBER_VARIABLE(name, variable) \
template <class T> \
struct name \
{ \
private: \
    template <class U> \
    static auto test(int) -> decltype(((U*)nullptr)->variable ); \
    template <class U> \
    static void test(...); \
public: \
    static constexpr bool value = !std::is_same<void, decltype(test<T>(0))>::value; \
    using type = bool_constant_type<value>; \
}

#define DEF_HAS_METHOD_SIG(name, methodname) \
template <class T, class ReturnType = void, class... Args> \
struct name \
{ \
private: \
    struct sentry {}; \
    template <class U> \
    static auto test(int) -> decltype(((U*)nullptr)->methodname( *((Args*)nullptr)... )); \
    template <class U> \
    static sentry test(...); \
 \
    using ComputedReturnType = decltype(test<T>(0)); \
public: \
    static constexpr bool value = std::is_same<ReturnType, ComputedReturnType>::value; \
    using type = bool_constant_type<value>; \
}

template <bool B, bool... Values>
struct ConstexprAnd
{
    static constexpr bool value = B && ConstexprAnd<Values...>::value;
};

template <bool B>
struct ConstexprAnd<B>
{
    static constexpr bool value = B;
};


