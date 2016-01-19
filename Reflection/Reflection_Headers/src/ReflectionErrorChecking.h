#pragma once

#include <type_traits>

// None of the functions or macros in this class are to be used outside of the Reflection
// package.

template<> constexpr bool Are_Classes_Reflected()
{
	return true;
}

template<typename Arg1, typename... Args>
constexpr bool Are_Classes_Reflected()
{
	return std::is_member_function_pointer<decltype( &Arg1::GetClassType )>::value &&
	       Are_Classes_Reflected<Args...>();
}

#define IS_THIS_CLASS_REFLECTED std::is_member_function_pointer<decltype( &GetClassType )>::value


#define CLASS_NOT_REFLECTED_ERROR_MESSAGE "Cannot have a reflected variable in a non-reflected class."
#define PARENT_CLASS_NOT_REFLECTED_ERROR_MESSAGE "One or more of the declared parent classes is not reflected."

#define CHECK_REFLECTED_THIS_CLASS static_assert( IS_THIS_CLASS_REFLECTED, CLASS_NOT_REFLECTED_ERROR_MESSAGE );
#define CHECK_REFLECTED_PARENT_CLASSES( ... ) static_assert( Are_Classes_Reflected<__VA_ARGS__>(), PARENT_CLASS_NOT_REFLECTED_ERROR_MESSAGE );
