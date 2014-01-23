#if !defined(BUFFER_H)
#define BUFFER_H

#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>

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
		bool try_pop(ImageFrame &imgDst,
			const std::chrono::seconds &wait_time = std::chrono::seconds(3));
	protected:
		std::size_t capacity_ = 0;
		ImageFrame *data_ = nullptr;

		////////////////////////////////////////////////////////////////////////////////////
		// Real-time dimension information.
		// Declared as atomic to be thread safe.
		std::atomic_size_t back_ = 0;
		std::atomic_size_t count_ = 0;
		std::atomic_size_t front_ = 0;

		////////////////////////////////////////////////////////////////////////////////////
		// Thread safe locks.
		std::mutex mutex_;
		std::condition_variable not_empty_;
		std::condition_variable not_full_;
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

	// The internally used lock must be a std::unique_lock to use std::condition_variable.
	void ImageBuffer::push(ImageFrame &&imgSrc)
	{
		// Wait indefinitely if the buffer is full.		
		std::unique_lock<std::mutex> lock(this->mutex_);
		this->not_full_.wait(lock, [this](){
			return this->count_.load() != this->capacity_; });

		// Move data instead of copying.
		this->data_[this->back_.load()] = std::move(imgSrc);

		// Update counter.
		this->back_ = (this->back_.load() + 1) % this->capacity_;
		++this->count_;

		// Notify that buffer is not empty.
		this->not_empty_.notify_one();
	}

	bool ImageBuffer::try_pop(ImageFrame &imgDst, const std::chrono::seconds &wait_time)
	{
		// Wait for given wait time if buffer is empty.
		std::unique_lock<std::mutex> lock(this->mutex_);
		//this->not_empty_.wait(lock, [this](){ return this->count_.load() != 0; });
		auto now = std::chrono::system_clock::now();
		if (this->not_empty_.wait_until(lock, now + wait_time,
			[this](){ return this->count_.load() != 0; }))
		{
			// Move data instead of copying to a temporary variable.
			imgDst = std::move(this->data_[this->front_.load()]);

			// Update counter.
			this->front_ = (this->front_.load() + 1) % this->capacity_;
			--this->count_;

			// Notify that buffer is not full.
			this->not_full_.notify_one();

			return true;
		}
		else
			return false;	// timed out.
	}

}
#endif
