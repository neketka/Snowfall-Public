#pragma once
#include <GL\glew.h>
#include <vector>
#include <memory.h>
#include "GLResource.h"

#include "export.h"

class BufferOptions
{
public:
	bool MapRead;
	bool MapWrite;
	bool MapPersistent;
	bool MapCoherent;
	bool DynamicStorage;
	bool ClientStorage;
	GLenum GetEnum()
	{
		GLenum qualifiers = GL_NONE;
		if (MapRead)
			qualifiers |= GL_MAP_READ_BIT;
		if (MapWrite)
			qualifiers |= GL_MAP_WRITE_BIT;
		if (MapPersistent)
			qualifiers |= GL_MAP_PERSISTENT_BIT;
		if (MapCoherent)
			qualifiers |= GL_MAP_COHERENT_BIT;
		if (DynamicStorage)
			qualifiers |= GL_DYNAMIC_STORAGE_BIT;
		if (ClientStorage)
			qualifiers |= GL_CLIENT_STORAGE_BIT;
		return qualifiers;
	}
	BufferOptions(bool mapRead, bool mapWrite, bool mapPersistent, bool mapCoherent, bool dynamic, bool client)
	 : MapRead(mapRead), MapWrite(mapWrite), MapPersistent(mapPersistent), MapCoherent(mapCoherent), DynamicStorage(dynamic),
		ClientStorage(client) {}
};

class MappingOptions
{
public:
	bool MapRead;
	bool MapWrite;
	bool MapPersistent;
	bool MapCoherent;
	bool FlushExplicit;
	bool InvalidateRange;
	bool InvalidateBuffer;
	bool Unsynchronized;
	GLenum GetEnum()
	{
		GLenum qualifiers = GL_NONE;
		if (MapRead)
			qualifiers |= GL_MAP_READ_BIT;
		if (MapWrite)
			qualifiers |= GL_MAP_WRITE_BIT;
		if (MapPersistent)
			qualifiers |= GL_MAP_PERSISTENT_BIT;
		if (MapCoherent)
			qualifiers |= GL_MAP_COHERENT_BIT;
		if (FlushExplicit)
			qualifiers |= GL_MAP_FLUSH_EXPLICIT_BIT;
		if (InvalidateRange)
			qualifiers |= GL_MAP_INVALIDATE_RANGE_BIT;
		if (InvalidateBuffer)
			qualifiers |= GL_MAP_INVALIDATE_BUFFER_BIT;
		if (Unsynchronized)
			qualifiers |= GL_MAP_UNSYNCHRONIZED_BIT;
		return qualifiers;
	}

	MappingOptions(bool mapRead, bool mapWrite, bool mapPersistent, bool mapCoherent, bool flush, bool invalidateRange,
		bool invalidateBuffer, bool unsynchronized)
		: MapRead(mapRead), MapWrite(mapWrite), MapPersistent(mapPersistent), MapCoherent(mapCoherent), FlushExplicit(flush),
		InvalidateRange(invalidateRange), InvalidateBuffer(invalidateBuffer), Unsynchronized(unsynchronized) {}
};

class SNOWFALLENGINE_API TBuffer : public IGLResource
{
public:
	inline GLuint GetID() { return m_id; }
	inline int GetSize() { return m_length; }
	inline int GetTSize() { return m_tsize; }
	virtual void Destroy() override;
	void UnmapBuffer();
	bool operator==(const TBuffer& other) const { return m_id == other.m_id; }
protected:
	TBuffer(void *data, int tsize, int length, BufferOptions options);
	TBuffer() {}
	void CopyData(void *data, int offset, int length);
	void CopyBufferData(int offset, int size, TBuffer dest, int destOffset);
	void FlushBuffer(int offset, int length);
	void MapBuffer(int offset, int length, MappingOptions options);

	inline void *GetMappedPointer() { return m_mappingPtr; }
private:
	GLuint m_id;
	void *m_mappingPtr;
	int m_length;
	int m_tsize;
};

template<class T>
class Buffer : public TBuffer
{
public:
	Buffer(int size, BufferOptions options)
		: TBuffer(nullptr, sizeof(T), size, options){}

	Buffer(T *data, int length, BufferOptions options)
		: TBuffer(data, sizeof(T), length, options) {}

	Buffer(std::vector<T> data, BufferOptions options)
		: TBuffer(data.data(), sizeof(T), static_cast<int>(data.size()), options) {}

	Buffer() {}

	void CopyData(std::vector<T> data, int offset)
	{
		TBuffer::CopyData(data.data(), static_cast<int>(offset), static_cast<int>(data.size()));
	}
	
	void CopyData(T *data, int offset, int length)
	{
		TBuffer::CopyData(data, offset, length);
	}

	void CopyBufferData(int offset, int length, Buffer<T> dest, int destOffset)
	{
		TBuffer::CopyBufferData(offset, length, dest, destOffset);
	}

	void MapBuffer(int offset, int length, MappingOptions options)
	{
		TBuffer::MapBuffer(offset, length, options);
	}

	void FlushBuffer(int offset, int length)
	{
		TBuffer::FlushBuffer(offset, length);
	}

	inline int GetLength() { return GetSize(); }

	inline T *GetMappedPointer() { return static_cast<T *>(TBuffer::GetMappedPointer()); }
};
