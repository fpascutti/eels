#include <type_traits>
#include <gtest/gtest.h>
#include <eels/expected.h>

struct trivially_destructible
{ };
struct non_trivially_destructible
{ ~non_trivially_destructible() { } };

TEST(type_traits, is_trivially_destructible)
{
    EXPECT_TRUE((std::is_trivially_destructible<eels::expected<int, int>>::value)) << "An expected class made of trivially destructible types should be trivially destructible itself.";
    EXPECT_TRUE((std::is_trivially_destructible<eels::expected<int, trivially_destructible>>::value)) << "An expected class made of trivially destructible types should be trivially destructible itself.";
    EXPECT_TRUE((std::is_trivially_destructible<eels::expected<trivially_destructible, int>>::value)) << "An expected class made of trivially destructible types should be trivially destructible itself.";
    EXPECT_TRUE((std::is_trivially_destructible<eels::expected<trivially_destructible, trivially_destructible>>::value)) << "An expected class made of trivially destructible types should be trivially destructible itself.";

    EXPECT_FALSE((std::is_trivially_destructible<eels::expected<non_trivially_destructible, non_trivially_destructible>>::value)) << "An expected class made of non trivially destructible types should not be trivially destructible itself.";
    EXPECT_FALSE((std::is_trivially_destructible<eels::expected<non_trivially_destructible, int>>::value)) << "An expected class with a non trivially destructible value type should not be trivially destructible itself.";
    EXPECT_FALSE((std::is_trivially_destructible<eels::expected<int, non_trivially_destructible>>::value)) << "An expected class with a non trivially destructible error type should not be trivially destructible itself.";
}

TEST(type_traits, compilation_succeeds_for_void_value_type)
{
    eels::expected<void, int> e;
}

//TEST(type_traits, compilation_fails_for_void_error_type)
//{
//    eels::expected<int, void> e;
//}

//TEST(type_traits, compilation_fails_for_reference_error_type)
//{
//    eels::expected<int, int&> e0;
//    eels::expected<int, trivially_destructible&> e1;
//}