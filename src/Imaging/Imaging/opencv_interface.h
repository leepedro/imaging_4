#if !defined(OPENCV_INTERFACE_H)
#define OPENCV_INTERFACE_H

#include <sstream>

#include "opencv2/opencv.hpp"

#include "utilities/safe_operations.h"
#include "image.h"

namespace Imaging
{
	int GetOpenCvType(DataType ty, std::size_t d)
	{
		switch (ty)
		{
		case DataType::CHAR:
			return CV_MAKETYPE(CV_8S, Cast<int>(d));
		case DataType::SCHAR:
			return CV_MAKETYPE(CV_8S, Cast<int>(d));
		case DataType::UCHAR:
			return CV_MAKETYPE(CV_8U, Cast<int>(d));
		case DataType::SHORT:
			return CV_MAKETYPE(CV_16S, Cast<int>(d));
		case DataType::USHORT:
			return CV_MAKETYPE(CV_16U, Cast<int>(d));
		case DataType::INT:
			return CV_MAKETYPE(CV_32S, Cast<int>(d));
		case DataType::FLOAT:
			return CV_MAKETYPE(CV_32F, Cast<int>(d));
		case DataType::DOUBLE:
			return CV_MAKETYPE(CV_64F, Cast<int>(d));
		default:
			std::ostringstream errMsg;
			errMsg << typeid(DataType).name() << "::" <<
				static_cast<std::underlying_type<DataType>::type>(ty) <<
				" is not supported by OpenCV at this moment.";
			throw std::logic_error(errMsg.str());
		}
	}

	DataType GetDataType(int cvType)
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
}

#endif
