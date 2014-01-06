#if !defined(COORDINATE_H)
#define COORDINATE_H

#include <array>

namespace Imaging
{
	template <typename T>
	class Point2D : public std::array<T, 2>
	{
		static_assert(std::is_arithmetic<T>::value,
			"Only arithmetic data types are supported for this class template.");
	public:
		////////////////////////////////////////////////////////////////////////////////////
		// Default constructors.
		Point2D(void) = default;
		Point2D &operator=(const Point2D<T> &src) = default;

		////////////////////////////////////////////////////////////////////////////////////
		// Custom constructors.
		Point2D(T x, T y);
		Point2D(const std::array<T, 2> &src);

		T &x = this->at(0);
		T &y = this->at(1);
	};

	template <typename T>
	class Size2D : public std::array<T, 2>
	{
		static_assert(std::is_arithmetic<T>::value,
			"Only arithmetic data types are supported for this class template.");

	public:
		////////////////////////////////////////////////////////////////////////////////////
		// Default constructors.
		Size2D(void) = default;
		Size2D &operator=(const Size2D<T> &src) = default;

		////////////////////////////////////////////////////////////////////////////////////
		// Custom constructors.
		Size2D(T width, T height);
		Size2D(const std::array<T, 2> &src);

		T &width = this->at(0);
		T &height = this->at(1);
	};

	/* Rectangle classes.
	RectTypeA: a rectangle with two corners; point1<T>(x, y), point2<T>(x, y)
	RectTypeB: a rectangle with one corner and extension; origin<T>(x, y), size<U>(w, h)
	RectTypeC: a rectangle with a center and extension; center<T>(x, y), extension<U>(w, h)	*/

	template <typename T>
	class RectTypeA
	{
		static_assert(std::is_arithmetic<T>::value,
		"Only arithmetic data types are supported for this class template.");

	public:
		////////////////////////////////////////////////////////////////////////////////////
		// Operators.
		bool operator==(const RectTypeA<T> &rhs) const;
		bool operator!=(const RectTypeA<T> &rhs) const;

		////////////////////////////////////////////////////////////////////////////////////
		// Data.
		Point2D<T> point1, point2;
	};

	template <typename T, typename U>
	class RectTypeB
	{
		static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<U>::value,
		"Only arithmetic data types are supported for this class template.");

	public:
		////////////////////////////////////////////////////////////////////////////////////
		// Default constructors.
		RectTypeB(void) = default;
		//RectTypeB<T, U> &operator=(const RectTypeB<T, U> &src) = default;

		////////////////////////////////////////////////////////////////////////////////////
		// Custom constructors.
		RectTypeB(const Point2D<T> &orgn, const Size2D<U> &sz);

		////////////////////////////////////////////////////////////////////////////////////
		// Operators.
		bool operator==(const RectTypeB<T, U> &rhs) const;
		bool operator!=(const RectTypeB<T, U> &rhs) const;

		////////////////////////////////////////////////////////////////////////////////////
		// Data.
		Point2D<T> origin;
		Size2D<U> size;
	};

	template <typename T, typename U>
	class RectTypeC
	{
		static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<U>::value,
		"Only arithmetic data types are supported for this class template.");

	public:
		////////////////////////////////////////////////////////////////////////////////////
		// Operators.
		bool operator==(const RectTypeC<T, U> &rhs) const;
		bool operator!=(const RectTypeC<T, U> &rhs) const;

		////////////////////////////////////////////////////////////////////////////////////
		// Data.
		Point2D<T> center;
		Size2D<U> extension;
	};

}

//#include "coordinate_inl.h"
namespace Imaging
{
	////////////////////////////////////////////////////////////////////////////////////
	// Point2D<T>

	template <typename T>
	Point2D<T>::Point2D(T x, T y) : Point2D<T>()
	{
		this->x = x;
		this->y = y;
	}

	template <typename T>
	Point2D<T>::Point2D(const std::array<T, 2> &src) : std::array<T, 2>(src) {}

	// Point2D<T>
	////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////
	// Size2D<T>

	template <typename T>
	Size2D<T>::Size2D(T width, T height) : Size2D<T>()
	{
		this->width = width;
		this->height = height;
	}

	template <typename T>
	Size2D<T>::Size2D(const std::array<T, 2> &src) : std::array<T, 2>(src) {}

	// Size2D<T>
	////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////
	// RectTypeB<T, U>

	template <typename T, typename U>
	RectTypeB<T, U>::RectTypeB(const Point2D<T> &orgn, const Size2D<U> &sz) :
		RectTypeB<T, U>()
	{
		this->origin = orgn;
		this->size = sz;
	}

	template <typename T, typename U>
	bool RectTypeB<T, U>::operator==(const RectTypeB<T, U> &rhs) const
	{
		return this->origin == rhs.origin && this->size == rhs.size;
	}

	template <typename T, typename U>
	bool RectTypeB<T, U>::operator!=(const RectTypeB<T, U> &rhs) const
	{
		return !(*this == rhs);
	}

	// RectTypeB<T, U>
	////////////////////////////////////////////////////////////////////////////////////
	
}

#endif
