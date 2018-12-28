#include "AssetManager.h"
#include "LocalAssetStreamSource.h"
#include <filesystem>

namespace filesystem = std::filesystem;

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

IAsset *AssetManager::LocateAsset(std::string path)
{
	auto asset = m_assets.find(path);
	return asset == m_assets.end() ? nullptr : asset->second;
}
