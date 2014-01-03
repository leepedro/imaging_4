#if !defined(IMAGE_H)
#define IMAGE_H

#include <vector>

#include "coordinates.h"

namespace Imaging
{
	////////////////////////////////////////////////////////////////////////////////////////
	// Forward declarations.

	//class ImageFrame;

	// Forward declarations.
	////////////////////////////////////////////////////////////////////////////////////////

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

	template <typename T>
	inline DataType GetDataType(void)
	{
		static_assert(false, "Unsupported data type.");
		throw std::logic_error("This function must not be called as it is disabled.");
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

	std::size_t GetNumBytes(DataType ty);

	/* Pixel-based bitmap (raster) image.

	This class template stores image data as a std::vector<byte>, so it is equivalent to raw
	pointer and capable of padding bytes if necessary. The byte unit type can be either
	{char, unsigned char, signed char}.
	The image data can be accessed through iterators by accessing methods.
	The dimension of image data can be changed during the runtime through resizing the
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

		////////////////////////////////////////////////////////////////////////////////////
		// Default constructors.
		ImageFrame(void) = default;
		ImageFrame(const ImageFrame &src);
		ImageFrame(ImageFrame &&src);
		ImageFrame &operator=(const ImageFrame &src);
		ImageFrame &operator=(ImageFrame &&src);

		////////////////////////////////////////////////////////////////////////////////////
		// Custom constructors.
		ImageFrame(DataType ty, const Size2D<SizeType> &sz, SizeType d = 1);
		ImageFrame(DataType ty, const std::vector<ByteType> &srcData,
			const Size2D<SizeType> &sz, SizeType d = 1);
		ImageFrame(DataType ty, std::vector<ByteType> &&srcData,
			const Size2D<SizeType> &sz, SizeType d = 1);

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
		void CopyFrom(DataType ty, const std::vector<ByteType> &srcData,
			const Size2D<SizeType> &sz, SizeType d = 1);

		/* Copies image data from a raw pointer.
		The source data may have its own padding bytes.
		Destination is resized per the size of source data.
		NOTE: Since there is no way to check the dimension of source data, users must ensure
		that the size of source data is correct. */
		void CopyFrom(DataType ty, const ByteType *src, const Size2D<SizeType> &sz,
			SizeType d, SizeType stepBytes);

		/* Creates a seprate ImageFrame object with the image data within the ROI.
		Destination is resized per the size of source data. */
		ImageFrame CopyTo(const ROI &roiSrc) const;

		static SizeType GetBytesPerLine(DataType ty, SizeType w, SizeType d);

		/* Moves image data from an std::vector<byte> with an identical allocation scheme.
		Destination is resized per the size of source data. */
		void MoveFrom(DataType ty, std::vector<ByteType> &&srcData,
			const Size2D<SizeType> &sz, SizeType d = 1);

		template <typename T>
		void Reset(const Size2D<SizeType> &sz, SizeType d = 1, T value = 0);
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


	template <typename T>
	void ImageFrame::Reset(const Size2D<SizeType> &sz, SizeType d, T value)
	{
		// Compute memory requirement.
		DataType ty = GetDataType<T>();
		auto bytes_line = ImageFrame::GetBytesPerLine(ty, sz.width, d);
		auto bytes_total = sz.height * bytes_line;

		// Memory re-allocation.
		if (this->data.size() != bytes_total)
			this->data_.resize(bytes_total, value);

		// Update dimension.
		this->bytesPerLine_ = bytes_line;
		this->dataType_ = ty;
		this->depth_ = d;
		this->size_ = sz;
	}
}

#endif
