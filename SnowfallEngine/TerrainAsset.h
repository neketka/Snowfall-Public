#pragma once
#include "AssetManager.h"
#include "MeshManager.h"
#include "MaterialAsset.h"
#include "TextureAsset.h"
#include "UserAsset.h"
#include "WorkerManager.h"

#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

#define LOD_COUNT 4

using PhysicsTerrainShape = btHeightfieldTerrainShape;

class TerrainChunk
{
public:
	glm::ivec2 Location;
	PhysicsTerrainShape *PhysicsShape = nullptr;

	UserAsset *ChunkDataAsset = nullptr;
	bool NewlySaved = false;

	TextureAsset *TerrainTexture = nullptr;
	TextureAsset *NormalTexture = nullptr;

	float *Heights[LOD_COUNT];
	glm::vec4 *AlphaMap[LOD_COUNT];

	bool PropagatingLOD = false;
	bool LoadingLOD[LOD_COUNT];

	float *LowerBoundHeights = nullptr;

	int CurrentLOD = LOD_COUNT;
	int MaxLoadedLOD = LOD_COUNT;
	int LowestLoadedLOD = LOD_COUNT;
};

class TerrainLoadUpdate
{
public:
	bool NewlyCreated;
	int ChunkX;
	int ChunkY;
	int ChunkAddress;
	int OldLod;
	int NewLod;
};

class TerrainLoadRequest
{
public:
	int X, Y, Address, Lod;
};

class CalculatedGPUData
{
public:
	TerrainChunk *chunk;
	int lod;
	std::vector<glm::vec4> terrainData;
	std::vector<glm::vec4> normalData;
};

class TerrainAsset : public IAsset
{
public:
	SNOWFALLENGINE_API TerrainAsset(std::string path, IAssetStreamIO *stream);
	SNOWFALLENGINE_API TerrainAsset(std::string path, int width, float scale, float minHeight, float maxHeight);
	SNOWFALLENGINE_API ~TerrainAsset();

	SNOWFALLENGINE_API virtual std::string GetPath() const override;
	SNOWFALLENGINE_API virtual void SetStream(IAssetStreamIO *stream) override;
	SNOWFALLENGINE_API virtual void Load() override;
	SNOWFALLENGINE_API virtual void Unload() override;
	SNOWFALLENGINE_API virtual bool IsReady() override;
	SNOWFALLENGINE_API virtual bool IsValid() override;
	SNOWFALLENGINE_API virtual IAsset *CreateCopy(std::string newPath) override;
	SNOWFALLENGINE_API virtual void Export() override;

	SNOWFALLENGINE_API PhysicsTerrainShape *GetPhysicsShape(int chunkAddress);
	SNOWFALLENGINE_API GeometryHandle GetGeometry(int lod);

	SNOWFALLENGINE_API int ChunkToAddress(glm::ivec2 pos);
	SNOWFALLENGINE_API glm::ivec4 WorldPosToChunk(glm::ivec2 pos);

	SNOWFALLENGINE_API void SetTerrainMaterial(MaterialAsset *asset);
	SNOWFALLENGINE_API void RequestChunkMaxLOD(int pX, int pY, int lod);

	SNOWFALLENGINE_API void SetLowerBoundArea(int address, float *data);
	SNOWFALLENGINE_API void ModifyArea(float *data, int x, int y, int w, int h);
	SNOWFALLENGINE_API void ModifyArea(int address, float *data);
	SNOWFALLENGINE_API void ModifyAlphaMap(glm::vec4 *data, int x, int y, int w, int h);
	SNOWFALLENGINE_API void ModifyAlphaMap(int address, glm::vec4 *data);

	SNOWFALLENGINE_API void ClearLodUpdates();
	SNOWFALLENGINE_API std::vector<TerrainLoadUpdate>& GetLoadUpdates();

	SNOWFALLENGINE_API bool DoesChunkExist(int address);
	SNOWFALLENGINE_API bool IsChunkLoaded(int address);

	SNOWFALLENGINE_API void ApplyChangesToGPU();

	SNOWFALLENGINE_API void RenderTerrainToMeshManager(LayerMask mask);

	void SetSavable(bool savable) { m_saveChangesOnUnload = savable; }
	bool IsSavable() { return m_saveChangesOnUnload; }

	float GetMinHeight() { return m_minHeight; }
	float GetMaxHeight() { return m_maxHeight; }
	int GetBaseLength() { return m_chunkBaseLength; }
	int GetRealLength() { return m_realBaseLength; }
	int GetChunkLod(int address) { if (IsChunkLoaded(address)) return m_addressMap[address]->CurrentLOD; else return -1; }

private:
	bool m_dead = false;
	void TryLoadChunkLODFromAsset(TerrainChunk *chunk, int lod);
	void TrySaveChunkToAsset(TerrainChunk *chunk);
	void DisposeChunk(TerrainChunk *chunk);
	void PropagateLOD(TerrainChunk *chunk);
	bool EnsureLOD(TerrainChunk *chunk, int lod);
	void UpdateEdgeValuesWithNeighbors(TerrainChunk *chunk, int lod);

	void ReloadPageFile();
	void SavePageFile();

	void LoadGeometries();

	Sampler m_samplers[2];

	GeometryHandle m_geometries[LOD_COUNT];
	bool m_geometriesLoaded = false;

	float m_minHeight;
	float m_maxHeight;
	float m_realBaseLength;
	int m_chunkBaseLength;

	bool m_saveChangesOnUnload = false;
	std::string m_path;

	std::string m_matPath;
	MaterialAsset *m_material;
	std::vector<WorkerTask<CalculatedGPUData *> *> m_workingCalcs;
	IAssetStreamIO *m_pageFileStream = nullptr;

	bool m_loadedPageFile = false;

	std::map<int, std::string> m_addressToPath;
	std::map<int, TerrainChunk *> m_addressMap;
	std::vector<TerrainLoadRequest> m_loadRequests;
	std::vector<TerrainLoadUpdate> m_updates;
	std::set<int> m_changedChunks;
};

class TerrainAssetReader : public IAssetReader
{
public:
	virtual AssetReadingPriority GetPriority() override { return AssetReadingPriority::Last; }
	virtual std::vector<std::string> GetExtensions() override;
	virtual void LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager) override;
};