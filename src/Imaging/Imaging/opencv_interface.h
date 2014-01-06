#if !defined(OPENCV_INTERFACE_H)
#define OPENCV_INTERFACE_H

#include "opencv2/opencv.hpp"

#include "utilities/algorithms.h"
#include "image.h"

namespace Imaging
{
	cv::Mat CreateCvMat(DataType ty, const ImageSize &sz, ImageFrame::SizeType d);
	cv::Mat CreateCvMat(const ImageFrame &imgSrc);
	cv::Mat CreateCvMatShared(ImageFrame &imgSrc);
	DataType GetDataType(int cvType);
	int GetOpenCvType(DataType ty, std::size_t d);
}

//#include "opencv_interface_inl.h"
namespace Imaging
{
	inline cv::Mat CreateCvMat(DataType ty, const ImageSize &sz, ImageFrame::SizeType d)
	{
		return cv::Mat(Utilities::Cast<int>(sz.height), Utilities::Cast<int>(sz.width),
			GetOpenCvType(ty, d));
	}
}
#endif
