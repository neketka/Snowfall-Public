#include "stdafx.h"

#include "AssetManager.h"
#include "LocalAssetStream.h"
#include "Snowfall.h"
#include "UserAsset.h"
#include "UUID.h"

AssetManager::AssetManager()
{
}

AssetManager::~AssetManager()
{
	for (auto asset : m_assets)
	{
		asset.second->Unload();
	}
}
/*
void AssetManager::EnumerateLocalPath(bool asRoot, std::string path)
{
	for (filesystem::directory_entry entry : filesystem::recursive_directory_iterator(path))
	{
		if (!filesystem::is_directory(entry))
		{
			std::string ext = filesystem::path(entry).extension().string();
			auto reader_iter = m_readers.find(ext);
			std::string relPath = filesystem::path(entry).lexically_relative(path).string();
			std::replace(relPath.begin(), relPath.end(), static_cast<char>(filesystem::path::preferred_separator), '.');
			if (reader_iter != m_readers.end())
			{
				LocalAssetStreamSource *src = new LocalAssetStreamSource(entry.path().string());
				reader_iter->second->LoadAssets(ext, relPath.substr(0, relPath.length() - ext.length()), src, *this);
			}
		}
	}
}
*/

void AssetManager::SetUserDataFolder(std::string name)
{
	char *str;
	size_t len;
	if (_dupenv_s(&str, &len, "APPDATA")) return;
	m_userFolder = filesystem::path(std::string(str) + "\\" + name);
	if (!filesystem::exists(m_userFolder))
		filesystem::create_directory(m_userFolder);
	for (filesystem::directory_entry entry : filesystem::directory_iterator(m_userFolder))
	{
		if (!filesystem::is_directory(entry))
		{
			LocalAssetStream *src = new LocalAssetStream(entry.path().string());

			src->OpenStreamRead();

			UserAsset *asset = new UserAsset(src->ReadString(), src);
			m_assets.insert({ asset->GetPath(), asset });

			src->CloseStream();
		}
	}
}

IAsset *AssetManager::CreateUserData(std::string path)
{
	LocalAssetStream *stream = new LocalAssetStream(m_userFolder.append(GenerateUUID() + ".udata").string());
	UserAsset *uasset = new UserAsset(path, stream);
	uasset->Export();

	return uasset;
}

void AssetManager::EnumerateUnpackedFolder(std::string path)
{
	for (filesystem::directory_entry entry : filesystem::recursive_directory_iterator(path))
	{
		if (!filesystem::is_directory(entry))
		{
			std::string ext = filesystem::path(entry).extension().string();
			auto reader_iter = m_readers.find(ext);
			std::string relPath = filesystem::path(entry).lexically_relative(path).string();
			if (reader_iter != m_readers.end())
			{
				LocalAssetStream *src = new LocalAssetStream(entry.path().string());
				reader_iter->second->LoadAssets(ext, src, *this);
			}
		}
	}
}

void AssetManager::RegisterReader(IAssetReader *reader)
{
	for (std::string ext : reader->GetExtensions())
	{
		m_readers.insert_or_assign(ext, reader);
	}
}

void AssetManager::AddAsset(IAsset *asset)
{
	m_assets.insert_or_assign(asset->GetPath(), asset);
}

void AssetManager::DeleteAsset(IAsset *asset)
{
	m_assets.erase(asset->GetPath());
}

IAsset *AssetManager::LocateAsset(std::string path)
{
	auto asset = m_assets.find(path);
	return asset == m_assets.end() ? nullptr : asset->second;
}

IAsset *AssetManager::LocateAssetGlobal(std::string path)
{
	return Snowfall::GetGameInstance().GetAssetManager().LocateAsset(path);
}