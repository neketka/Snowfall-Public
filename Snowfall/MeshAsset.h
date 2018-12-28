#pragma once
#include "AssetManager.h"
#include "Mesh.h"

enum class MeshFormat
{
	Obj
};

class MeshStreamReadingHint 
{
public:
	int StreamPosBegin;
	int StreamReadLength;
	std::string ObjectName;
};

class MeshAsset : public IAsset
{
public:
	MeshAsset(std::string path, Mesh mesh);
	MeshAsset(std::string path, MeshFormat format, MeshStreamReadingHint readingHint, IAssetStreamSource *stream);
	~MeshAsset();

	virtual std::string GetPath() const override;
	virtual void Load() override;
	virtual void Unload() override;
	virtual bool IsReady() override;
	virtual bool IsValid() override;

	Mesh& GetMesh();
private:
	Mesh *m_mesh;
	bool m_inMemory;
	bool m_loaded;
	bool m_loadSuccess;
	MeshFormat m_format;
	MeshStreamReadingHint m_hint;
	IAssetStreamSource *m_stream;
	std::string m_path;
};

class MeshAssetReader : public IAssetReader
{
	virtual std::vector<std::string> GetExtensions() override;
	virtual void LoadAssets(std::string ext, std::string path, IAssetStreamSource * streamSource, AssetManager& assetManager) override;
};
