#include "stdafx.h"

#include "LocalAssetStream.h"

LocalAssetStream::LocalAssetStream(std::string path) : m_path(path)
{
}

void LocalAssetStream::OpenStreamRead()
{
	m_istream = std::ifstream(m_path, std::ifstream::binary);
	m_istream.seekg(0, std::ios::end);
	m_length = static_cast<int>(m_istream.tellg());
	m_istream.seekg(0);
	reading = true;
}

void LocalAssetStream::CloseStream()
{
	if (reading)
		m_istream.close();
	else if (writing)
	{
		m_ostream.flush();
		m_ostream.close();
	}
	reading = false;
	writing = false;
}

void LocalAssetStream::ReadStreamBytes(char *buffer, int length)
{
	m_istream.read(buffer, length);
}

int LocalAssetStream::GetStreamLength()
{
	return m_length;
}

int LocalAssetStream::GetStreamPosition()
{
	if (reading)
		return static_cast<int>(m_istream.tellg());
	return static_cast<int>(m_ostream.tellp());
}

void LocalAssetStream::SeekStream(int position)
{
	if (reading)
		m_istream.seekg(position);
	else if (writing)
		m_ostream.seekp(position);
}

void LocalAssetStream::WriteStreamBytes(char *buffer, int length)
{
	m_ostream.write(buffer, length);
	if (GetStreamPosition() >= m_length)
		m_length = GetStreamPosition() + 1;
}

bool LocalAssetStream::CanRead()
{
	return true;
}

bool LocalAssetStream::CanWrite()
{
	return true;
}

void LocalAssetStream::OpenStreamWrite()
{
	m_ostream = std::ofstream(m_path, std::ostream::trunc | std::ostream::binary);
	m_length = 0;
	writing = true;
}
