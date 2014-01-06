#if !defined(OPENCV_INTERFACE_H)
#define OPENCV_INTERFACE_H

#include "opencv2/opencv.hpp"

#include "utilities/safe_operations.h"
#include "image.h"

namespace Imaging
{
	inline int GetOpenCvType(DataType ty, std::size_t d)
	{
		int depth;
		Utilities::Cast(d, depth);
		switch (ty)
		{
		case DataType::CHAR:
			return CV_MAKETYPE(CV_8S, depth);
		case DataType::SCHAR:
			return CV_MAKETYPE(CV_8S, depth);
		case DataType::UCHAR:
			return CV_MAKETYPE(CV_8U, depth);
		case DataType::SHORT:
			return CV_MAKETYPE(CV_16S, depth);
		case DataType::USHORT:
			return CV_MAKETYPE(CV_16U, depth);
		case DataType::INT:
			return CV_MAKETYPE(CV_32S, depth);
		case DataType::FLOAT:
			return CV_MAKETYPE(CV_32F, depth);
		case DataType::DOUBLE:
			return CV_MAKETYPE(CV_64F, depth);
		default:
			std::ostringstream errMsg;
			errMsg << typeid(DataType).name() << "::" <<
				static_cast<std::underlying_type<DataType>::type>(ty) <<
				" is not supported by OpenCV at this moment.";
			throw std::logic_error(errMsg.str());
		}
	}

	inline DataType GetDataType(int cvType)
	{
		switch (cvType)
		{
		case CV_8S:
			return DataType::SCHAR;
		case CV_8U:
			return DataType::UCHAR;
		case CV_16S:
			return DataType::SHORT;
		case CV_16U:
			return DataType::USHORT;
		case CV_32S:
			return DataType::INT;
		case CV_32F:
			return DataType::FLOAT;
		case CV_64F:
			return DataType::DOUBLE;
		default:
			throw std::logic_error("Unrecognized OpenCV data type.");
		}
	}

	inline cv::Mat CreateCvMat(DataType ty, const ImageSize &sz, ImageFrame::SizeType d)
	{
		return cv::Mat(Utilities::Cast<int>(sz.height), Utilities::Cast<int>(sz.width),
			GetOpenCvType(ty, d));
	}

	inline cv::Mat CreateCvMatShared(ImageFrame &imgSrc)
	{
		auto it = imgSrc.Begin();
		return cv::Mat(Utilities::Cast<int>(imgSrc.size.height),
			Utilities::Cast<int>(imgSrc.size.width),
			GetOpenCvType(imgSrc.dataType, imgSrc.depth), &(*it));
	}
}

#endif
