#include <iostream>

#include "utilities/containers.h"
//#include "utilities/algorithms.h"
//#include "coordinates.h"
#include "image.h"
#include "opencv_interface.h"

template <typename T>
void TestPoint2D_imp(void)
{
	std::cout << "Testing constructors of Point2D<" << typeid(T).name() << ">." << std::endl;

	// (default) Point2D(void)
	Imaging::Point2D<T> pt1;			// NON-ZERO!
	pt1.x = 1, pt1.y = 2;

	// (default) Point2D(const Point2D<T> &)
	Imaging::Point2D<T> pt2 = pt1;

	Imaging::Point2D<T> pt3;
	// std::array<T, 2>::operator= + Point2D(const std::array<T, 2> &)
	pt3 = pt1;

	// Point2D(const std::array<T, 2> &)
	std::array<T, 2> array1{ 5, 6 };
	Imaging::Point2D<T> pt4(array1);
	Imaging::Point2D<T> pt5 = array1;

	// Point2D(T, T) though uniform initialization.
	Imaging::Point2D<T> pt6({ static_cast<T>(5), static_cast<T>(6) });
	Imaging::Point2D<T> pt7 = { 7, 8 };
	Imaging::Point2D<T> pt8{ 9, 10 };
	Imaging::Point2D<T> pt9(11, 12);

	Imaging::Point2D<T> pt10;
	// std::array<T, 2>::operator= + Point2D(const std::array<T, 2> &) ???
	pt10 = array1;

	// The operators of std::array<T, N> work OUTSIDE of the namespace. Great!
	// Note: Using overloaded operators of std::array<T, N> requires a ctor with
	// std::array<T, N>.
	std::cout << "Testing with the operators of std::array<" << typeid(T).name() << ", 2>." <<
		std::endl;

	Imaging::Point2D<T> pt11 = pt1 + pt1;	// {2, 4}
	Imaging::Point2D<T> pt12 = pt1 - pt1;	// {0, 0}
	Imaging::Point2D<T> pt13 = pt1 * pt1;	// {1, 4}
	Imaging::Point2D<T> pt14(1, 2), pt15(1, 2), pt16(1, 2);
	pt14 += pt1;	// {2, 4}
	pt15 -= pt1;	// {0, 0}
	pt16 *= pt1;	// {1, 4}
	Imaging::Point2D<T> pt17 = pt1 + static_cast<T>(1);	// {2, 3}
	Imaging::Point2D<T> pt18 = pt1 - static_cast<T>(1);	// {0, 1}
	Imaging::Point2D<T> pt19 = pt1 * static_cast<T>(1);	// {1, 2}
	Imaging::Point2D<T> pt20(1, 2), pt21(1, 2), pt22(1, 2);
	pt20 += static_cast<T>(1);	// {2, 3}
	pt21 -= static_cast<T>(1);	// {0, 1}
	pt22 *= static_cast<T>(1);	// {1, 2}
	Imaging::Point2D<T> pt23(1, 2), pt24(1, 2), pt25(1, 2), pt26(1, 2);
	++pt23;	// {2, 3}
	pt24++;	// {2, 3}
	--pt25;	// {0, 1}
	pt26--;	// {0, 1}
	bool b1 = pt23 == pt24;		// true
	bool b2 = pt25 != pt26;		// false

	std::array<int, 2> arrayI2 = Imaging::Cast<int>(array1);
	Imaging::Point2D<int> ptInt;
	Imaging::Cast(pt1.cbegin(), pt1.cend(), ptInt.begin());

	std::array<int, 2> arrayI3 = Imaging::Cast<int>(pt1);
	//Imaging::Point2D<int> ptInt = Imaging::Cast<int>(pt1);
	//Imaging::Point2D<int> ptInt = { 1, 2 };
	//Imaging::Point2D<T> pt11 = pt1 + ptInt;	// {2, 4}
	//Imaging::Point2D<T> pt12 = pt1 - ptInt;	// {0, 0}
	//Imaging::Point2D<T> pt13 = pt1 * ptInt;	// {1, 4}

	//pt14 += ptInt;	// {2, 4}
	//pt15 -= ptInt;	// {0, 0}
	//pt16 *= ptInt;	// {1, 4}

	//Imaging::Point2D<T> pt17 = pt1 + static_cast<int>(1);	// {2, 3}
	//Imaging::Point2D<T> pt18 = pt1 - static_cast<int>(1);	// {0, 1}
	//Imaging::Point2D<T> pt19 = pt1 * static_cast<int>(1);	// {1, 2}

	//pt20 += static_cast<int>(1);	// {2, 3}
	//pt21 -= static_cast<int>(1);	// {0, 1}
	//pt22 *= static_cast<int>(1);	// {1, 2}



	// Point2D(const Point2D<U> &)
	//Imaging::Point2D<T> pt27 = ptInt;

	// Point2D(const Point2D<U> &) + (default) Point2D &operator=(const Point2D<T> &)
	//ptInt = pt27;

	std::cout << "Completed testing Poinr2D<" << typeid(T).name() << ">." << std::endl;
}

void TestPoint2D(void)
{
	TestPoint2D_imp<int>();
	TestPoint2D_imp<unsigned int>();
	TestPoint2D_imp<long long>();
	TestPoint2D_imp<unsigned long long>();
	TestPoint2D_imp<float>();
	TestPoint2D_imp<double>();
}

void TestROI(void)
{
	Imaging::ImageFrame::ROI roi1;
	Imaging::ImageFrame::ROI roi2 = { { 0, 0 }, { 64, 32 } };
	Imaging::ImageFrame::ROI roi3{ { 0, 0 }, { 64, 32 } };
	if (roi2 == roi3)
		std::cout << "good" << std::endl;
	if (roi2 == Imaging::ImageFrame::ROI{ { 0, 0 }, roi3.size })
		std::cout << "good" << std::endl;
	//if (roi2 == { { 0, 0 }, roi3.size })
	//	std::cout << "good" << std::endl;
}

int main(void)
{
	TestPoint2D();
	TestROI();
}