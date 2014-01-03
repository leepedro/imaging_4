#if !defined(ALGORITHMS_H)
#define ALGORITHMS_H

/*
Declares and defines common arithmetic operations for standard container classes.
Features are designed as non-member function templates in a namespace.
*/

#include "safe_operations.h"

namespace Imaging
{
	/* Function arguments.
	The arguments are defined as iterators instead of containers, so these functions can be
	shared by multiple container classes. */

	/* Warning: C = A + B vs. A += B

	The operation & assignment {+=, -=, *=} might have been implemented using {+, -, *}
	by setting the output iterator the same as one of input (lhs) iterator.
	Must NOT do so because the iterators of lhs will be increased twice per loop.
	Instead, make separate implementations for such scenario. */

	// C = A + B
	template <typename InputIteratorA, typename InputIteratorB, typename OutputIterator>
	void AddRange(InputIteratorA itA, InputIteratorA itA_last, InputIteratorB itB,
		OutputIterator itC)
	{
		for (; itA != itA_last; ++itA, ++itB, ++itC)
			Add(*itA, *itB, *itC);
	}

	// C = A - B
	template <typename InputIteratorA, typename InputIteratorB, typename OutputIterator>
	void SubtractRange(InputIteratorA itA, InputIteratorA itA_last, InputIteratorB itB,
		OutputIterator itC)
	{
		for (; itA != itA_last; ++itA, ++itB, ++itC)
			Subtract(*itA, *itB, *itC);
	}

	// C = A * B
	template <typename InputIteratorA, typename InputIteratorB, typename OutputIterator>
	void MultiplyRange(InputIteratorA itA, InputIteratorA itA_last, InputIteratorB itB,
		OutputIterator itC)
	{
		for (; itA != itA_last; ++itA, ++itB, ++itC)
			Multiply(*itA, *itB, *itC);
	}

	// A += B
	template <typename InputIterator, typename InOutputIterator>
	void AddRange(InputIterator itSrc, InputIterator itSrcLast, InOutputIterator itSrcDst)
	{
		for (; itSrc != itSrcLast; ++itSrc, ++itSrcDst)
			Add(*itSrcDst, *itSrc, *itSrcDst);
	}

	// A -= B
	template <typename InputIterator, typename InOutputIterator>
	void SubtractRange(InputIterator itSrc, InputIterator itSrcLast,
		InOutputIterator itSrcDst)
	{
		for (; itSrc != itSrcLast; ++itSrc, ++itSrcDst)
			Subtract(*itSrcDst, *itSrc, *itSrcDst);
	}

	// A *= B
	template <typename InputIterator, typename InOutputIterator>
	void MultiplyRange(InputIterator itSrc, InputIterator itSrcLast,
		InOutputIterator itSrcDst)
	{
		for (; itSrc != itSrcLast; ++itSrc, ++itSrcDst)
			Multiply(*itSrcDst, *itSrc, *itSrcDst);
	}

	// ++A
	template <typename Iterator>
	void IncrementRange(Iterator it, Iterator itLast)
	{
		for (; it != itLast; ++it)
			Increment(*it);
	}

	// --A
	template <typename Iterator>
	void DecrementRange(Iterator it, Iterator itLast)
	{
		for (; it != itLast; ++it)
			Decrement(*it);
	}

	/* Fills the integral range value in an ascending order from zero while preventing
	overflow by comparing the range value with the maximum value at each iteration.
	Once the range value reached the maximum value, it goes back to zero. (inefficient) */
	template <typename Iterator>
	std::enable_if_t<std::is_integral<typename Iterator::value_type>::value, void> FillRange(
		Iterator it, Iterator itLast)
	{
		for (auto value = static_cast<typename Iterator::value_type>(0),
			limit = std::numeric_limits<typename Iterator::value_type>::max(); it != itLast;
			++it)
		{
			*it = value;
			if (value == limit)
				value = 0;
			else
				++value;
		}
	}

	// Fills the floating point range value in an ascending order without the range check.
	template <typename Iterator>
	std::enable_if_t<std::is_floating_point<typename Iterator::value_type>::value, void> FillRange(
		Iterator it, Iterator itLast)
	{
		for (auto value = static_cast<typename Iterator::value_type>(0); it != itLast; ++it,
			++value)
			*it = value;
	}

	// Copies periodically located data using step size (for both source and destination). 
	/*	Incrementing iterators.
	Iterators must NOT be be incremented for this function because it will go beyond range
	at the final loop. Instead, compute the current position at each loop. */
	template <typename InputIterator, typename OutputIterator, typename SizeType>
	void CopyLines(InputIterator itSrc, SizeType stepSrc, OutputIterator itDst,
		SizeType stepDst, SizeType nElemPerLine, SizeType nLines)
	{
		//for (auto H = 0; H != nLines; ++H, itSrc += stepSrc, itDst += stepDst)
		//	std::copy_n(itSrc, nElemPerLine, itDst);
		for (auto H = 0; H != nLines; ++H)
			std::copy_n(itSrc + H * stepSrc, nElemPerLine, itDst + H * stepDst);
	}

	template <typename InputIterator, typename OutputIterator>
	void Cast(InputIterator itSrc, InputIterator itSrcLast, OutputIterator itDst)
	{
		for (; itSrc != itSrcLast; ++itSrc, ++itDst)
			*itDst = Cast<typename OutputIterator::value_type>(*itSrc);
	}
}

#endif
