#ifndef EELS_EXPECTED_TAGS_H_
#define EELS_EXPECTED_TAGS_H_

#include <eels/expected/config.h>

#if defined(EELS_EXPECTED_NO_CXX11_INLINE_NAMESPACES)
namespace eels { namespace expected_v1 {
#else
namespace eels { inline namespace expected_v1 {
#endif

struct in_place_t {};
static EELS_EXPECTED_CXX11_CONSTEXPR in_place_t in_place;

struct unexpected_t {};
static EELS_EXPECTED_CXX11_CONSTEXPR unexpected_t unexpected;

} }

#endif // EELS_EXPECTED_TAGS_H_
