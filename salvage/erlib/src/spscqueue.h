#ifndef ER_HEADER_SPSCQUEUE_H
#define ER_HEADER_SPSCQUEUE_H

#include "erbase.h"

#if defined(ERLIB_OS_WIN)
#include "windows.h"
#endif

//http://msinilo.pl/blog/

// Single producer/single consumer, bounded FIFO queue.
template<typename T, size_t TCapacity>
class SPSCQueue
{
	// TCapacity must be power-of-two
	typedef char ERR_CapacityNotPowerOfTwo[((TCapacity & (TCapacity - 1)) == 0 ? 1 : -1)];

public:
	SPSCQueue()
	:	m_pushIndex(0),
		m_popIndex(0)
	{
		// NOTE: for smaller queues we can consider using static buffer.
		m_buffer = new T[TCapacity];
	}
	~SPSCQueue()
	{
		delete[] m_buffer;
	}

	uint32_t Size() const
	{
		const uint32_t pushed = m_pushIndex;
		const uint32_t popped = m_popIndex;
		return pushed - popped;
	}
	bool IsFull() const
	{
		const uint32_t s = Size();
		// It's OK on overflow -> it's not full in such case.
		// (more pushed than popped if it overflows).
		return s >= TCapacity;
	}

	void Push(const T& t)
	{
		const uint32_t pushIndex = m_pushIndex;
		const uint32_t index = pushIndex & (TCapacity - 1);

		m_buffer[index] = t;
		MemoryBarrier();
		// Publish data in buffer before increasing index.
		Write32(&m_pushIndex, pushIndex + 1);
	}
	void Pop(T& t)
	{
		const uint32_t popIndex = m_popIndex;
		const uint32_t index = popIndex & (TCapacity - 1);

		t = m_buffer[index];
		MemoryBarrier();
		// Make sure data is full copied from buffer before publishing pop.
		Write32(&m_popIndex, popIndex + 1);
	}

	uint32_t PushIndex() {
		return m_pushIndex;
	}

	uint32_t PopIndex() {
		return m_popIndex;
	}

private:
	SPSCQueue(const SPSCQueue&);
	SPSCQueue& operator=(const SPSCQueue&);

	static void Write32(volatile uint32_t* ptr, uint32_t v)
	{
		*ptr = v;
	}

	uint32_t	m_pushIndex;
	uint32_t	m_popIndex;
	T*			m_buffer;
};


#endif
