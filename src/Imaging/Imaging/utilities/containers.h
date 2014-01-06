#if !defined(CONTAINERS_H)
#define CONTAINERS_H

/*
Declares and defines arithmetic operators and common functions for standard container
classes.
The operators are defined under global namespace for easier use. (maybe a bad idea?)
The operators take container objects with the same base data type.
Function templates can take container objects with different base data types.
*/

#include <array>
#include <vector>

#include "algorithms.h"

/* Operations between containers with different data types.
C++ does not support operations between containers with different data types, so custom
feature needs to be implemented.
Function overload is a good way to do this.
Since C++ cannot overload different functions for different return data types, it is
necessary to place return value at the function parameter list and return it as a reference.

Overload operators for only the same type operations.
Pre-cast source arrays to temporary variables with destination type, and then run the
operation with the temporary variables.
Functions defined in algorithm.h can be an alternative. */

/* Reusing {+=, -=, *=} operators for {+, -, *}.
Copy ctor of std::array is no more expensive than default ctor.

Create the result array by copying lhs and then run the assignment operators. */

// C = A + B
template <typename T, std::size_t N>
std::array<T, N> operator+(const std::array<T, N> &lhs, const std::array<T, N> &rhs)
{
	std::array<T, N> result = lhs;
	result += rhs;
	return result;
}

// C = A - B
template <typename T, std::size_t N>
std::array<T, N> operator-(const std::array<T, N> &lhs, const std::array<T, N> &rhs)
{
	std::array<T, N> result = lhs;
	result -= rhs;
	return result;
}

// C = A * B
template <typename T, std::size_t N>
std::array<T, N> operator*(const std::array<T, N> &lhs, const std::array<T, N> &rhs)
{
	std::array<T, N> result = lhs;
	result *= rhs;
	return result;
}

// C = A + b
template <typename T, std::size_t N>
std::array<T, N> operator+(const std::array<T, N> &lhs, T rhs)
{
	std::array<T, N> result = lhs;
	result += rhs;
	return result;
}

// C = A - b
template <typename T, std::size_t N>
std::array<T, N> operator-(const std::array<T, N> &lhs, T rhs)
{
	std::array<T, N> result = lhs;
	result -= rhs;
	return result;
}

// C = A * b
template <typename T, std::size_t N>
std::array<T, N> operator*(const std::array<T, N> &lhs, T rhs)
{
	std::array<T, N> result = lhs;
	result *= rhs;
	return result;
}

// A += B
template <typename T, std::size_t N>
std::array<T, N> &operator+=(std::array<T, N> &lhs, const std::array<T, N> &rhs)
{
	Utilities::AddRange(rhs.cbegin(), rhs.cend(), lhs.begin());
	return lhs;
}

// A -= B
template <typename T, std::size_t N>
std::array<T, N> &operator-=(std::array<T, N> &lhs, const std::array<T, N> &rhs)
{
	Utilities::SubtractRange(rhs.cbegin(), rhs.cend(), lhs.begin());
	return lhs;
}

// A *= B
template <typename T, std::size_t N>
std::array<T, N> &operator*=(std::array<T, N> &lhs, const std::array<T, N> &rhs)
{
	Utilities::MultiplyRange(rhs.cbegin(), rhs.cend(), lhs.begin());
	return lhs;
}

// A += b
template <typename T, std::size_t N>
std::array<T, N> &operator+=(std::array<T, N> &lhs, T rhs)
{
	for (auto &value : lhs)
		Utilities::Add(value, rhs, value);
	return lhs;
}

// A -= b
template <typename T, std::size_t N>
std::array<T, N> &operator-=(std::array<T, N> &lhs, T rhs)
{
	for (auto &value : lhs)
		Utilities::Subtract(value, rhs, value);
	return lhs;
}

// A *= b
template <typename T, std::size_t N>
std::array<T, N> &operator*=(std::array<T, N> &lhs, T rhs)
{
	for (auto &value : lhs)
		Utilities::Multiply(value, rhs, value);
	return lhs;
}

// ++A
template <typename T, std::size_t N>
std::array<T, N> &operator++(std::array<T, N> &rhs)
{
	for (auto &value : rhs)
		Utilities::Increment(value);
	return rhs;
}

// A++
template <typename T, std::size_t N>
std::array<T, N> operator++(std::array<T, N> &lhs, int)
{
	std::array<T, N> temp = lhs;
	++lhs;
	return temp;
}

// --A
template <typename T, std::size_t N>
std::array<T, N> &operator--(std::array<T, N> &rhs)
{
	for (auto &value : rhs)
		Utilities::Decrement(value);
	return rhs;
}

// A--
template <typename T, std::size_t N>
std::array<T, N> operator--(std::array<T, N> &lhs, int)
{
	std::array<T, N> temp = lhs;
	--lhs;
	return temp;
}

namespace Utilities
{
	// To be removed
	//template <typename T, typename U, std::size_t N>
	//std::enable_if_t<std::is_floating_point<U>::value, std::array<T, N>> RoundAs(
	//	const std::array<U, N> &src)
	//{
	//	std::array<T, N> dst;
	//	RoundRange(src.cbegin(), src.cend(), dst.begin());
	//	return dst;
	//}

	template <typename T, typename U, std::size_t N>
	std::enable_if_t<std::is_floating_point<T>::value, void> Round(
		const std::array<T, N> &src, std::array<U, N> &dst)
	{
		RoundRange(src.cbegin(), src.cend(), dst.begin());
	}

	// To be removed
	//template <typename T, typename U, std::size_t N>
	//std::enable_if_t<!std::is_same<T, U>::value, std::array<T, N>> Cast(
	//	const std::array<U, N> &src)
	//{
	//	static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<U>::value,
	//		"Only arithmetic data types are supported for this class template.");
	//	std::array<T, N> dst;
	//	CastRange(src.cbegin(), src.cend(), dst.begin());
	//	return dst;
	//}

	// To be removed
	//template <typename T, typename U, std::size_t N>
	//std::enable_if_t<std::is_same<T, U>::value, std::array<T, N>> Cast(
	//	const std::array<U, N> &src)
	//{
	//	static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<U>::value,
	//		"Only arithmetic data types are supported for this class template.");
	//	return src;
	//}

	template <typename T, typename U, std::size_t N>
	std::enable_if_t<!std::is_same<T, U>::value, void> Cast(
		const std::array<T, N> &src, std::array<U, N> &dst)
	{
		CastRange(src.cbegin(), src.cend(), dst.begin());
	}

	template <typename T, typename U, std::size_t N>
	std::enable_if_t<std::is_same<T, U>::value, void> Cast(
		const std::array<T, N> &src, std::array<U, N> &dst)
	{
		dst = src;
	}

	template <typename T>
	std::vector<T> GetRangeVector(std::size_t length)
	{
		std::vector<T> v(length);
		FillRange(v.begin(), v.end());
		return v;
	}
}
#endif
