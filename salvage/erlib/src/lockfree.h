#ifndef ER_HEADER_LOCKFREE_H
#define ER_HEADER_LOCKFREE_H

template <class T> struct LFNode {
	T value;
	LFNode<T> * volatile next;

	LFNode() : value(), next(NULL) {}
	LFNode( T v ) : value(v), next(NULL) {}
};

template <class T> class LFStack {
	LFNode<T> *volatile head;
	volatile unsigned int pops;

public:

	void Push( LFNode<T> *node );
	LFNode<T> *Pop();

	LFStack() : head(NULL), pops(0) {}
};

bool CAS( unsigned int *ptr, unsigned int ov, unsigned int nv ) {
	if ( *ptr == ov ) {
		*ptr = nv;
		return true;
	}
	return false;
}

bool CAS2( unsigned int *ptr, unsigned int ov0, unsigned int ov1, unsigned int nv0, unsigned int nv1 ) {
	if ( *ptr == ov0 ) {
		*ptr = nv0;
		return true;
	}
	return false;
}

template <class T> void LFStack<T>::Push( LFNode<T> *node ) {
	while ( true ) {
		node->next = head;
		if ( CAS((unsigned int*)&head, (unsigned int)node->next, (unsigned int)node) ) {
			break;
		}
	}
}

template <class T> LFNode<T> * LFStack<T>::Pop() {
	while ( true ) {
		LFNode<T> *h = head;
		unsigned int p = pops;

		if ( NULL == h ) {
			return NULL;
		}

		LFNode<T> *next = h->next;
		if ( CAS2( &head, h, p, next, p+1 ) ) {
			return h;
		}
	}
}


#if 0

#ifndef THREAD_LOCKFREE_DE_QUEUE_H
#define THREAD_LOCKFREE_DE_QUEUE_H

#include "core/Atomic.h"
#include "thread/CAS.h"
#if	RDE_DEBUG
#	include "core/Thread.h"
#endif

namespace rde
{

// Double-ended queue, bounded.
// Single producer, multiple consumers.
// Holds pointers to T, not Ts themselves.
template<typename T>
class LockFreeDEQueue
{
public:
	// Creates queue able to hold up to 
	// "maxCapacity" items at given moment.
	explicit LockFreeDEQueue(int maxCapacity)
    {
		m_data = new T*[maxCapacity];
        m_top = 0;
        m_topCounter = 0;
        m_bottom = 0;
#if RDE_DEBUG
		m_producerThreadId = -1;
#endif
	}
    ~LockFreeDEQueue()
    {
		delete[] m_data;
        m_data = 0;
	}
    void PushBottom(T* t)
    {
#if RDE_DEBUG
		if (m_producerThreadId == -1)
			m_producerThreadId = Thread::GetCurrentThreadId();
		RDE_ASSERT(m_producerThreadId == Thread::GetCurrentThreadId());
#endif
		int32_t bottom = m_bottom;
		m_data[bottom] = t;
        Interlocked::ReadWriteBarrier();
		// all stores done here.
        m_bottom = bottom + 1;
	}
    // Can be called from multiple threads.
    T* PopTop()
    {
		int32_t topCounter = m_topCounter;
        int32_t top = m_top;

        // Empty queue.
        if (m_bottom <= top)
			return 0;

		Interlocked::ReadWriteBarrier();
		T* t = m_data[top];
        if (CAS2(&m_top, top, topCounter, top + 1, topCounter + 1))
			return t;

		// CAS2 failed
        return 0;
	}
    // Only called from one thread.
    T* PopBottom()
    {
		if (m_bottom == 0)      // Empty queue
			return 0;

#if RDE_DEBUG
		RDE_ASSERT(m_producerThreadId == -1 || 
			m_producerThreadId == Thread::GetCurrentThreadId());
#endif

        --m_bottom;
        Interlocked::ReadWriteBarrier();
        T* t = m_data[m_bottom];
        int32_t topCounter = m_topCounter;
        int32_t top = m_top;

        // No danger of conflict, just return.
        if (m_bottom > top)
			return t;
        // Possible conflict, slow-path.
        if (m_bottom == top)
        {
			m_bottom = 0;
            if (CAS2(&m_top, top, topCounter, 0, topCounter + 1))
				return t;
        }
        FetchAndStore(&m_top, 0, topCounter + 1);
        return 0;
    }

private:
	RDE_FORBID_COPY(LockFreeDEQueue);

	T* volatile*		m_data;
    int32_t volatile   m_bottom; 
    int32_t volatile	m_top;
    int32_t volatile	m_topCounter;
#if RDE_DEBUG
	int					m_producerThreadId;
#endif
};
} // rde

#endif
#endif


#endif