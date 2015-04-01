#ifndef EELS_CONFIG_H_
#define EELS_CONFIG_H_

#if defined(_MSC_VER) // VISUAL STUDIO

#if _MSC_VER <= 1900 // VS2015

#define EELS_NO_CXX14_CONSTEXPR

#if _MSC_VER  <= 1800 // VS2013

#define EELS_NO_CXX11_INLINE_NAMESPACES
#define EELS_NO_CXX11_CONSTEXPR
#define EELS_NO_CXX11_NOEXCEPT
#define EELS_NO_CXX11_REF_QUALIFIERS

#endif // _MSC_VER <= 1800
#endif // _MSC_VER <= 1900

#endif // defined(_MSC_VER)

#if defined(EELS_NO_CXX11_CONSTEXPR)
#  define EELS_CXX11_CONSTEXPR
#  define EELS_CXX11_CONSTEXPR_OR_CONST const
#else
#  define EELS_CXX11_CONSTEXPR constexpr
#  define EELS_CXX11_CONSTEXPR_OR_CONST constexpr
#endif

#if defined(EELS_NO_CXX14_CONSTEXPR)
#  define EELS_CXX14_CONSTEXPR
#else
#  define EELS_CXX14_CONSTEXPR constexpr
#endif

#if defined(EELS_NO_CXX11_NOEXCEPT)
#  define EELS_NOEXCEPT_IF(...)
#else
#  define EELS_NOEXCEPT_IF(...) noexcept((__VA_ARGS__))
#endif

#endif // EELS_CONFIG_H_
