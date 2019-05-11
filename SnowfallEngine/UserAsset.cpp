#include "UserAsset.h"


UserAsset::UserAsset(IAssetStreamIO *stream) : m_stream(stream)
{
	stream->OpenStreamRead();
	m_path = stream->ReadString();
	stream->CloseStream();
}

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
}

void UserAsset::Load()
{
}

void UserAsset::Unload()
{
}

bool UserAsset::IsReady()
{
	return m_loaded;
}

bool UserAsset::IsValid()
{
	return true;
}

IAsset *UserAsset::CreateCopy(std::string newPath, IAssetStreamIO * output)
{
	return nullptr;
}

void UserAsset::Export()
{
}
