#pragma once
#include "Buffer.h"
#include "LinearAllocator.h"

#include "export.h"

template<class T>
class SubmitBuffer
{
public:
	SubmitBuffer(int size) :
		m_allocator(size), m_buffer(size, BufferOptions(false, false, false, false, true, false))
	{
	}

	MemoryAllocation Allocate(int size)
	{
		return m_allocator.Allocate(size);
	}

	void Destroy()
	{
		m_buffer.Destroy();
	}

	void UploadData(MemoryAllocation alloc, T *data, int offset, int size)
	{
		m_buffer.CopyData(data, offset + alloc.GetPosition(), size);
	}

	void Release(MemoryAllocation alloc)
	{
		m_allocator.Deallocate(alloc);
	}

	inline Buffer<T> GetBuffer() { return m_buffer; }
private:
	LinearAllocator m_allocator;
	Buffer<T> m_buffer;
};

