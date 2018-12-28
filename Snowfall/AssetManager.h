#pragma once
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <iterator>

class AssetManager;

class IAssetStreamOutput
{
public:
	virtual void OpenStream() = 0;
	virtual void CloseStream() = 0;
	virtual void WriteStream(char *buffer, int length) = 0;
	virtual void SeekStream(int position) = 0;
	virtual void GetStreamPosition() = 0;
};

class IAssetStreamSource
{
public:
	virtual void OpenStream() = 0;
	virtual void CloseStream() = 0;
	virtual void ReadStream(char *buffer, int length) = 0;
	virtual void SeekStream(int position) = 0;
	virtual int GetStreamPosition() = 0;
	virtual int GetStreamLength() = 0;
};

class IAssetReader
{
public:
	virtual std::vector<std::string> GetExtensions() = 0;
	virtual void LoadAssets(std::string ext, std::string path, IAssetStreamSource *streamSource, AssetManager& assetManager) = 0;
};

class IAsset
{
public:
	virtual std::string GetPath() const = 0;
	virtual void Load() = 0;
	virtual void Unload() = 0;
	virtual bool IsReady() = 0;
	virtual bool IsValid() = 0;

	bool operator==(IAsset& asset) const
	{
		return GetPath() == asset.GetPath();
	}
};

class AssetManager
{
public:
	AssetManager();
	~AssetManager();
	void EnumerateLocalPath(bool asRoot, std::string path);
	void RegisterReader(IAssetReader *reader); // Pointer will be owned by the AssetManager instance
	void AddAsset(IAsset *asset); // Pointer will be owned by the AssetManager instance

	template<class T> 
	T& LocateAsset(std::string path)
	{
		IAsset *asset = LocateAsset(path);
		if (!asset || !asset->IsValid())
			return GetDefaultAsset<T>();
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
private:
	IAsset *LocateAsset(std::string path);

	std::map<std::string, IAsset *> m_assets;
	std::map<std::string, IAssetReader *> m_readers;
	std::map<const char *, IAsset *> m_defaultAssets;
};

