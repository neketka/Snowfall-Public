#include "LocalAssetStreamSource.h"

LocalAssetStreamSource::LocalAssetStreamSource(std::string path) : m_path(path)
{
}

void LocalAssetStreamSource::OpenStream()
{
	m_stream = std::ifstream(m_path, std::ifstream::binary);
	m_stream.seekg(0, std::ios::end);
	m_length = static_cast<int>(m_stream.tellg());
	m_stream.seekg(0);
}

void LocalAssetStreamSource::CloseStream()
{
	m_stream.close();
}

void LocalAssetStreamSource::ReadStream(char *buffer, int length)
{
	m_stream.read(buffer, length);
}

int LocalAssetStreamSource::GetStreamLength()
{
	return m_length;
}

int LocalAssetStreamSource::GetStreamPosition()
{
	return static_cast<int>(m_stream.tellg());
}

void LocalAssetStreamSource::SeekStream(int position)
{
	m_stream.seekg(position);
}
