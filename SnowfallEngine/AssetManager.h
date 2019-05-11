#pragma once
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <iterator>
#include <filesystem>

#include "export.h"

class AssetManager;
namespace filesystem = std::filesystem;

class IAssetStreamIO
{
public:
	virtual void OpenStreamRead() = 0;
	virtual void OpenStreamWrite() = 0;
	virtual void CloseStream() = 0;
	virtual void WriteStreamBytes(char *buffer, int length) = 0;

	template<class T>
	void WriteStream(T *buffer, int length) { WriteStreamBytes(reinterpret_cast<char *>(buffer), length * sizeof(T)); }
	void WriteString(std::string str)
	{
		unsigned int len = static_cast<unsigned int>(str.length());
		WriteStream(&len, 1);
		WriteStream(const_cast<char *>(str.c_str()), len);
	}

	virtual void ReadStreamBytes(char *buffer, int length) = 0;
	template<class T>
	void ReadStream(T *buffer, int length) { ReadStreamBytes(reinterpret_cast<char *>(buffer), length * sizeof(T)); }
	std::string ReadString()
	{
		unsigned int size = 0;
		ReadStream(&size, 1);
		return ReadString(size);
	}
	std::string ReadString(unsigned int length)
	{
		char *buffer = new char[length + 1];
		buffer[length] = '\0';
		ReadStream(buffer, length);
		std::string str = std::string(buffer);
		delete buffer;

		return str;
	}

	virtual bool CanRead() = 0;
	virtual bool CanWrite() = 0;
	virtual void SeekStream(int position) = 0;
	virtual int GetStreamPosition() = 0;
	virtual int GetStreamLength() = 0;
};

class IAssetReader
{
public:
	virtual std::vector<std::string> GetExtensions() = 0;
	virtual void LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager) = 0;
};

class IAsset
{
public:
	virtual std::string GetPath() const = 0;
	virtual void SetStream(IAssetStreamIO *stream) = 0;
	virtual void Load() = 0;
	virtual void Unload() = 0;
	virtual bool IsReady() = 0;
	virtual bool IsValid() = 0; 

	virtual IAsset *CreateCopy(std::string newPath, IAssetStreamIO *output) = 0;
	virtual void Export() = 0;

	bool operator==(IAsset& asset) const
	{
		return GetPath() == asset.GetPath();
	}
};

class AssetManager
{
public:
	SNOWFALLENGINE_API AssetManager();
	SNOWFALLENGINE_API ~AssetManager();

	SNOWFALLENGINE_API void SetUserDataFolder(std::string name);
	SNOWFALLENGINE_API void EnumerateUnpackedFolder(std::string path);
	SNOWFALLENGINE_API void RegisterReader(IAssetReader *reader); // Pointer will be owned by the AssetManager instance
	SNOWFALLENGINE_API void AddAsset(IAsset *asset); // Pointer will be owned by the AssetManager instance
	SNOWFALLENGINE_API void DeleteAsset(IAsset& asset);

	template<class T>
	T *CopyAssetSameSource(std::string newPath, IAsset& asset)
	{
		T *a = asset.CreateCopy(newPath, nullptr);
		AddAsset(a);
		return a;
	}

	template<class T>
	T *CopyAssetNewSource(std::string newPath, IAssetStreamIO *output, IAsset& asset)
	{
		T *a = asset.CreateCopy(newPath, output);
		AddAsset(a);
		return a;
	}

	template<class T> 
	T& LocateAsset(std::string path)
	{
		IAsset *asset = LocateAsset(path);
		if (!asset || !asset->IsValid())
			return GetDefaultAsset<T>();
		return *dynamic_cast<T*>(asset);
	}

	template<class T>
	T *LocateAssetNullable(std::string path)
	{
		IAsset *asset = LocateAsset(path);
		return *dynamic_cast<T*>(asset);
	}

	template<class T>
	void SetDefaultAsset(T *asset) // Pointer will be owned by the AssetManager instance
	{
		m_defaultAssets.insert({ typeid(T).name(), static_cast<IAsset *>(asset) });
	}

	template<class T>
	T& GetDefaultAsset() 
	{
		return dynamic_cast<T&>(*m_defaultAssets.at(typeid(T).name()));
	}

	template<class T>
	static T& LocateAssetGlobal(std::string path)
	{
		return *dynamic_cast<T*>(LocateAssetGlobal(path));
	}
private:
	SNOWFALLENGINE_API IAsset *LocateAsset(std::string path);
	SNOWFALLENGINE_API static IAsset *LocateAssetGlobal(std::string path);
	filesystem::path m_userFolder;

	std::map<std::string, IAsset *> m_assets;
	std::map<std::string, IAssetReader *> m_readers;
	std::map<const char *, IAsset *> m_defaultAssets;
};
