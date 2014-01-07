#include "opencv_interface.h"

namespace Imaging
{
	cv::Mat CreateCvMat(const ImageFrame &imgSrc)
	{
		cv::Mat cvDst = CreateCvMat(imgSrc.dataType, imgSrc.size, imgSrc.depth);

		if (imgSrc.HaveZeroPaddingBytes())
		{	// Copy entire data in one copy.
#if defined(_MSC_VER)
			std::copy_n(imgSrc.Cbegin(), imgSrc.data.size(),
				stdext::checked_array_iterator<unsigned char *>(cvDst.ptr(), imgSrc.data.size()));
#else
			std::copy_n(imgSrc.Cbegin(), imgSrc.data.size(), cvDst.ptr());
#endif
		}
		else
		{	// Compute effective bytes/line, and copy only the effective bytes line by line.
			auto bytes_line = cvDst.cols * cvDst.channels() * cvDst.elemSize1();
#if defined(_MSC_VER)
			auto bytes_total = bytes_line * imgSrc.size.height;
			Utilities::CopyLines(imgSrc.Cbegin(), imgSrc.bytesPerLine,
				stdext::checked_array_iterator<unsigned char *>(cvDst.ptr(), bytes_total),
				bytes_line, bytes_line, imgSrc.size.height);
#else
			Utilities::CopyLines(imgSrc.Cbegin(), imgSrc.bytesPerLine,
				cvDst.ptr(), bytes_line, bytes_line, imgSrc.size.height);
#endif
		}
		return cvDst;
	}

	// Run only if source image frame does not have padding bytes.
	cv::Mat CreateCvMatShared(ImageFrame &imgSrc)
	{
		if (imgSrc.HaveZeroPaddingBytes())
		{
			auto it = imgSrc.Begin();
			return cv::Mat(Utilities::Cast<int>(imgSrc.size.height),
				Utilities::Cast<int>(imgSrc.size.width),
				GetOpenCvType(imgSrc.dataType, imgSrc.depth), &(*it));
		}
		else
			throw std::runtime_error(
			"Cannot create a Cv::Mat object with shared memory because of padding bytes.");
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

	int GetOpenCvType(DataType ty, std::size_t d)
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

}