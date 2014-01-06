#include <sstream>

#include "utilities/containers.h"
#include "image.h"

namespace Imaging
{
	std::size_t GetNumBytes(DataType ty)
	{
		switch (ty)
		{
		case DataType::UNDEFINED:
			return 0;
		case DataType::CHAR:
		case DataType::SCHAR:
		case DataType::UCHAR:
			return 1;
		case DataType::SHORT:
		case DataType::USHORT:
			return 2;
		case DataType::INT:
		case DataType::UINT:
		case DataType::FLOAT:
			return 4;
		case DataType::LONGLONG:
		case DataType::ULONGLONG:
		case DataType::DOUBLE:
			return 8;
		default:
			throw std::logic_error("Unknown DataType.");
		}
	}

	////////////////////////////////////////////////////////////////////////////////////
	// ImageFrame

	////////////////////////////////////////////////////////////////////////////////////
	// Constructors.

	ImageFrame::ImageFrame(const ImageFrame &src) : ImageFrame()
	{
		*this = src;
	}

	ImageFrame::ImageFrame(ImageFrame &&src) : ImageFrame()
	{
		*this = std::move(src);
	}

	ImageFrame &ImageFrame::operator=(const ImageFrame &src)
	{
		ImageFrame::EvalSize(src.dataType, src.data.size(), src.size.width, src.size.height, src.depth);

		this->data_ = src.data;

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
		ImageFrame::EvalSize(src.dataType, src.data.size(), src.size.width, src.size.height, src.depth);

		this->data_ = std::move(src.data_);

		this->bytesPerLine_ = src.bytesPerLine;
		this->dataType_ = src.dataType;
		this->depth_ = src.depth;
		this->size_ = src.size;

		src.bytesPerLine_ = 0;
		src.dataType_ = DataType::UNDEFINED;
		src.depth_ = 0;
		src.size_ = { 0, 0 };

		return *this;
	}

	ImageFrame::ImageFrame(DataType ty, const Size2D<SizeType> &sz, SizeType d) :
		ImageFrame()
	{
		this->Reset(ty, sz, d);
	}

	ImageFrame::ImageFrame(DataType ty, const std::vector<ByteType> &srcData,
		const Size2D<SizeType> &sz, SizeType d) : ImageFrame()
	{
		this->CopyFrom(ty, srcData, sz, d);
	}

	ImageFrame::ImageFrame(DataType ty, std::vector<ByteType> &&srcData,
		const Size2D<SizeType> &sz, SizeType d) : ImageFrame()
	{
		this->MoveFrom(ty, std::move(srcData), sz, d);
	}

	// Constructors.
	////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////
	// Accessors.

	ImageFrame::Iterator ImageFrame::Begin(const Point2D<SizeType> &pt)
	{
		if (pt == Point2D<SizeType>{ 0, 0 })
			return this->data_.begin();
		else
		{
			this->EvalPosition(pt);
			return this->data_.begin() + this->GetOffset(pt);
		}
	}

	ImageFrame::ConstIterator ImageFrame::Cbegin(const Point2D<SizeType> &pt) const
	{
		if (pt == Point2D<SizeType>{ 0, 0 })
			return this->data.cbegin();
		else
		{
			this->EvalPosition(pt);
			return this->data.cbegin() + this->GetOffset(pt);
		}
	}

	ImageFrame::SizeType ImageFrame::GetOffset(const Point2D<SizeType> &pt) const
	{
		auto bytes_line = ImageFrame::GetBytesPerLine(this->dataType, this->size.width,
			this->depth);
		return bytes_line * pt.y + GetNumBytes(this->dataType) * this->depth * pt.x;
	}

	// Accessors.
	////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////
	// Methods.

	void ImageFrame::Clear()
	{
		this->data_.clear();

		this->bytesPerLine_ = 0;
		this->dataType_ = DataType::UNDEFINED;
		this->depth_ = 0;
		this->size_ = { 0, 0 };
	}

	void ImageFrame::CopyFrom(DataType ty, const std::vector<ByteType> &srcData,
		const Size2D<SizeType> &sz, SizeType d)
	{
		ImageFrame::EvalSize(ty, srcData.size(), sz.width, sz.height, d);

		this->data_ = srcData;

		this->bytesPerLine_ = ImageFrame::GetBytesPerLine(ty, sz.width, d);
		this->dataType_ = ty;
		this->depth_ = d;
		this->size_ = sz;
	}

	void ImageFrame::CopyFrom(DataType ty, const ByteType *src, const Size2D<SizeType> &sz,
		SizeType d, SizeType stepBytes)
	{
		auto bytes_line = ImageFrame::GetBytesPerLine(ty, sz.width, d);
		auto bytes_total = sz.height * bytes_line;

		if (bytes_line == stepBytes)
			this->data_.assign(src, src + bytes_total);
		else
		{
			if (this->data.size() != bytes_total)
				this->data_.resize(bytes_total);
			auto itDst = this->data_.begin();
			for (auto H = 0; H != sz.height; ++H, src += stepBytes, itDst += bytes_line)
				std::copy_n(src, sz.width * d * GetNumBytes(ty), itDst);
		}

		this->bytesPerLine_ = bytes_line;
		this->dataType_ = ty;
		this->depth_ = d;
		this->size_ = sz;
	}

	ImageFrame ImageFrame::CopyTo(const ROI &roiSrc) const
	{
		// Check ROI.
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
			Utilities::CopyLines(itSrc, this->bytesPerLine, itDst, imgDst.bytesPerLine, bytes_line_roi,
				roiSrc.size.height);
		}

		return imgDst;
	}

	void ImageFrame::EvalPosition(const Point2D<SizeType> &pt) const
	{
		this->EvalRoi(pt, { 1, 1 });
	}

	void ImageFrame::EvalRoi(const ROI &roi) const
	{
		this->EvalRoi(roi.origin, roi.size);
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

	ImageFrame::SizeType ImageFrame::GetBytesPerLine(DataType ty, SizeType w, SizeType d)
	{
		return w * d * GetNumBytes(ty) + 0;
	}

	void ImageFrame::MoveFrom(DataType ty, std::vector<ByteType> &&srcData,
		const Size2D<SizeType> &sz, SizeType d)
	{
		ImageFrame::EvalSize(ty, srcData.size(), sz.width, sz.height, d);

		this->data_ = std::move(srcData);

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
