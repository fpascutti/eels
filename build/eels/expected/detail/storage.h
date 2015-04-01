#ifndef EELS_EXPECTED_DETAIL_STORAGE_H_
#define EELS_EXPECTED_DETAIL_STORAGE_H_

#include <cassert>
#include <new>
#include <tuple>
#include <type_traits>
#include <utility>
#include <eels/expected/config.h>
#include <eels/expected/tags.h>
#include <eels/expected/detail/buffer.h>
#include <eels/expected/detail/tuple_indices.h>

#if defined(EELS_EXPECTED_NO_CXX11_INLINE_NAMESPACES)
namespace eels { namespace expected_v1 { namespace detail {
#else
namespace eels { inline namespace expected_v1 { namespace detail {
#endif

class empty_storage_access {};

template<typename StorageT, typename ValueT, typename TagT, typename NextT>
class storage_access
	: public NextT
{
private:
	typedef StorageT storage_type;
	typedef ValueT value_type;
	typedef TagT tag_type;

public:
	template<typename... ArgsT>
	EELS_EXPECTED_CXX14_CONSTEXPR void construct(ArgsT&&... args)
	{ ::new(storage()) value_type(std::forward<ArgsT>(args)...); }

	EELS_EXPECTED_CXX14_CONSTEXPR void construct(const value_type& val)
	{ ::new(storage()) value_type(val); }

	EELS_EXPECTED_CXX14_CONSTEXPR void construct(value_type&& val)
	{ ::new(storage()) value_type(std::move(val)); }

	EELS_EXPECTED_CXX14_CONSTEXPR void destruct()
	{ get().~value_type(); }

	EELS_EXPECTED_CXX14_CONSTEXPR void assign(const value_type& val)
	{ get() = val; }

	EELS_EXPECTED_CXX14_CONSTEXPR void assign(value_type&& val)
	{ get() = std::move(val); }

#if defined(EELS_EXPECTED_NO_CXX11_REF_QUALIFIERS)
    EELS_EXPECTED_CXX14_CONSTEXPR value_type& get() { return *reinterpret_cast<value_type*>(storage()); }
    EELS_EXPECTED_CXX14_CONSTEXPR const value_type& get() const { return *reinterpret_cast<const value_type*>(storage()); }
#else
    EELS_EXPECTED_CXX14_CONSTEXPR value_type& get() & { return *reinterpret_cast<value_type*>(storage()); }
    EELS_EXPECTED_CXX14_CONSTEXPR const value_type& get() const& { return *reinterpret_cast<const value_type*>(storage()); }
    EELS_EXPECTED_CXX14_CONSTEXPR value_type&& get() && { return *reinterpret_cast<value_type*>(storage()); }
#endif // EELS_EXPECTED_REFQUALIFIERS

private:
	void* storage() { return static_cast<typename storage_type::buffer_type*>(static_cast<storage_type*>(this))->template get<tag_type>(); }
	const void* storage() const { return static_cast<const typename storage_type::buffer_type*>(static_cast<const storage_type*>(this))->template get<tag_type>(); }
};

template<typename ValueT, typename ErrorT>
class storage_base
	: public buffer_selector<ValueT, ErrorT>::type,
	  public storage_access<storage_base<ValueT, ErrorT>, ErrorT, error_from_buffer_t,
			 storage_access<storage_base<ValueT, ErrorT>, ValueT, value_from_buffer_t, empty_storage_access>
	  >
{
public:
	typedef ValueT value_type;
	typedef ErrorT error_type;
	typedef typename buffer_selector<value_type, error_type>::type buffer_type;

protected:
	typedef storage_access<storage_base<ValueT, ErrorT>, ValueT, value_from_buffer_t, empty_storage_access> value_access_type;
	typedef storage_access<storage_base<ValueT, ErrorT>, ErrorT, error_from_buffer_t, value_access_type> error_access_type;

public:
	// default, copy and move constructors
	EELS_EXPECTED_CXX14_CONSTEXPR storage_base()
		: is_valid_(false)
	{
		error_access_type::construct();
	}

	EELS_EXPECTED_CXX14_CONSTEXPR storage_base(const storage_base<value_type, error_type>& other)
		: is_valid_(other.valid())
	{
		if(valid())
			value_access_type::construct(other.value());
		else
			error_access_type::construct(other.error());
	}

	EELS_EXPECTED_CXX14_CONSTEXPR storage_base(storage_base<value_type, error_type>&& other)
		: is_valid_(other.valid())
	{
		if(valid())
			value_access_type::construct(std::move(other.value()));
		else
			error_access_type::construct(std::move(other.error()));
	}

	// value constructors
	EELS_EXPECTED_CXX14_CONSTEXPR storage_base(const value_type& val)
		: is_valid_(true)
	{
		value_access_type::construct(val);
	}
	EELS_EXPECTED_CXX14_CONSTEXPR storage_base(value_type&& val)
		: is_valid_(true)
	{
		value_access_type::construct(std::move(val));
	}

	// in_place constructors
	template<typename... ArgsT>
	EELS_EXPECTED_CXX14_CONSTEXPR storage_base(in_place_t, ArgsT&&... args)
		: is_valid_(true)
	{
		value_access_type::construct(std::forward<ArgsT>(args)...);
	}

	// unexpected constructors
	template<typename... ArgsT>
	EELS_EXPECTED_CXX14_CONSTEXPR storage_base(unexpected_t, ArgsT&&... args)
		: is_valid_(false)
	{
		error_access_type::construct(std::forward<ArgsT>(args)...);
	}

	// copy and move assignment operators
	storage_base<value_type, error_type>& operator=(const storage_base<value_type, error_type>& other)
	{
		if(other.valid())
		{
			const auto& value = static_cast<const value_access_type&>(other).get();
			if(valid())
				value_access_type::assign(value);
			else
				buffer_type::switch_fromto(static_cast<error_access_type&>(*this), static_cast<value_access_type&>(*this), value);
		}
		else
		{
			const auto& err = static_cast<const error_access_type&>(other).get();
			if(valid())
				buffer_type::switch_fromto(static_cast<value_access_type&>(*this), static_cast<error_access_type&>(*this), err);
			else
				error_access_type::assign(err);				
		}
		is_valid_ = other.valid();
		return *this;
	}

	storage_base<value_type, error_type>& operator=(storage_base<value_type, error_type>&& other)
	{
		if (other.valid())
		{
			auto value = std::move(static_cast<value_access_type&&>(other).get());
			if (valid())
				value_access_type::assign(std::move(value));
			else
				buffer_type::switch_fromto(static_cast<error_access_type&>(*this), static_cast<value_access_type&>(*this), std::move(value));
		}
		else
		{
			auto err = std::move(static_cast<error_access_type&&>(other).get());
			if (valid())
				buffer_type::switch_fromto(static_cast<value_access_type&>(*this), static_cast<error_access_type&>(*this), std::move(err));
			else
				error_access_type::assign(std::move(err));
		}
		is_valid_ = other.valid();
		return *this;
	}

    // value assignment operators
    storage_base<value_type, error_type>& operator=(const value_type& val)
    {
        if(valid())
            value_access_type::assign(val);
        else
            buffer_type::switch_fromto(static_cast<error_access_type&>(*this), static_cast<value_access_type&>(*this), value);
        is_valid_ = true;
        return *this;
    }

    storage_base<value_type, error_type>& operator=(value_type&& val)
    {
        if(valid())
            value_access_type::assign(std::move(val));
        else
            buffer_type::switch_fromto(static_cast<error_access_type&>(*this), static_cast<value_access_type&>(*this), std::move(val));
        is_valid_ = true;
        return *this;
    }

    // in_place assignment
    template<typename... ArgsT>
    storage_base<value_type, error_type>& assign(in_place_t, ArgsT&&... args)
    {
        if(valid())
            value_access_type::assign(value_type(std::forward<ArgsT>(args)...));            
        else
            buffer_type::switch_fromto(static_cast<error_access_type&>(*this), static_cast<value_access_type&>(*this), std::forward<ArgsT>(args)...);
        is_valid_ = true;
        return *this;
    }

    // unexpected assignment
    template<typename... ArgsT>
    storage_base<value_type, error_type>& assign(unexpected_t, ArgsT&&... args)
    {
        if(valid())
            buffer_type::switch_fromto(static_cast<value_access_type&>(*this), static_cast<error_access_type&>(*this), std::forward<ArgsT>(args)...);
        else
            error_access_type::assign(error_type(std::forward<ArgsT>(args)...));
        is_valid_ = false;
        return *this;
    }
    
    // observers
	EELS_EXPECTED_CXX14_CONSTEXPR bool valid() const { return is_valid_; }

#if defined(EELS_EXPECTED_NO_CXX11_REF_QUALIFIERS)
    EELS_EXPECTED_CXX14_CONSTEXPR value_type& value() { assert(valid());  return value_access_type::get(); }
    EELS_EXPECTED_CXX14_CONSTEXPR const value_type& value() const { assert(valid());  return value_access_type::get(); }
#else
    EELS_EXPECTED_CXX14_CONSTEXPR value_type& value() & { assert(valid());  return value_access_type::get(); }
    EELS_EXPECTED_CXX14_CONSTEXPR const value_type& value() const& { assert(valid());  return value_access_type::get(); }
    EELS_EXPECTED_CXX14_CONSTEXPR value_type&& value() && { assert(valid());  return value_access_type::get(); }
#endif // EELS_EXPECTED_REFQUALIFIERS

#if defined(EELS_EXPECTED_NO_CXX11_REF_QUALIFIERS)
    EELS_EXPECTED_CXX14_CONSTEXPR error_type& error() { assert(!valid());  return error_access_type::get(); }
    EELS_EXPECTED_CXX14_CONSTEXPR const error_type& error() const { assert(!valid());  return error_access_type::get(); }
#else
    EELS_EXPECTED_CXX14_CONSTEXPR error_type& error() & { assert(!valid());  return error_access_type::get(); }
    EELS_EXPECTED_CXX14_CONSTEXPR const error_type& error() const& { assert(!valid());  return error_access_type::get(); }
    EELS_EXPECTED_CXX14_CONSTEXPR error_type&& error() && { assert(!valid());  return error_access_type::get(); }
#endif // EELS_EXPECTED_REFQUALIFIERS

private:
	bool is_valid_;
};

template<typename ValueT, typename ErrorT>
class trivial_storage
	: public storage_base<ValueT, ErrorT>
{
private:
    typedef storage_base<ValueT, ErrorT> base_type;

public:
    EELS_EXPECTED_CXX14_CONSTEXPR trivial_storage()
        : base_type()
    { }
    EELS_EXPECTED_CXX14_CONSTEXPR trivial_storage(const trivial_storage<value_type, error_type>& other)
        : base_type(other)
    { }
    EELS_EXPECTED_CXX14_CONSTEXPR trivial_storage(trivial_storage<value_type, error_type>&& other)
        : base_type(std::move(other))
    { }
    EELS_EXPECTED_CXX14_CONSTEXPR trivial_storage(const value_type& val)
        : base_type(val)
    { }
    EELS_EXPECTED_CXX14_CONSTEXPR trivial_storage(value_type&& val)
        : base_type(std::move(val))
    { }
    template<typename... ArgsT>
    EELS_EXPECTED_CXX14_CONSTEXPR trivial_storage(in_place_t, ArgsT&&... args)
        : base_type(in_place, std::forward<ArgsT>(args)...)
    { }
    template<typename... ArgsT>
    EELS_EXPECTED_CXX14_CONSTEXPR trivial_storage(unexpected_t, ArgsT&&... args)
        : base_type(unexpected, std::forward<ArgsT>(args)...)
    { }
    template<typename... ArgsT, std::size_t... Indices>
    EELS_EXPECTED_CXX14_CONSTEXPR trivial_storage(std::tuple<ArgsT...>&& factory, const tuple_indices<Indices...>&)
        : base_type(std::forward<ArgsT>(std::get<Indices>(factory))...)
    { }

    trivial_storage<value_type, error_type>& operator=(const trivial_storage<value_type, error_type>& other)
    {
        base_type::operator=(other);
        return *this;
    }

    trivial_storage<value_type, error_type>& operator=(trivial_storage<value_type, error_type>&& other)
    {
        base_type::operator=(std::move(other));
        return *this;
    }

    trivial_storage<value_type, error_type>& operator=(const value_type& val)
    {
        base_type::operator=(val);
        return *this;
    }

    trivial_storage<value_type, error_type>& operator=(value_type&& val)
    {
        base_type::operator=(std::move(val));
        return *this;
    }

    template<typename... ArgsT, std::size_t... Indices>
    trivial_storage<value_type, error_type>& assign(std::tuple<ArgsT...>&& factory, const tuple_indices<Indices...>&)
    {
        base_type::assign(std::forward<ArgsT>(std::get<Indices>(factory))...);
        return *this;
    }
};

template<typename ValueT, typename ErrorT>
class nontrivial_storage
	: public storage_base<ValueT, ErrorT>
{
private:
    typedef storage_base<ValueT, ErrorT> base_type;

public:
    EELS_EXPECTED_CXX14_CONSTEXPR nontrivial_storage()
        : base_type()
    { }
    EELS_EXPECTED_CXX14_CONSTEXPR nontrivial_storage(const nontrivial_storage<value_type, error_type>& other)
        : base_type(other)
    { }
    EELS_EXPECTED_CXX14_CONSTEXPR nontrivial_storage(nontrivial_storage<value_type, error_type>&& other)
        : base_type(std::move(other))
    { }
    EELS_EXPECTED_CXX14_CONSTEXPR nontrivial_storage(const value_type& val)
        : base_type(val)
    { }
    EELS_EXPECTED_CXX14_CONSTEXPR nontrivial_storage(value_type&& val)
        : base_type(std::move(val))
    { }
    template<typename... ArgsT>
    EELS_EXPECTED_CXX14_CONSTEXPR nontrivial_storage(in_place_t, ArgsT&&... args)
        : base_type(in_place, std::forward<ArgsT>(args)...)
    { }
    template<typename... ArgsT>
    EELS_EXPECTED_CXX14_CONSTEXPR nontrivial_storage(unexpected_t, ArgsT&&... args)
        : base_type(unexpected, std::forward<ArgsT>(args)...)
    { }
    template<typename... ArgsT, std::size_t... Indices>
    EELS_EXPECTED_CXX14_CONSTEXPR nontrivial_storage(std::tuple<ArgsT...>&& factory, const tuple_indices<Indices...>& )
        : base_type(std::forward<ArgsT>(std::get<Indices>(factory))...)
    { }

	~nontrivial_storage()
	{
		if(valid())
			value_access_type::destruct();
		else
			error_access_type::destruct();
	}

    nontrivial_storage<value_type, error_type>& operator=(const nontrivial_storage<value_type, error_type>& other)
    {
        base_type::operator=(other);
        return *this;
    }

    nontrivial_storage<value_type, error_type>& operator=(nontrivial_storage<value_type, error_type>&& other)
    {
        base_type::operator=(std::move(other));
        return *this;
    }

    nontrivial_storage<value_type, error_type>& operator=(const value_type& val)
    {
        base_type::operator=(val);
        return *this;
    }

    nontrivial_storage<value_type, error_type>& operator=(value_type&& val)
    {
        base_type::operator=(std::move(val));
        return *this;
    }

    template<typename... ArgsT, std::size_t... Indices>
    nontrivial_storage<value_type, error_type>& assign(std::tuple<ArgsT...>&& factory, const tuple_indices<Indices...>&)
    {
        base_type::assign(std::forward<ArgsT>(std::get<Indices>(factory))...);
        return *this;
    }
};

template<typename ValueT, typename ErrorT>
class storage_selector
{
public:
	typedef ValueT value_type;
	typedef ErrorT error_type;
	typedef typename std::conditional<
							std::is_trivially_destructible<value_type>::value && std::is_trivially_destructible<error_type>::value,
						trivial_storage<value_type, error_type>,
						nontrivial_storage<value_type, error_type>
	>::type type;
};

} } }

#endif // EELS_EXPECTED_DETAIL_STORAGE_H_