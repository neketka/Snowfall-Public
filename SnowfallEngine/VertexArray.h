#pragma once
#include <vector>
#include <GL\glew.h>
#include "Buffer.h"

#include "export.h"

class Attribute
{
public:
	Attribute(int index, int offset, int length, int size, bool isFloat, int instanceDivisor=0) : 
		Index(index), Offset(offset), Length(length), Size(size), IsFloat(isFloat), InstanceDivisor(instanceDivisor) {}
	int Index;
	int Offset;
	int Length;
	int Size;
	int InstanceDivisor;
	bool IsFloat;
};

class BufferStructure
{
public:
	BufferStructure(std::vector<Attribute> attributes) : m_attributes(attributes) 
	{
		m_totalSize = 0;
		for (Attribute att : attributes)
			m_totalSize += att.Size;
	}
	inline std::vector<Attribute> GetAttributes() { return m_attributes; }
	inline int GetTotalSize() { return m_totalSize; }
private:
	std::vector<Attribute> m_attributes;
	int m_totalSize;
};

class VertexArray : public IGLResource
{
public:
	SNOWFALLENGINE_API VertexArray() {}
	SNOWFALLENGINE_API VertexArray(std::vector<BufferStructure> structures);
	SNOWFALLENGINE_API void SetIndexBuffer(Buffer<int> buffer);
	inline GLuint GetID() { return m_id; }
	SNOWFALLENGINE_API virtual void Destroy() override;
	SNOWFALLENGINE_API void SetBuffer(int structure, TBuffer buffer, int offset = 0);
	SNOWFALLENGINE_API void SetAttributeEnabled(int index, bool enabled);
	SNOWFALLENGINE_API void SetStructureEnabled(int structure, bool enabled);
private:
	std::vector<BufferStructure> m_structures;
	int m_length;
	bool m_hasIndex;
	GLuint m_id;
};
