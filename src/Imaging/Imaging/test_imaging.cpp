#include <iostream>

#include "utilities/containers.h"
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
	// Point2D(const std::array<T, 2> &) + (default) Point2D &operator=(const Point2D<T> &)  ???
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

	Imaging::Point2D<int> ptInt1, ptInt2;
	Utilities::CastRange(pt1.cbegin(), pt1.cend(), ptInt1.begin());
	Utilities::Cast(pt1, ptInt2);

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

void TestImage(void)
{
	using namespace Imaging;

	ImageFrame img1;
	img1.Reset<unsigned char>({ 16, 8 }, 3);
	img1.Reset<unsigned char>({ 48, 8 }, 1);
	img1.Reset<unsigned char>({ 48, 8 });
	img1.Reset<int>({ 4, 8 }, 3);
	img1.Reset<int>({ 12, 8 }, 1);
	img1.Reset<int>({ 12, 8 });
	img1.Reset(DataType::UCHAR, { 16, 8 }, 3);
	img1.Reset(DataType::UCHAR, { 48, 8 }, 1);
	img1.Reset(DataType::UCHAR, { 48, 8 });
	img1.Reset(DataType::INT, { 4, 8 }, 3);
	img1.Reset(DataType::INT, { 12, 8 }, 1);
	img1.Reset(DataType::INT, { 12, 8 });
}

void TestImageProcessing(void)
{
	using namespace Imaging;

	// Load an image from a file.
	cv::Mat cvSrc1 = cv::imread(std::string("Lenna.png"), CV_LOAD_IMAGE_COLOR);
	cv::namedWindow(std::string("Source 1"), CV_WINDOW_AUTOSIZE);
	cv::imshow(std::string("Source 1"), cvSrc1);
	cv::waitKey(0);

	// Copy image data from cv::Mat object to ImageFrame.
	ImageFrame img1;
	ImageSize szSrc1;
	Utilities::Cast(cvSrc1.cols, szSrc1.width);
	Utilities::Cast(cvSrc1.rows, szSrc1.height);
	img1.CopyFrom(reinterpret_cast<const ImageFrame::ByteType *>(cvSrc1.ptr()),
		GetDataType(cvSrc1.depth()), szSrc1, cvSrc1.channels(),
		cvSrc1.cols * cvSrc1.channels() * cvSrc1.elemSize1());
	
	// Copy image data from ImageFrame to cv::Mat.
	cv::Mat cvDst1 = CreateCvMat(img1);
	cv::namedWindow(std::string("Copied 1"), CV_WINDOW_AUTOSIZE);
	cv::imshow(std::string("Copied 1"), cvDst1);
	cv::waitKey(0);

	// Copy an ROI to a separate ImageFrame.
	ImageFrame::ROI roiSrc1({ 100, 100 }, img1.size - ImageFrame::SizeType(100));
	ImageFrame img2 = img1.CopyTo(roiSrc1);

	// Create a cv::Mat object sharing the memory.
	auto it = img2.Begin();
	cv::Mat cvDst2 = CreateCvMatShared(img2);
	cv::namedWindow(std::string("ROI and Shared 1"), CV_WINDOW_AUTOSIZE);
	cv::imshow(std::string("ROI and Shared 1"), cvDst2);
	cv::waitKey(0);
}

int main(void)
{
	TestPoint2D();
	TestROI();
	TestImage();
	TestImageProcessing();
}