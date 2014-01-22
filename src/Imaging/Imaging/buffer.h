#if !defined(BUFFER_H)
#define BUFFER_H

#include <mutex>
#include <thread>
#include <condition_variable>

#include "image.h"

namespace Imaging
{
	/* Thread safe image queue buffer */
	class ImageBuffer
	{
	public:
		ImageBuffer(std::size_t capacity);
		~ImageBuffer(void);
		void push(ImageFrame &&imgSrc);
		ImageFrame pop(void);
	protected:
		ImageFrame *data_ = nullptr;
		std::size_t capacity_ = 0;
		std::size_t front_ = 0;
		std::size_t back_ = 0;
		std::size_t count_ = 0;
		std::condition_variable not_full_;
		std::condition_variable not_empty_;
		std::mutex lock_;
	};
}

namespace Imaging
{
	ImageBuffer::ImageBuffer(std::size_t capacity) : capacity_(capacity)
	{
		this->data_ = new ImageFrame[this->capacity_];
	}

	ImageBuffer::~ImageBuffer(void)
	{
		delete[] this->data_;
	}

	void ImageBuffer::push(ImageFrame &&imgSrc)
	{
		// Wait if buffer is full.
		std::unique_lock<std::mutex> l(this->lock_);
		this->not_full_.wait(l, [this](){ return this->count_ != this->capacity_; });

		// Move data instead of copying.
		this->data_[this->back_] = std::move(imgSrc);

		// Update counter.
		this->back_ = (this->back_ + 1) % this->capacity_;
		++this->count_;

		// Notify that buffer is not empty.
		this->not_empty_.notify_one();
	}

	ImageFrame ImageBuffer::pop(void)
	{
		// Wait if buffer is empty.
		std::unique_lock<std::mutex> l(this->lock_);
		this->not_empty_.wait(l, [this](){ return this->count_ != 0; });

		// Move data instead of copying to a temporary variable.
		ImageFrame result = std::move(this->data_[this->front_]);

		// Update counter.
		this->front_ = (this->front_ + 1) % this->capacity_;
		--this->count_;

		// Notify that buffer is not full.
		this->not_full_.notify_one();

		// Return the temporary variable.
		return result;
	}

}
#endif
