#include "stdafx.h"

#include "UserAsset.h"

UserAsset::UserAsset(std::string path, IAssetStreamIO *stream)
{
	m_path = path;
	m_stream = stream;
}

UserAsset::~UserAsset()
{
	//Dispose of resource
	delete m_stream;
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
	m_data.clear();
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
	memcpy(m_data.data(), data, length);
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
