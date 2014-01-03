#if !defined(SAFE_OPERATIONS_H)
#define SAFE_OPERATIONS_H

/*
Declares and defines common arithmetic operations with overflow checking.
Features are designed as non-member function templates in a namespace.
If overflow happens, the functions throw an std::overflow_error.

Employs the built-in safeint.h for Visual C++.
Employs the downloadable SafeInt.hpp from http://safeint.codeplex.com/ for other compilers.
*/

#include <stdexcept>
#include <sstream>
#include <type_traits>
#include <typeinfo>

#if defined(_MSC_VER)
// Use the built-in safeint.h for Visual C++.
#include <safeint.h>
//using namespace msl::utilities;
#else
// Use the downloadable SafeInt.hpp from http://safeint.codeplex.com/ for other compilers.
#include "SafeInt.hpp"
#endif

#if defined(__GNUG__)
// std::enable_if_t<bool, T> is not defined for g++ as it is a C++14 feature.
namespace std
{
	template <bool B, typename T = void>
	using enable_if_t = typename enable_if<B, T>::type;
}
#endif

namespace Imaging
{
	/* How to constraint return value data type.
	C++ cannot overload different functions for different return data types.

	If return data type is important, design functions in a way that result values are
	always returned as a function argument instead of a return value.
	'void Func(T t, U u, T &result)' instead of 'T result = Func(T t, U u)'. */

	/* Operations between integral type (regardless of same or different data types).
	The functions of safe int library (either the built-in safeint.h for Visual C++ or
	the SafeInt.hpp from http://safeint.codeplex.com) can detect integer overflow.

	Always use the functions in the safe int libraries.
	Due to its syntax, the function argument must be always (T, U, T &) where the last T& is
	the result. */

	/* How to design function templates for operations with two data types {T, U}.

	In order to avoid template parameter ambiguity situation, define three scenarios for
	binary operations.
	1. Same data types only
	template <typename T> void Func(T, T, T &)
	2. Different data types only and the return value is the same as the first one.
	template <typename T, typename U>
	std::enable_if_t<!std::is_same<T, U>::value, void> Func(T, U, T &)
	3. Different data types only and the return value is the same as the second one.
	template <typename T, typename U>
	std::enable_if_t<!std::is_same<T, U>::value, void> Func(T, U, U &)

	Scenario 1 and 2 can be merged as following.
	template <typename T, typename U> void Func(T, U, T &)

	NOTE: Order-dependent operations such as subtraction or division cannot use the scenario
	3 because the employed safe int library requires function arguments to be always
	(T, U, T &). */

	/* Operations between floating point type.
	As long as the result is the same or wider than the two source data, data precision loss
	does not happen.

	1. Enable the function only if the result data type is the same or wider than source data.
	2. Disable the function for other scenarios.
	Ignore overflow risk. */

	/* Operations between integral type and floating point type.
	Operations between integral type and floating point type is okay only if the result
	value is floating point and it is wider (not the same) than the integral source value.

	1. Enable the function only if the result value is floating and wider than the integral
	source value.
	2. Disble the function for other scenarios. */

	/* std::enable_if_t<T> and function overload.
	Implementing all of the scheme above takes a lot of function overload implementation for
	a simple operation.

	Use std::enable_if_t<T> for the return variable to reduce the number of required
	function arguments. */

	////////////////////////////////////////////////////////////////////////////////////////
	// Add

	////////////////////////////////////////////////////////////////////////////////////////
	// High level functions for end-users.
	// NOTE: The result data type could be T or U for these high-level function templates.

	// 1. Same data type only. T = T + T (T + T -> T)
	// 2. Different data type scenario A. T = T + U (T + U -> T)
	template <typename T, typename U>
	void Add(T t, U u, T &result)
	{
		static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<U>::value,
			"Only arithmetic data types are supported for this function template.");
		Add_imp(t, u, result, std::is_integral<T>(), std::is_integral<U>());
	}

	// 3. Different data type scenario B. U = T + U (T + U -> U)
	template <typename T, typename U>
	std::enable_if_t<!std::is_same<T, U>::value, void> Add(T t, U u, U &result)
	{
		static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<U>::value,
			"Only arithmetic data types are supported for this function template.");
		Add_imp(u, t, result, std::is_integral<U>(), std::is_integral<T>());
	}

	// High level functions for end-users.
	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////
	// Low level functions for internal only.
	// NOTE: The result data type is always T for these low-level function templates.

	// integral = integral + integral; checking overflow.
	template <typename T, typename U>
	void Add_imp(T t, U u, T &result, std::true_type, std::true_type)
	{
		if (!msl::utilities::SafeAdd(t, u, result))
		{
			std::ostringstream errMsg;
			errMsg << "The result value exceeds the limit of " << typeid(result).name();
			throw std::overflow_error(errMsg.str());
		}
	}

	// integral = integral + floating; Disabled becasue of data loss risk.
	template <typename T, typename U>
	void Add_imp(T t, U u, T &result, std::true_type, std::false_type)
	{
		static_assert(!std::is_integral<T>::value || std::is_integral<U>::value,
			"The result data type must be floating point type if any source value is "
			"floating point type.");
		throw std::logic_error("This function must not be called as it is disabled.");
	}

	// floating = floating + integral. sizeof(result) > sizeof(src)
	template <typename T, typename U>
	std::enable_if_t<(sizeof(T) > sizeof(U)), void> Add_imp(T t, U u, T &result,
		std::false_type, std::true_type)
	{
		result = t + u;
	}

	// floating = floating + integral. sizeof(result) <= sizeof(src)
	// Disabled becasue of data loss risk.
	template <typename T, typename U>
	std::enable_if_t<sizeof(T) <= sizeof(U), void> Add_imp(T t, U u, T &result,
		std::false_type, std::true_type)
	{
		static_assert(sizeof(T) > sizeof(U),
			"The result data type must be wider than source data types.");
		throw std::logic_error("This function must not be called as it is disabled.");
	}

	// floating = floating + floating. sizeof(result) >= sizeof(src)
	template <typename T, typename U>
	std::enable_if_t<sizeof(T) >= sizeof(U), void> Add_imp(T t, U u, T &result,
		std::false_type, std::false_type)
	{
		result = t + u;
	}

	// floating = floating + floating. sizeof(result) < sizeof(src)
	// Disabled becasue of data loss risk.
	template <typename T, typename U>
	std::enable_if_t<sizeof(T) < sizeof(U), void> Add_imp(T t, U u, T &result,
		std::false_type, std::false_type)
	{
		static_assert(sizeof(T) >= sizeof(U),
			"The result data type must be the same or wider than source data types.");
		throw std::logic_error("This function must not be called as it is disabled.");
	}

	// Low level functions for internal only.
	////////////////////////////////////////////////////////////////////////////////////////

	// Add
	////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////
	// Multiply

	////////////////////////////////////////////////////////////////////////////////////////
	// High level functions for end-users.
	// NOTE: The result data type could be T or U for these high-level function templates.

	// 1. Same data type only. T = T * T (T * T -> T)
	// 2. Different data type scenario A. T = T * U (T * U -> T)
	template <typename T, typename U>
	void Multiply(T t, U u, T &result)
	{
		static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<U>::value,
			"Only arithmetic data types are supported for this function template.");
		Multiply_imp(t, u, result, std::is_integral<T>(), std::is_integral<U>());
	}

	// 3. Different data type scenario B. U = T * U (T * U -> U)
	template <typename T, typename U>
	std::enable_if_t<!std::is_same<T, U>::value, void> Multiply(T t, U u, U &result)
	{
		static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<U>::value,
			"Only arithmetic data types are supported for this function template.");
		Multiply_imp(u, t, result, std::is_integral<U>(), std::is_integral<T>());
	}

	// High level functions for end-users.
	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////
	// Low level functions for internal only.
	// NOTE: The result data type is always T for these low-level function templates.

	// integral = integral * integral; checking overflow.
	template <typename T, typename U>
	void Multiply_imp(T t, U u, T &result, std::true_type, std::true_type)
	{
		if (!msl::utilities::SafeMultiply(t, u, result))
		{
			std::ostringstream errMsg;
			errMsg << "The result value exceeds the limit of " << typeid(result).name();
			throw std::overflow_error(errMsg.str());
		}
	}

	// integral = integral * floating; Disabled becasue of data loss risk.
	template <typename T, typename U>
	void Multiply_imp(T t, U u, T &result, std::true_type, std::false_type)
	{
		static_assert(!std::is_integral<T>::value || std::is_integral<U>::value,
			"The result data type must be floating point type if any source value is "
			"floating point type.");
		throw std::logic_error("This function must not be called as it is disabled.");
	}

	// floating = floating * integral. sizeof(result) > sizeof(src)
	template <typename T, typename U>
	std::enable_if_t<(sizeof(T) > sizeof(U)), void> Multiply_imp(T t, U u, T &result,
		std::false_type, std::true_type)
	{
		result = t * u;
	}

	// floating = floating * integral. sizeof(result) <= sizeof(src)
	// Disabled becasue of data loss risk.
	template <typename T, typename U>
	std::enable_if_t<sizeof(T) <= sizeof(U), void> Multiply_imp(T t, U u, T &result,
		std::false_type, std::true_type)
	{
		static_assert(sizeof(T) > sizeof(U),
			"The result data type must be wider than source data types.");
		throw std::logic_error("This function must not be called as it is disabled.");
	}

	// floating = floating * floating. sizeof(result) >= sizeof(src)
	template <typename T, typename U>
	std::enable_if_t<sizeof(T) >= sizeof(U), void> Multiply_imp(T t, U u, T &result,
		std::false_type, std::false_type)
	{
		result = t * u;
	}

	// floating = floating * floating. sizeof(result) < sizeof(src)
	// Disabled becasue of data loss risk.
	template <typename T, typename U>
	std::enable_if_t<sizeof(T) < sizeof(U), void> Multiply_imp(T t, U u, T &result,
		std::false_type, std::false_type)
	{
		static_assert(sizeof(T) >= sizeof(U),
			"The result data type must be the same or wider than source data types.");
		throw std::logic_error("This function must not be called as it is disabled.");
	}

	// Low level functions for internal only.
	////////////////////////////////////////////////////////////////////////////////////////

	// Multiply
	////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////
	// Subtract

	////////////////////////////////////////////////////////////////////////////////////////
	// High level functions for end-users.
	// NOTE: The result data type is always T for subtraction or division.

	// 1. Same data type only. T = T - T (T - T -> T)
	// 2. Different data type scenario A. T = T - U (T - U -> T)
	template <typename T, typename U>
	void Subtract(T t, U u, T &result)
	{
		static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<U>::value,
			"Only arithmetic data types are supported for this function template.");
		Subtract_imp(t, u, result, std::is_integral<T>(), std::is_integral<U>());
	}

	// 3. Different data type scenario B. U = T - U (T - U -> U)
	template <typename T, typename U>
	std::enable_if_t<!std::is_same<T, U>::value, void> Subtract(T t, U u, U &result)
	{
		static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<U>::value,
			"Only arithmetic data types are supported for this function template.");
		static_assert(std::is_same<T, U>::value,
			"The first source value must be the same type as the result value.");
		throw std::logic_error("This function must not be called as it is disabled.");
	}

	// High level functions for end-users.
	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////
	// Low level functions for internal only.

	// integral = integral - integral; checking overflow.
	template <typename T, typename U>
	void Subtract_imp(T t, U u, T &result, std::true_type, std::true_type)
	{
		if (!msl::utilities::SafeSubtract(t, u, result))
		{
			std::ostringstream errMsg;
			errMsg << "The result value exceeds the limit of " << typeid(result).name();
			throw std::overflow_error(errMsg.str());
		}
	}

	// integral = integral - floating; Disabled becasue of data loss risk.
	template <typename T, typename U>
	void Subtract_imp(T t, U u, T &result, std::true_type, std::false_type)
	{
		static_assert(!std::is_integral<T>::value || std::is_integral<U>::value,
			"The result data type must be floating point type if any source value is "
			"floating point type.");
		throw std::logic_error("This function must not be called as it is disabled.");
	}

	// floating = floating - integral. sizeof(result) > sizeof(src)
	template <typename T, typename U>
	std::enable_if_t<(sizeof(T) > sizeof(U)), void> Subtract_imp(T t, U u, T &result,
		std::false_type, std::true_type)
	{
		result = t - u;
	}

	// floating = floating - integral. sizeof(result) <= sizeof(src)
	// Disabled becasue of data loss risk.
	template <typename T, typename U>
	std::enable_if_t<sizeof(T) <= sizeof(U), void> Subtract_imp(T t, U u, T &result,
		std::false_type, std::true_type)
	{
		static_assert(sizeof(T) > sizeof(U),
			"The result data type must be wider than source data types.");
		throw std::logic_error("This function must not be called as it is disabled.");
	}

	// floating = floating - floating. sizeof(result) >= sizeof(src)
	template <typename T, typename U>
	std::enable_if_t<sizeof(T) >= sizeof(U), void> Subtract_imp(T t, U u, T &result,
		std::false_type, std::false_type)
	{
		result = t - u;
	}

	// floating = floating - floating. sizeof(result) < sizeof(src)
	// Disabled becasue of data loss risk.
	template <typename T, typename U>
	std::enable_if_t<sizeof(T) < sizeof(U), void> Subtract_imp(T t, U u, T &result,
		std::false_type, std::false_type)
	{
		static_assert(sizeof(T) >= sizeof(U),
			"The result data type must be the same or wider than source data types.");
		throw std::logic_error("This function must not be called as it is disabled.");
	}

	// Low level functions for internal only.
	////////////////////////////////////////////////////////////////////////////////////////

	// Subtract
	////////////////////////////////////////////////////////////////////////////////////////


	template <typename T>
	void Increment(T &value)
	{
		static_assert(std::is_arithmetic<T>::value,
			"Only arithmetic data types are supported for this function template.");
		Increment_imp(value, std::is_integral<T>());
	}

	// integral; checking overflow.
	template <typename T>
	void Increment_imp(T &value, std::true_type)
	{
		Add(value, 1, value);
	}

	// floating.
	template <typename T>
	void Increment_imp(T &value, std::false_type)
	{
		++value;
	}

	template <typename T>
	void Decrement(T &value)
	{
		static_assert(std::is_arithmetic<T>::value,
			"Only arithmetic data types are supported for this function template.");
		Decrement_imp(value, std::is_integral<T>());
	}

	// integral; checking overflow.
	template <typename T>
	void Decrement_imp(T &value, std::true_type)
	{
		Subtract(value, 1, value);
	}

	// floating.
	template <typename T>
	void Decrement_imp(T &value, std::false_type)
	{
		--value;
	}

	template <typename T, typename U>
	T SafeCast(U src)
	{
		static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<U>::value,
			"Only arithmetic data types are supported for this function template.");
		T dst;
		SafeCast_imp(src, dst, std::is_integral<U>(), std::is_integral<T>());
		return dst;
	}

	// integral to integral; checking overflow.
	template <typename T, typename U>
	void SafeCast_imp(U src, T &dst, std::true_type, std::true_type)
	{
		if (!msl::utilities::SafeCast(src, dst))
		{
			std::ostringstream errMsg;
			errMsg << "The source value exceeds the limit of " << typeid(dst).name();
			throw std::overflow_error(errMsg.str());
		}
	}

	// floating point to integral; checking overflow, suppressing data loss warning.
	template <typename T, typename U>
	void SafeCast_imp(U src, T &dst, std::false_type, std::true_type)
	{
		if (static_cast<U>(std::numeric_limits<T>::max()) < src)
			throw std::overflow_error("Source value is too high.");
		else if (static_cast<U>(std::numeric_limits<T>::min()) > src)
			throw std::overflow_error("Source value is too low.");
		else
			dst = static_cast<T>(src);
	}

	// integral to floating type; suppressing data loss warning if dst is float.
	template <typename T, typename U>
	void SafeCast_imp(U src, T &dst, std::true_type, std::false_type)
	{
		dst = static_cast<T>(src);
	}

	// floating to floating; suppressing data loss warning if src is double and dst is float.
	template <typename T, typename U>
	void SafeCast_imp(U src, T &dst, std::false_type, std::false_type)
	{
		dst = static_cast<T>(src);
	}
}

#endif
