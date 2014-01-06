#if !defined(SAFE_OPERATIONS_H)
#define SAFE_OPERATIONS_H

/*
Declares and defines common arithmetic operations with overflow checking.
Features are designed as non-member function templates in a namespace.
If overflow happens, the functions throw an std::overflow_error.

Employs the built-in safeint.h for Visual C++.
Employs the SafeInt.hpp from http://safeint.codeplex.com/ for other compilers.

All function templates from SafeInt library throws no exception.
The function templates in this file provide similar but more convenient interface with
exception.
*/

#include <stdexcept>
#include <sstream>
//#include <type_traits>
#include <typeinfo>

//#if defined(_MSC_VER)
//// Use the built-in safeint.h for Visual C++.
////#include <safeint.h>
////using namespace msl::utilities;
//#else
//// Use the downloadable SafeInt.hpp from http://safeint.codeplex.com/ for other compilers.
//#include "SafeInt.hpp"
//#endif
#include "SafeInt.hpp"

#if defined(__GNUG__)
// std::enable_if_t<bool, T> is not defined for GCC as it is a C++14 feature.
namespace std
{
	template <bool B, typename T = void>
	using enable_if_t = typename enable_if<B, T>::type;
}
#endif

namespace Utilities
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
	binary (i.e. two input arguments) operations.
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

	1. Enable the function only if the result data type is the same or wider than source
	data.
	2. Disable the function for other scenarios.
	Ignore overflow risk. */

	/* Operations between integral type and floating point type.
	Operations between integral type and floating point type is okay only if the result
	value is floating point and it is wider (not the same) than the integral source value.

	1. Enable the function only if the result value is floating and wider than the integral
	source value.
	2. Disable the function for other scenarios. */

	/* std::enable_if_t<T> and function overload.
	Implementing all of the scheme above takes a lot of function overload implementation for
	a simple operation.

	Use std::enable_if_t<T> for the return variable to reduce the number of required
	function arguments. */

	/* std::enable_if<> vs. static_assert().
	The condition statement of static_assert() is evaluated at different time depending on
	compilers.
	static_assert() can throws a custom error message at compile time.
	static_assert() cannot prevent a function template from being selected for wrong data
	types because it is inside of the function template.

	Use static_assert() at class definition.
	Use enable_if<> to let function templates to be selected/hidden for certain data types.
	Use static_assert() to throw a custom error message at compile time AFTER the function
	template is called.	*/

	////////////////////////////////////////////////////////////////////////////////////////
	// Add
	// NOTE: The result data type could be T or U for these high-level function templates.

	// 1. Same data type only. T = T + T (T + T -> T)
	// 2. Different data type scenario A. T = T + U (T + U -> T)
	template <typename T, typename U>
	std::enable_if_t<std::is_arithmetic<T>::value && std::is_arithmetic<U>::value, void> Add(
		T t, U u, T &result)
	{
		Internal::Add_imp(t, u, result, std::is_integral<T>(), std::is_integral<U>());
	}

	// 3. Different data type scenario B. U = T + U (T + U -> U)
	template <typename T, typename U>
	std::enable_if_t<std::is_arithmetic<T>::value && std::is_arithmetic<U>::value && !std::is_same<T, U>::value, void> Add(
		T t, U u, U &result)
	{
		Internal::Add_imp(u, t, result, std::is_integral<U>(), std::is_integral<T>());
	}

	namespace Internal
	{
		// NOTE: The result data type is always T for these low-level function templates.

		// integral = integral + integral; checking overflow.
		template <typename T, typename U>
		void Add_imp(T t, U u, T &result, std::true_type, std::true_type)
		{
			if (!::SafeAdd(t, u, result))
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
	}

	// Add
	////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////
	// Multiply
	// NOTE: The result data type could be T or U for these high-level function templates.

	// 1. Same data type only. T = T * T (T * T -> T)
	// 2. Different data type scenario A. T = T * U (T * U -> T)
	template <typename T, typename U>
	std::enable_if_t<std::is_arithmetic<T>::value && std::is_arithmetic<U>::value, void> Multiply(
		T t, U u, T &result)
	{
		Internal::Multiply_imp(t, u, result, std::is_integral<T>(), std::is_integral<U>());
	}

	// 3. Different data type scenario B. U = T * U (T * U -> U)
	template <typename T, typename U>
	std::enable_if_t<std::is_arithmetic<T>::value && std::is_arithmetic<U>::value && !std::is_same<T, U>::value, void> Multiply(
		T t, U u, U &result)
	{
		Internal::Multiply_imp(u, t, result, std::is_integral<U>(), std::is_integral<T>());
	}

	namespace Internal
	{
		// NOTE: The result data type is always T for these low-level function templates.

		// integral = integral * integral; checking overflow.
		template <typename T, typename U>
		void Multiply_imp(T t, U u, T &result, std::true_type, std::true_type)
		{
			if (!::SafeMultiply(t, u, result))
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
		std::enable_if_t<(sizeof(T) > sizeof(U)), void > Multiply_imp(T t, U u, T &result,
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
	}

	// Multiply
	////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////
	// Subtract
	// NOTE: The result data type is always T for subtraction or division.

	// 1. Same data type only. T = T - T (T - T -> T)
	// 2. Different data type scenario A. T = T - U (T - U -> T)
	template <typename T, typename U>
	std::enable_if_t<std::is_arithmetic<T>::value && std::is_arithmetic<U>::value, void> Subtract(
		T t, U u, T &result)
	{
		Internal::Subtract_imp(t, u, result, std::is_integral<T>(), std::is_integral<U>());
	}

	// 3. Different data type scenario B. U = T - U (T - U -> U)
	template <typename T, typename U>
	std::enable_if_t<std::is_arithmetic<T>::value && std::is_arithmetic<U>::value && !std::is_same<T, U>::value, void> Subtract(
		T t, U u, U &result)
	{
		static_assert(std::is_same<T, U>::value,
			"The first source value must be the same type as the result value.");
		throw std::logic_error("This function must not be called as it is disabled.");
	}

	namespace Internal
	{
		// integral = integral - integral; checking overflow.
		template <typename T, typename U>
		void Subtract_imp(T t, U u, T &result, std::true_type, std::true_type)
		{
			if (!::SafeSubtract(t, u, result))
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
	}

	// Subtract
	////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////
	// Increment

	// ++T
	template <typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, void> Increment(T &value)
	{
		Internal::Increment_imp(value, std::is_integral<T>());
	}

	namespace Internal
	{
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
	}

	// Increment
	////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////
	// Decrement

	// --T
	template <typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, void> Decrement(T &value)
	{
		Internal::Decrement_imp(value, std::is_integral<T>());
	}

	namespace Internal
	{
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
	}

	// Decrement
	////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////
	// Cast

	// Cast(T, U); T -> U
	template <typename T, typename U>
	std::enable_if_t<std::is_arithmetic<T>::value && std::is_arithmetic<U>::value, void> Cast(
		T src, U &dst)
	{
		Internal::Cast_imp(src, dst, std::is_integral<T>(), std::is_integral<U>());
	}

	// T = Cast<T>(U); U -> T
	/* NOTE: Users must make sure the data type of the return value is correct because
	there is no way to ensure it. */
	template <typename T, typename U>
	std::enable_if_t<std::is_arithmetic<T>::value && std::is_arithmetic<U>::value, T> Cast(
		U src)
	{
		T dst;
		Internal::Cast_imp(src, dst, std::is_integral<U>(), std::is_integral<T>());
		return dst;
	}

	namespace Internal
	{
		// integral to integral; checking overflow.
		template <typename T, typename U>
		void Cast_imp(T src, U &dst, std::true_type, std::true_type)
		{
			if (!::SafeCast(src, dst))
			{
				std::ostringstream errMsg;
				errMsg << "The source value exceeds the limit of " << typeid(dst).name();
				throw std::overflow_error(errMsg.str());
			}
		}

		// floating point to integral; checking overflow, suppressing data loss warning.
		template <typename T, typename U>
		void Cast_imp(T src, U &dst, std::false_type, std::true_type)
		{
			if (static_cast<T>(std::numeric_limits<U>::max()) < src)
				throw std::overflow_error("Source value is too high.");
			else if (static_cast<T>(std::numeric_limits<U>::min()) > src)
				throw std::overflow_error("Source value is too low.");
			else
				dst = static_cast<U>(src);
		}

		// integral to floating type; suppressing data loss warning if dst is float.
		template <typename T, typename U>
		void Cast_imp(T src, U &dst, std::true_type, std::false_type)
		{
			dst = static_cast<U>(src);
		}

		// floating to floating; suppressing data loss warning if src is double and dst is
		// float.
		template <typename T, typename U>
		void Cast_imp(T src, U &dst, std::false_type, std::false_type)
		{
			dst = static_cast<U>(src);
		}
	}

	// Cast
	////////////////////////////////////////////////////////////////////////////////////////

}

#endif
