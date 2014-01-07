#if !defined(IMAGE_H)
#define IMAGE_H

#include <vector>

#include "coordinates.h"

namespace Imaging
{
	/* Static member function.

	If a function does not use any class member (if there is no 'this->' within the function
	definition), declare the function as a static member or non-member function. */

	////////////////////////////////////////////////////////////////////////////////////
	// DataType and functions using DataType.

	// Data type supported as image data.
	enum struct DataType
	{
		UNDEFINED,
		CHAR,
		SCHAR,
		UCHAR,
		SHORT,
		USHORT,
		INT,
		UINT,
		LONGLONG,
		ULONGLONG,
		FLOAT,
		DOUBLE
	};

	////////////////////////////////////////////////////////////////////////////////////
	// GetDataType<T>()

	template <typename T>
	inline DataType GetDataType(void)
	{
		return DataType::UNDEFINED;
	}

	template <>
	inline DataType GetDataType<char>(void)
	{
		return DataType::CHAR;
	}

	template <>
	inline DataType GetDataType<signed char>(void)
	{
		return DataType::SCHAR;
	}

	template <>
	inline DataType GetDataType<unsigned char>(void)
	{
		return DataType::UCHAR;
	}

	template <>
	inline DataType GetDataType<short>(void)
	{
		return DataType::SHORT;
	}

	template <>
	inline DataType GetDataType<unsigned short>(void)
	{
		return DataType::USHORT;
	}

	template <>
	inline DataType GetDataType<int>(void)
	{
		return DataType::INT;
	}

	template <>
	inline DataType GetDataType<unsigned int>(void)
	{
		return DataType::UINT;
	}

	template <>
	inline DataType GetDataType<long long>(void)
	{
		return DataType::LONGLONG;
	}

	template <>
	inline DataType GetDataType<unsigned long long>(void)
	{
		return DataType::ULONGLONG;
	}

	template <>
	inline DataType GetDataType<float>(void)
	{
		return DataType::FLOAT;
	}

	template <>
	inline DataType GetDataType<double>(void)
	{
		return DataType::DOUBLE;
	}

	// GetDataType<T>()
	////////////////////////////////////////////////////////////////////////////////////

	std::size_t GetNumBytes(DataType ty);

	// DataType and functions using DataType.
	////////////////////////////////////////////////////////////////////////////////////


	/* Pixel-based bitmap (raster) image.

	This class template stores image data as a std::vector<byte>, so it is equivalent to raw
	pointer and capable of padding bytes if necessary. The byte unit type can be either
	{char, unsigned char, signed char}.
	The image data can be accessed through iterators by accessing methods.
	The dimension of image data can be changed during the runtime by resizing the
	std::vector<byte>.

	Since the data type of images are usually known after loading an image file, which is
	runtime instead of compile time, template based	image class is not a practical design.
	The data type of image data is determined by a enum struct DataType, and the data type of
	an image can be changed during the runtime.

	Image pixel values are always stored in the following order.
	channel -> pixel -> line -> frame

	The terms used to describe the dimension of data are following.
	depth: number of channels per pixel
	width: number of pixels per line
	height: number of lines per frame
	length: number of frames per block (not used in this class)
	c: position of a channel at given pixel; [0 ~ depth)
	x: position of a pixel at given line; [0 ~ width)
	y: position of a line at given frame; [0 ~ height) */

	class ImageFrame
	{
	public:
		////////////////////////////////////////////////////////////////////////////////////
		// Types and constants.

		typedef char ByteType;
		// for pixel coordinate and raw (byte) data position.
		typedef std::vector<ByteType>::size_type SizeType;
		typedef std::vector<ByteType>::iterator Iterator;
		typedef std::vector<ByteType>::const_iterator ConstIterator;
		typedef RectTypeB<SizeType, SizeType> ROI;

		// Types and constants.
		////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////
		// Default constructors.
		ImageFrame(void) = default;
		ImageFrame(const ImageFrame &src);
		ImageFrame(ImageFrame &&src);
		ImageFrame &operator=(const ImageFrame &src);
		ImageFrame &operator=(ImageFrame &&src);
		// Default constructors.
		////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////
		// Custom constructors.
		ImageFrame(DataType ty, const Size2D<SizeType> &sz, SizeType d = 1);
		ImageFrame(const std::vector<ByteType> &srcData, DataType ty,
			const Size2D<SizeType> &sz, SizeType d = 1);
		ImageFrame(std::vector<ByteType> &&srcData, DataType ty,
			const Size2D<SizeType> &sz, SizeType d = 1);
		// Custom constructors.
		////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////
		// Accessors.
		Iterator Begin(const Point2D<SizeType> &pt = { 0, 0 });
		ConstIterator Cbegin(const Point2D<SizeType> &pt = { 0, 0 }) const;

		const SizeType &bytesPerLine = this->bytesPerLine_;
		const std::vector<ByteType> &data = this->data_;
		const DataType &dataType = this->dataType_;
		const SizeType &depth = this->depth_;
		const Size2D<SizeType> &size = this->size_;

		////////////////////////////////////////////////////////////////////////////////////
		// Methods.
		void Clear(void);

		/* Copies image data from an std::vector<byte> with an identical allocation scheme.
		Destination is resized per the size of source data. */
		void CopyFrom(const std::vector<ByteType> &srcData, DataType ty,
			const Size2D<SizeType> &sz, SizeType d = 1);

		/* Copies image data from a raw pointer.
		The source data may have its own padding bytes.
		Destination is resized per the size of source data.
		NOTE: Since there is no way to check the dimension of source data, users must ensure
		that the size of source data is correct. */
		void CopyFrom(const ByteType *src, DataType ty, const Size2D<SizeType> &sz,
			SizeType d, SizeType stepBytes);

		/* Creates a seprate ImageFrame object with the image data within the ROI.
		Destination is resized per the size of source data. */
		ImageFrame CopyTo(const ROI &roiSrc) const;

		static SizeType GetBytesPerLine(DataType ty, SizeType w, SizeType d);

		bool HaveZeroPaddingBytes(void) const;

		/* Moves image data from an std::vector<byte> with an identical allocation scheme.
		Destination is resized per the size of source data. */
		void MoveFrom(std::vector<ByteType> &&srcData, DataType ty,
			const Size2D<SizeType> &sz, SizeType d = 1);

		template <typename T>
		void Reset(const Size2D<SizeType> &sz, SizeType d = 1);
		void Reset(DataType ty, const Size2D<SizeType> &sz, SizeType d = 1);

	protected:
		////////////////////////////////////////////////////////////////////////////////////
		// Accessors.
		SizeType GetOffset(const Point2D<SizeType> &pt) const;

		////////////////////////////////////////////////////////////////////////////////////
		// Methods.
		void EvalPosition(const Point2D<SizeType> &pt) const;
		void EvalRoi(const ROI &roi) const;
		void EvalRoi(const Point2D<SizeType> &orgn, const Size2D<SizeType> &sz) const;
		static void EvalSize(DataType ty, SizeType length, SizeType w, SizeType h, SizeType d);

		////////////////////////////////////////////////////////////////////////////////////
		// Data.
		SizeType bytesPerLine_ = 0;
		std::vector<ByteType> data_;
		DataType dataType_ = DataType::UNDEFINED;
		SizeType depth_ = 0;
		Size2D<SizeType> size_ = Size2D<SizeType>(0, 0);
	};

	// Custom name for frequently used classes.
	typedef ImageFrame::SizeType ImageSizeType;
	typedef Size2D<ImageSizeType> ImageSize;


}

//#include "image_inl.h"

namespace Imaging
{
	////////////////////////////////////////////////////////////////////////////////////////
	// ImageFrame

	////////////////////////////////////////////////////////////////////////////////////////
	// Constructors.

	inline ImageFrame::ImageFrame(const ImageFrame &src) : ImageFrame()
	{
		*this = src;
	}

	inline ImageFrame::ImageFrame(ImageFrame &&src) : ImageFrame()
	{
		*this = std::move(src);
	}

	inline ImageFrame::ImageFrame(DataType ty, const Size2D<SizeType> &sz, SizeType d) :
		ImageFrame()
	{
		this->Reset(ty, sz, d);
	}

	inline ImageFrame::ImageFrame(const std::vector<ByteType> &srcData, DataType ty,
		const Size2D<SizeType> &sz, SizeType d) : ImageFrame()
	{
		this->CopyFrom(srcData, ty, sz, d);
	}

	inline ImageFrame::ImageFrame(std::vector<ByteType> &&srcData, DataType ty,
		const Size2D<SizeType> &sz, SizeType d) : ImageFrame()
	{
		this->MoveFrom(std::move(srcData), ty, sz, d);
	}

	// Constructors.
	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////
	// Accessors.

	inline ImageFrame::Iterator ImageFrame::Begin(const Point2D<SizeType> &pt)
	{
		if (pt == Point2D<SizeType>{ 0, 0 })
			return this->data_.begin();
		else
		{
			this->EvalPosition(pt);
			return this->data_.begin() + this->GetOffset(pt);
		}
	}

	inline ImageFrame::ConstIterator ImageFrame::Cbegin(const Point2D<SizeType> &pt) const
	{
		if (pt == Point2D<SizeType>{ 0, 0 })
			return this->data.cbegin();
		else
		{
			this->EvalPosition(pt);
			return this->data.cbegin() + this->GetOffset(pt);
		}
	}

	inline ImageFrame::SizeType ImageFrame::GetOffset(const Point2D<SizeType> &pt) const
	{
		auto bytes_line = ImageFrame::GetBytesPerLine(this->dataType, this->size.width,
			this->depth);
		return bytes_line * pt.y + GetNumBytes(this->dataType) * this->depth * pt.x;
	}

	// Accessors.
	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////
	// Methods.

	inline void ImageFrame::EvalPosition(const Point2D<SizeType> &pt) const
	{
		this->EvalRoi(pt, { 1, 1 });
	}

	inline void ImageFrame::EvalRoi(const ROI &roi) const
	{
		this->EvalRoi(roi.origin, roi.size);
	}

	// static.
	inline ImageFrame::SizeType ImageFrame::GetBytesPerLine(DataType ty, SizeType w,
		SizeType d)
	{
		return w * d * GetNumBytes(ty) + 0;
	}

	inline bool ImageFrame::HaveZeroPaddingBytes(void) const
	{
		auto bytes_line = this->size.width * this->depth * GetNumBytes(this->dataType);
		return bytes_line == ImageFrame::GetBytesPerLine(this->dataType, this->size.width,
			this->depth);
	}

	template <typename T>
	void ImageFrame::Reset(const Size2D<SizeType> &sz, SizeType d)
	{
		// Compute memory requirement.
		DataType ty = GetDataType<T>();
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
	////////////////////////////////////////////////////////////////////////////////////////

	// ImageFrame
	////////////////////////////////////////////////////////////////////////////////////////
}
#endif
