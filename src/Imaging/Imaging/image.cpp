#include <sstream>

#include "utilities/containers.h"
#include "image.h"

namespace Imaging
{
	////////////////////////////////////////////////////////////////////////////////////
	// DataType and functions using DataType.

	std::size_t GetNumBytes(DataType ty)
	{
		switch (ty)
		{
		case DataType::UNDEFINED:
			throw std::runtime_error("Data type is undefined.");
		case DataType::CHAR:
			return sizeof(char);
		case DataType::SCHAR:
			return sizeof(signed char);
		case DataType::UCHAR:
			return sizeof(unsigned char);
		case DataType::SHORT:
			return sizeof(short);
		case DataType::USHORT:
			return sizeof(unsigned short);
		case DataType::INT:
			return sizeof(int);
		case DataType::UINT:
			return sizeof(unsigned int);
		case DataType::FLOAT:
			return sizeof(float);
		case DataType::LONGLONG:
			return sizeof(long long);
		case DataType::ULONGLONG:
			return sizeof(unsigned long long);
		case DataType::DOUBLE:
			return sizeof(double);
		default:
			throw std::logic_error("Unknown data type.");
		}
	}

	// DataType and functions using DataType.
	////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////
	// ImageFrame

	////////////////////////////////////////////////////////////////////////////////////
	// Constructors.
	
	ImageFrame &ImageFrame::operator=(const ImageFrame &src)
	{
		// Check source dimension.
		ImageFrame::EvalSize(src.dataType, src.data.size(), src.size.width, src.size.height,
			src.depth);

		// Copy image data.
		this->data_ = src.data;

		// Update dimension.
		this->bytesPerLine_ = src.bytesPerLine;
		this->dataType_ = src.dataType;
		this->depth_ = src.depth;
		this->size_ = src.size;

		return *this;
	}

	/* Move ctor and noexcept.
	Many sources say a move ctor, which calls this operator, must guarante not to throw an
	exception.
	Visual Studio 2013 does NOT support noexcept specifier at this moment.
	If no exception guarantee is required later, remove EvalSize().	*/
	ImageFrame &ImageFrame::operator=(ImageFrame &&src)
	{
		// Check source dimension.
		ImageFrame::EvalSize(src.dataType, src.data.size(), src.size.width, src.size.height,
			src.depth);

		// Move image data.
		this->data_ = std::move(src.data_);

		// Update dimension.
		this->bytesPerLine_ = src.bytesPerLine;
		this->dataType_ = src.dataType;
		this->depth_ = src.depth;
		this->size_ = src.size;

		// Update dimension at the source.
		src.bytesPerLine_ = 0;
		src.dataType_ = DataType::UNDEFINED;
		src.depth_ = 0;
		src.size_ = { 0, 0 };

		return *this;
	}

	// Constructors.
	////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////
	// Methods.

	void ImageFrame::Clear()
	{
		// Clear memory.
		this->data_.clear();

		// Update dimension.
		this->bytesPerLine_ = 0;
		this->dataType_ = DataType::UNDEFINED;
		this->depth_ = 0;
		this->size_ = { 0, 0 };
	}

	void ImageFrame::CopyFrom(const std::vector<ByteType> &srcData, DataType ty,
		const Size2D<SizeType> &sz, SizeType d)
	{
		// Check source dimension.
		ImageFrame::EvalSize(ty, srcData.size(), sz.width, sz.height, d);

		// Copy image data.
		this->data_ = srcData;

		// Update dimension.
		this->bytesPerLine_ = ImageFrame::GetBytesPerLine(ty, sz.width, d);
		this->dataType_ = ty;
		this->depth_ = d;
		this->size_ = sz;
	}

	void ImageFrame::CopyFrom(const ByteType *src, DataType ty, const Size2D<SizeType> &sz,
		SizeType d, SizeType stepBytes)
	{
		// Compute memory requirement.
		auto bytes_line = ImageFrame::GetBytesPerLine(ty, sz.width, d);
		auto bytes_total = sz.height * bytes_line;

		// Copy image data considering padding bytes.
		if (bytes_line == stepBytes)	// identical padding bytes.
			this->data_.assign(src, src + bytes_total);
		else
		{	// different padding bytes.
			if (this->data.size() != bytes_total)
				this->data_.resize(bytes_total);
			auto itDst = this->data_.begin();
			Utilities::CopyLines(src, stepBytes, itDst, bytes_line, stepBytes, sz.height);
			//for (auto H = 0; H != sz.height; ++H, src += stepBytes, itDst += bytes_line)
			//	std::copy_n(src, sz.width * d * GetNumBytes(ty), itDst);
		}

		// Update dimension.
		this->bytesPerLine_ = bytes_line;
		this->dataType_ = ty;
		this->depth_ = d;
		this->size_ = sz;
	}

	ImageFrame ImageFrame::CopyTo(const ROI &roiSrc) const
	{
		// Check source ROI.
		this->EvalRoi(roiSrc);

		ImageFrame imgDst;

		// Update dimension first.
		imgDst.bytesPerLine_ = ImageFrame::GetBytesPerLine(this->dataType,
			roiSrc.size.width, this->depth);
		imgDst.dataType_ = this->dataType;
		imgDst.depth_ = this->depth;
		imgDst.size_ = roiSrc.size;

		// Copy image data.
		if (roiSrc == ROI{ { 0, 0 }, this->size })
			imgDst.data_ = this->data;	// Copy entire image.
		else
		{	// Copy ROI line by line after computing the number of bytes.
			imgDst.Reset(this->dataType, roiSrc.size, this->depth);
			auto itSrc = this->Cbegin(roiSrc.origin);
			auto itDst = imgDst.Begin();
			auto bytes_line_roi = GetNumBytes(this->dataType) * roiSrc.size.width *
				this->depth;
			Utilities::CopyLines(itSrc, this->bytesPerLine, itDst, imgDst.bytesPerLine,
				bytes_line_roi,	roiSrc.size.height);
		}

		return imgDst;
	}

	void ImageFrame::EvalRoi(const Point2D<SizeType> &orgn, const Size2D<SizeType> &sz) const
	{
		Point2D<SizeType> ptEnd = orgn + sz;	// excluding point
		if (orgn.x < 0 || orgn.y < 0 || ptEnd.x > this->size.width ||
			ptEnd.y > this->size.height)
		{
			std::ostringstream errMsg;
			errMsg << "[" << orgn.x << ", " << orgn.y << "] ~ (" << ptEnd.x <<
				", " << ptEnd.y << ") is out of range.";
			throw std::out_of_range(errMsg.str());
		}
	}

	/* Static member function.
	Since this function does not actually use any class member, this method is declared as a
	static function.
	It is declared as a protected function for now because it is not expected to be used by
	any client. */
	void ImageFrame::EvalSize(DataType ty, SizeType length, SizeType w, SizeType h,
		SizeType d)
	{
		auto bytes_line = ImageFrame::GetBytesPerLine(ty, w, d);
		auto bytes_total = bytes_line * h;
		if (length != bytes_total)
		{
			std::ostringstream errMsg;
			errMsg << "The size of the image (" << w << " x " << h << " x " << d <<
				") is not matched with the size of its data (" << length << ").";
			throw std::runtime_error(errMsg.str());
		}
	}

	void ImageFrame::MoveFrom(std::vector<ByteType> &&srcData, DataType ty,
		const Size2D<SizeType> &sz, SizeType d)
	{
		// Check source dimension.
		ImageFrame::EvalSize(ty, srcData.size(), sz.width, sz.height, d);

		// Move image data.
		this->data_ = std::move(srcData);

		// Update dimension.
		this->bytesPerLine_ = ImageFrame::GetBytesPerLine(ty, sz.width, d);
		this->dataType_ = ty;
		this->depth_ = d;
		this->size_ = sz;
	}

	void ImageFrame::Reset(DataType ty, const Size2D<SizeType> &sz, SizeType d)
	{
		// Compute memory requirement.
		auto bytes_line = ImageFrame::GetBytesPerLine(ty, sz.width, d);
		auto bytes_total = sz.height * bytes_line;

		// Memory re-allocation.
		if (this->data.size() != bytes_total)
			this->data_.resize(bytes_total);

		// Update dimension.
		this->bytesPerLine_ = bytes_line;
		this->dataType_ = ty;
		this->depth_ = d;
		this->size_ = sz;
	}

	// Methods.
	////////////////////////////////////////////////////////////////////////////////////

	// ImageFrame
	////////////////////////////////////////////////////////////////////////////////////
}
