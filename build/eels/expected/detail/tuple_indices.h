#ifndef EELS_EXPECTED_DETAIL_TUPLE_INDICES_H_
#define EELS_EXPECTED_DETAIL_TUPLE_INDICES_H_

#include <tuple>

#if defined(EELS_NO_CXX11_INLINE_NAMESPACES)
namespace eels { namespace expected_v1 { namespace detail {
#else
namespace eels { inline namespace expected_v1 { namespace detail {
#endif

template<std::size_t... Indices>
class tuple_indices { };

template<std::size_t Index, std::size_t Count, typename TupleIndices>
class make_tuple_indices_impl;

template<std::size_t Index, std::size_t Count, std::size_t... Indices>
class make_tuple_indices_impl<Index, Count, tuple_indices<Indices...>>
{
public:
    typedef typename make_tuple_indices_impl<Index + 1, Count - 1, tuple_indices<Indices..., Index>>::type type;
};

template<std::size_t Index, std::size_t... Indices>
class make_tuple_indices_impl<Index, 0, tuple_indices<Indices...>>
{
public:
    typedef tuple_indices<Indices...> type;
};

template<typename... ValuesT>
typename make_tuple_indices_impl<0, std::tuple_size<std::tuple<ValuesT...>>::value, tuple_indices<>>::type make_tuple_indices(const std::tuple<ValuesT...>&)
{
    return typename make_tuple_indices_impl<0, std::tuple_size<std::tuple<ValuesT...>>::value, tuple_indices<>>::type{ };
}

} } }

#endif // EELS_EXPECTED_DETAIL_TUPLE_INDICES_H_
