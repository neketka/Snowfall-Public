#include "stdafx.h"
#include "MemoryAssetStream.h"

MemoryAssetStream::MemoryAssetStream(std::vector<char>& memory) : m_mem(&memory)
{
}

void MemoryAssetStream::OpenStreamRead()
{
}

void MemoryAssetStream::OpenStreamWrite(bool overwrite)
{
	if (overwrite)
		m_position = 0;
}

void MemoryAssetStream::CloseStream()
{
}

void MemoryAssetStream::WriteStreamBytes(char *buffer, int length)
{
	if (m_mem->size() - m_position - 1 < length)
		m_mem->reserve(m_mem->size() + length - m_position - 1);
	memcpy(m_mem->data(), buffer, glm::min<int>(length, m_mem->size() - m_position - 1));
}

void MemoryAssetStream::ReadStreamBytes(char *buffer, int length)
{
	memcpy(buffer, m_mem->data(), glm::min<int>(length, m_mem->size() - m_position - 1));
}

bool MemoryAssetStream::CanRead()
{
	return true;
}

bool MemoryAssetStream::CanWrite()
{
	return true;
}

void MemoryAssetStream::SeekStream(int position)
{
	m_position = position;
}

int MemoryAssetStream::GetStreamPosition()
{
	return m_position;
}

int MemoryAssetStream::GetStreamLength()
{
	return m_mem->size();
}
