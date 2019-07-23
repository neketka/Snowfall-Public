#include "stdafx.h"

#include "UserAsset.h"

UserAsset::UserAsset(std::string path, IAssetStreamIO *stream)
{
	m_path = path;
	m_stream = stream;
	m_memStream = new MemoryAssetStream(m_data);
}

UserAsset::~UserAsset()
{
	//Dispose of resource
	if (m_stream)
		delete m_stream;
	delete m_memStream;
}

std::string UserAsset::GetPath() const
{
	return m_path;
}

void UserAsset::SetStream(IAssetStreamIO *stream)
{
	m_stream = stream;
}

void UserAsset::Load()
{
	m_stream->OpenStreamRead();

	int size = 0;

	m_stream->ReadString();
	m_stream->ReadStream(&size, 1);
	m_data.resize(size);
	m_stream->ReadStreamBytes(m_data.data(), size);

	m_stream->CloseStream();
}

void UserAsset::Unload()
{
	m_data.resize(0);
}

bool UserAsset::IsReady()
{
	return m_loaded;
}

bool UserAsset::IsValid()
{
	return true;
}

void UserAsset::SetData(char *data, int offset, int length)
{
	m_memStream->SeekStream(offset);
	m_memStream->WriteStreamBytes(data, length);
}

int UserAsset::GetDataSize()
{
	return m_data.capacity();
}

void UserAsset::SetDataSize(int bytes)
{
	m_data.resize(bytes);
}

char *UserAsset::GetData()
{
	return m_data.data();
}

IAssetStreamIO *UserAsset::GetMemoryStream()
{
	return m_memStream;
}

IAssetStreamIO *UserAsset::GetIOStream()
{
	return m_stream;
}

IAsset *UserAsset::CreateCopy(std::string newPath)
{
	return new UserAsset(newPath, m_stream);
}

void UserAsset::Export()
{
	m_stream->OpenStreamRead();

	m_stream->WriteStream(static_cast<int>(m_data.capacity()));
	m_stream->WriteStream(static_cast<int>(m_data.capacity()));
	m_stream->WriteStream(m_data.data(), m_data.capacity());

	m_stream->CloseStream();
}
