#if !defined(ALGORITHMS_H)
#define ALGORITHMS_H

/*
Declares and defines common arithmetic operations for standard container classes.
Features are designed as non-member function templates in a namespace.
*/

#include <numeric>

#include "safe_operations.h"

namespace Utilities
{
	/* Function arguments.
	The arguments are defined as iterators instead of containers, so these functions can be
	shared by multiple container classes. */

	/* Warning: C = A + B vs. A += B

	The operation & assignment {+=, -=, *=} might have been implemented using {+, -, *}
	by setting the output iterator as the same one as the input (lhs) iterator.
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
	void CastRange(InputIterator itSrc, InputIterator itSrcLast, OutputIterator itDst)
	{
		for (; itSrc != itSrcLast; ++itSrc, ++itDst)
			Cast(*itSrc, *itDst);
	}

	template <typename InputIterator, typename OutputIterator>
	std::enable_if_t<std::is_floating_point<typename InputIterator::value_type>::value,
		void> RoundRange(InputIterator itSrc, InputIterator itSrcLast, OutputIterator itDst)
	{
		for (; itSrc != itSrcLast; ++itSrc, ++itDst)
			Cast(std::round(*itSrc), *itDst);
	}

}

#endif
