#ifndef EELS_EXPECTED_EXPECTED_H_
#define EELS_EXPECTED_EXPECTED_H_

#include <tuple>
#include <utility>
#include <eels/expected/config.h>
#include <eels/expected/tags.h>
#include <eels/expected/detail/storage.h>
#include <eels/expected/detail/tuple_indices.h>

#if defined(EELS_EXPECTED_NO_CXX11_INLINE_NAMESPACES)
namespace eels { namespace expected_v1 {
#else
namespace eels { inline namespace expected_v1 {
#endif

template<typename ValueT, typename ErrorT>
class expected
{
public:
	typedef ValueT value_type;
	typedef ErrorT error_type;

public:
	EELS_EXPECTED_CXX14_CONSTEXPR expected() EELS_EXPECTED_NOEXCEPT_IF((std::is_nothrow_default_constructible<error_type>::value))
        : storage_()
	{ }

	EELS_EXPECTED_CXX14_CONSTEXPR expected(const expected<value_type, error_type>& other) EELS_EXPECTED_NOEXCEPT_IF((std::is_nothrow_copy_constructible<value_type>::value && std::is_nothrow_copy_constructible<error_type>::value))
		: storage_(other.storage_)
	{ }

	EELS_EXPECTED_CXX14_CONSTEXPR expected(expected<value_type, error_type>&& other) EELS_EXPECTED_NOEXCEPT_IF((std::is_nothrow_move_constructible<value_type>::value && std::is_nothrow_move_constructible<error_type>::value))
		: storage_(std::move(other.storage_))
	{ }

	EELS_EXPECTED_CXX14_CONSTEXPR expected(const value_type& val) EELS_EXPECTED_NOEXCEPT_IF((std::is_nothrow_copy_constructible<value_type>::value))
		: storage_(val)
	{ }

	EELS_EXPECTED_CXX14_CONSTEXPR expected(value_type&& val) EELS_EXPECTED_NOEXCEPT_IF((std::is_nothrow_move_constructible<value_type>::value))
		: storage_(std::move(val))
	{ }

	template<typename... ArgsT>
	EELS_EXPECTED_CXX14_CONSTEXPR expected(in_place_t, ArgsT&&... args) EELS_EXPECTED_NOEXCEPT_IF((std::is_nothrow_constructible<value_type, ArgsT...>::value))
		: storage_(in_place, std::forward<ArgsT>(args)...)
	{ }

    template<typename... ArgsT>
    EELS_EXPECTED_CXX14_CONSTEXPR expected(std::tuple<in_place_t&, ArgsT...>&& factory) EELS_EXPECTED_NOEXCEPT_IF((std::is_nothrow_constructible<value_type, ArgsT...>::value))
        : storage_(std::move(factory), detail::make_tuple_indices(factory))
    { }
    
    template<typename... ArgsT>
	EELS_EXPECTED_CXX14_CONSTEXPR expected(unexpected_t, ArgsT&&... args) EELS_EXPECTED_NOEXCEPT_IF((std::is_nothrow_constructible<error_type, ArgsT...>::value))
		: storage_(unexpected, std::forward<ArgsT>(args)...)
	{ }

    template<typename... ArgsT>
    EELS_EXPECTED_CXX14_CONSTEXPR expected(std::tuple<unexpected_t&, ArgsT...> factory) EELS_EXPECTED_NOEXCEPT_IF((std::is_nothrow_constructible<value_type, ArgsT...>::value))
        : storage_(std::move(factory), detail::make_tuple_indices(factory))
    { }

    expected<value_type, error_type>& operator=(const expected<value_type, error_type>& other) EELS_EXPECTED_NOEXCEPT_IF((std::is_nothrow_copy_assignable<value_type>::value && std::is_nothrow_copy_constructible<value_type>::value && std::is_nothrow_destructible<value_type>::value && std::is_nothrow_copy_assignable<error_type>::value && std::is_nothrow_copy_constructible<error_type>::value && std::is_nothrow_destructible<error_type>::value))
	{
		storage_ = other.storage_;
		return *this;
	}

	expected<value_type, error_type>& operator=(expected<value_type, error_type>&& other) EELS_EXPECTED_NOEXCEPT_IF((std::is_nothrow_move_assignable<value_type>::value && std::is_nothrow_move_constructible<value_type>::value && std::is_nothrow_destructible<value_type>::value && std::is_nothrow_move_assignable<error_type>::value && std::is_nothrow_move_constructible<error_type>::value && std::is_nothrow_destructible<error_type>::value))
	{
		storage_ = std::move(other.storage_);
		return *this;
	}

    expected<value_type, error_type>& operator=(const value_type& val) EELS_EXPECTED_NOEXCEPT_IF((std::is_nothrow_copy_assignable<value_type>::value && std::is_nothrow_copy_constructible<value_type>::value && std::is_nothrow_destructible<error_type>::value))
    {
        storage_ = val;
        return *this;
    }

    expected<value_type, error_type>& operator=(value_type&& val) EELS_EXPECTED_NOEXCEPT_IF((std::is_nothrow_move_assignable<value_type>::value && std::is_nothrow_move_constructible<value_type>::value && std::is_nothrow_destructible<error_type>::value))
    {
        storage_ = std::move(val);
        return *this;
    }

    template<typename... ArgsT>
    expected<value_type, error_type>& operator=(std::tuple<in_place_t&, ArgsT...>&& factory) EELS_EXPECTED_NOEXCEPT_IF((std::is_nothrow_move_assignable<value_type>::value && std::is_nothrow_constructible<value_type, ArgsT...>::value && std::is_nothrow_destructible<error_type>::value))
    {
        storage_.assign(std::move(factory), detail::make_tuple_indices(factory));
        return *this;
    }

    template<typename... ArgsT>
    expected<value_type, error_type>& operator=(std::tuple<unexpected_t&, ArgsT...>&& factory) EELS_EXPECTED_NOEXCEPT_IF((std::is_nothrow_move_assignable<error_type>::value && std::is_nothrow_constructible<error_type, ArgsT...>::value && std::is_nothrow_destructible<value_type>::value))
    {
        storage_.assign(std::move(factory), detail::make_tuple_indices(factory));
        return *this;
    }

    EELS_EXPECTED_CXX11_CONSTEXPR operator bool() const { return storage_.valid(); }
    EELS_EXPECTED_CXX11_CONSTEXPR bool operator!() const { return !storage_.valid(); }

    EELS_EXPECTED_CXX11_CONSTEXPR value_type* operator->() { return &storage_.value(); }
    EELS_EXPECTED_CXX11_CONSTEXPR const value_type* operator->() const { return &storage_.value(); }

#if defined(EELS_EXPECTED_NO_CXX11_REF_QUALIFIERS)
    EELS_EXPECTED_CXX11_CONSTEXPR value_type& operator*() { return storage_.value(); }
    EELS_EXPECTED_CXX11_CONSTEXPR const value_type& operator*() const { return storage_.value(); }
#else
    EELS_EXPECTED_CXX11_CONSTEXPR value_type& operator*() & { return storage_.value(); }
    EELS_EXPECTED_CXX11_CONSTEXPR const value_type& operator*() const& { return storage_.value(); }
    EELS_EXPECTED_CXX11_CONSTEXPR value_type&& operator*() && { return std::move(storage_).value(); }
#endif // EELS_EXPECTED_REFQUALIFIERS

#if defined(EELS_EXPECTED_NO_CXX11_REF_QUALIFIERS)
    EELS_EXPECTED_CXX11_CONSTEXPR error_type& error() { return storage_.error(); }
    EELS_EXPECTED_CXX11_CONSTEXPR const error_type& error() const { return storage_.error(); }
#else
    EELS_EXPECTED_CXX11_CONSTEXPR error_type& error() & { return storage_.error(); }
    EELS_EXPECTED_CXX11_CONSTEXPR const error_type& error() const& { return storage_.error(); }
    EELS_EXPECTED_CXX11_CONSTEXPR error_type&& error() && { return std::move(storage_).error(); }
#endif // EELS_EXPECTED_REFQUALIFIERS

private:
	typename detail::storage_selector<value_type, error_type>::type storage_;
};

template<typename... ArgsT>
std::tuple<in_place_t&, ArgsT&&...> make_expected(ArgsT&&... args)
{
    return std::forward_as_tuple(in_place, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
std::tuple<unexpected_t&, ArgsT&&...> make_unexpected(ArgsT&&... args)
{    
    return std::forward_as_tuple(unexpected, std::forward<ArgsT>(args)...);
}

} }

#if defined(EELS_EXPECTED_NO_CXX11_INLINE_NAMESPACES)
namespace eels { using namespace expected_v1; }
#endif

#endif // EELS_EXPECTED_EXPECTED_H_