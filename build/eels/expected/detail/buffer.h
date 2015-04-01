#ifndef EELS_EXPECTED_DETAIL_BUFFER_H_
#define EELS_EXPECTED_DETAIL_BUFFER_H_

#include <type_traits>
#include <utility>

#if defined(EELS_EXPECTED_NO_CXX11_INLINE_NAMESPACES)
namespace eels { namespace expected_v1 { namespace detail {
#else
namespace eels { inline namespace expected_v1 { namespace detail {
#endif

class value_from_buffer_t {};
class error_from_buffer_t {};

template<typename ValueT, typename ErrorT>
class merged_buffer
{
public:
	typedef ValueT value_type;
	typedef ErrorT error_type;

	template<typename T> void* get();
	template<typename T> const void* get() const;

	template<> void* get<value_from_buffer_t>() { return &data_; }
	template<> const void* get<value_from_buffer_t>() const { return &data_; }

	template<> void* get<error_from_buffer_t>() { return &data_; }
	template<> const void* get<error_from_buffer_t>() const { return &data_; }

	template<typename FromT, typename ToT>
	void switch_fromto(FromT& from, ToT& to, const typename ToT::value_type& val)
	{ from.destruct(); to.construct(val); }
	template<typename FromT, typename ToT>
	void switch_fromto(FromT& from, ToT& to, typename ToT::value_type&& val)
	{ from.destruct(); to.construct(std::move(val)); }
    template<typename FromT, typename ToT, typename... ArgsT>
    void switch_fromto(FromT& from, ToT& to, ArgsT&&... args)
    { from.destruct(); to.construct(std::forward<ArgsT>(args)...); }
private:
    static EELS_EXPECTED_CXX11_CONSTEXPR_OR_CONST std::size_t size = sizeof(value_type) > sizeof(error_type) ? sizeof(value_type) : sizeof(error_type);
	typename std::aligned_union<size, value_type, error_type >::type data_;
};

template<typename ValueT, typename ErrorT>
class independent_buffer
{
public:
	typedef ValueT value_type;
	typedef ErrorT error_type;

	template<typename T> void* get();
	template<typename T> const void* get() const;

	template<> void* get<value_from_buffer_t>() { return &value_data_; }
	template<> const void* get<value_from_buffer_t>() const { return &value_data_; }

	template<> void* get<error_from_buffer_t>() { return &error_data_; }
	template<> const void* get<error_from_buffer_t>() const { return &error_data_; }

	template<typename FromT, typename ToT>
	void switch_fromto(FromT& from, ToT& to, const typename ToT::value_type& val)
	{ to.construct(val); from.destruct(); }
	template<typename FromT, typename ToT>
	void switch_fromto(FromT& from, ToT& to, typename ToT::value_type&& val)
	{ to.construct(std::move(val)); from.destruct(); }
    template<typename FromT, typename ToT, typename... ArgsT>
    void switch_fromto(FromT& from, ToT& to, ArgsT&&... args)
    { to.construct(std::forward<ArgsT>(args)...); from.destruct(); }

private:
	typename std::aligned_storage<sizeof(value_type), std::alignment_of<value_type>::value>::type value_data_;
	typename std::aligned_storage<sizeof(error_type), std::alignment_of<error_type>::value>::type error_data_;
};

template<typename ValueT, typename ErrorT>
class buffer_selector
{
public:
	typedef ValueT value_type;
	typedef ErrorT error_type;
	typedef typename std::conditional<
						std::is_nothrow_copy_constructible<value_type>::value && std::is_nothrow_move_constructible<value_type>::value &&
						std::is_nothrow_copy_constructible<error_type>::value && std::is_nothrow_move_constructible<error_type>::value,
					merged_buffer<value_type, error_type>,
					independent_buffer<value_type, error_type>
	>::type type;
};

} } }

#endif // EELS_EXPECTED_DETAIL_BUFFER_H_
