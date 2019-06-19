#pragma once
#include "AssetManager.h"
#include "Mesh.h"
#include "MeshManager.h"
#include "CommandBuffer.h"

#include "export.h"

class MeshAsset : public IAsset
{
public:
	SNOWFALLENGINE_API MeshAsset(std::string path, Mesh mesh);
	SNOWFALLENGINE_API MeshAsset(std::string path, IAssetStreamIO *stream);
	SNOWFALLENGINE_API ~MeshAsset();

	SNOWFALLENGINE_API virtual std::string GetPath() const override;
	SNOWFALLENGINE_API virtual void SetStream(IAssetStreamIO *stream) override;
	SNOWFALLENGINE_API virtual void Load() override;
	SNOWFALLENGINE_API virtual void Unload() override;
	SNOWFALLENGINE_API virtual bool IsReady() override;
	SNOWFALLENGINE_API virtual bool IsValid() override;

	SNOWFALLENGINE_API GeometryHandle& GetGeometry();
	SNOWFALLENGINE_API Mesh& GetMesh();

	SNOWFALLENGINE_API void DrawMeshDirect(CommandBuffer& buffer, int instances = 1);

	SNOWFALLENGINE_API virtual IAsset *CreateCopy(std::string newPath) override;
	SNOWFALLENGINE_API virtual void Export() override;
private:
	Mesh *m_mesh;
	GeometryHandle m_handle;
	bool m_inMemory;
	bool m_loaded;
	bool m_loadSuccess;
	IAssetStreamIO *m_stream;
	std::string m_path;
};

class SNOWFALLENGINE_API MeshAssetReader : public IAssetReader
{
public:
	virtual std::vector<std::string> GetExtensions() override;
	virtual void LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager) override;
};
