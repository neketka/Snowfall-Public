#include "stdafx.h"
#include "TerrainAsset.h"
#include "Snowfall.h"

#include <glm/gtc/noise.hpp>

std::vector<std::string> TerrainAssetReader::GetExtensions()
{
	return { ".trasset" };
}

void TerrainAssetReader::LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager)
{
	int x = 0;
	stream->OpenStreamRead();
	stream->ReadStream(&x, 1);
	assetManager.AddAsset(new TerrainAsset(stream->ReadString(), stream));

	stream->CloseStream();
}

TerrainAsset::TerrainAsset(std::string path, IAssetStreamIO *stream)
{
	m_path = path;
	m_pageFileStream = stream;
	m_saveChangesOnUnload = true;

	for (Sampler sm : m_samplers)
	{
		sm.SetMinificationFilter(MinificationFilter::NearestMipmapNearest);
		sm.SetMagnificationFilter(MagnificationFilter::Nearest);
	}
}

TerrainAsset::TerrainAsset(std::string path, int width, float scale, float minHeight, float maxHeight)
{
	m_path = path;
	m_chunkBaseLength = width;
	m_realBaseLength = scale;
	m_minHeight = minHeight;
	m_maxHeight = maxHeight;

	for (Sampler sm : m_samplers)
	{
		sm.SetMinificationFilter(MinificationFilter::NearestMipmapNearest);
		sm.SetMagnificationFilter(MagnificationFilter::Nearest);
	}
}

TerrainAsset::~TerrainAsset()
{
	if (m_pageFileStream)
		delete m_pageFileStream;
}

std::string TerrainAsset::GetPath() const
{
	return m_path;
}

void TerrainAsset::SetStream(IAssetStreamIO *stream)
{
	m_pageFileStream = stream;
}

void TerrainAsset::Load()
{
	if (!m_loadedPageFile)
		ReloadPageFile();

	LoadGeometries();

	for (TerrainLoadRequest& req : m_loadRequests)
	{
		TerrainLoadUpdate upd;
		upd.ChunkX = req.X;
		upd.ChunkY = req.Y;
		upd.ChunkAddress = req.Address;
		upd.NewlyCreated = false;

		req.Lod = glm::clamp(req.Lod, -1, LOD_COUNT - 1);

		float c = glm::ceil(m_chunkBaseLength / glm::pow(2, req.Lod));

		TerrainChunk *chunk;
		if (!m_addressMap.count(req.Address))
		{
			if (req.Lod == -1)
				continue;
			chunk = new TerrainChunk;
			int baseLen = m_chunkBaseLength;
			for (int i = 0; i < LOD_COUNT; ++i)
			{
				int bl2 = baseLen * baseLen;
				chunk->AlphaMap[i] = new glm::vec4[bl2];
				chunk->Heights[i] = new float[bl2];

				for (int j = 0; j < bl2; ++j)
					chunk->AlphaMap[i][j] = glm::vec4(1);
				memset(chunk->Heights[i], 0, bl2 * sizeof(float));

				baseLen = glm::ceil(baseLen * 0.5f);
			}

			chunk->Location = glm::ivec2(req.X, req.Y);
			chunk->CurrentLOD = 0;
			chunk->MaxLoadedLOD = 0;
			chunk->LowestLoadedLOD = 0;
			chunk->ChunkDataAsset = nullptr;

			memset(chunk->LockReadLOD, 0, sizeof(bool) * LOD_COUNT);
			memset(chunk->LockWriteLOD, 0, sizeof(bool) * LOD_COUNT);

			int texSize = glm::ceil(m_chunkBaseLength / glm::pow(2, LOD_COUNT - 1)) * glm::pow(2, LOD_COUNT - 1);

			chunk->TerrainTexture = new TextureAsset("", TextureType::Texture2D, TextureInternalFormat::RGBA32F, texSize, texSize, 1, 4);
			chunk->NormalTexture = new TextureAsset("", TextureType::Texture2D, TextureInternalFormat::RGBA32F, texSize, texSize, 1, 4);
			chunk->LowerBoundHeights = new float[(m_chunkBaseLength - 1) * (m_chunkBaseLength - 1)];
			memset(chunk->LowerBoundHeights, 0, sizeof(float) * (m_chunkBaseLength - 1) * (m_chunkBaseLength - 1));

			m_addressMap[req.Address] = chunk;

			if (m_addressToPath.contains(req.Address))
			{
				chunk->ChunkDataAsset = &AssetManager::LocateAssetGlobal<UserAsset>(m_addressToPath[req.Address]);
				for (int i = req.Lod; i < LOD_COUNT; ++i)
				{
					TryLoadChunkLODFromAsset(chunk, i);
				}

				chunk->MaxLoadedLOD = chunk->CurrentLOD = req.Lod;
				chunk->LowestLoadedLOD = LOD_COUNT - 1;
				upd.OldLod = -1;
				upd.NewLod = req.Lod;
			}
			else
			{
				upd.NewlyCreated = true;
				upd.OldLod = -1;
				upd.NewLod = 0;
			}

			m_updates.push_back(upd);

			m_changedChunks.insert(req.Address);

			chunk->PhysicsShape = new btHeightfieldTerrainShape(c, c, chunk->Heights[req.Lod], 1, m_minHeight, m_maxHeight, 1, PHY_ScalarType::PHY_FLOAT, true);
			chunk->PhysicsShape->setLocalScaling(btVector3(m_realBaseLength / c, 1, m_realBaseLength / c));

			continue;
		}
		else
			chunk = m_addressMap[req.Address];

		upd.OldLod = chunk->CurrentLOD;
		upd.NewLod = req.Lod;

		if (chunk->CurrentLOD == req.Lod)
			continue;

		chunk->PhysicsShape = new btHeightfieldTerrainShape(c, c, chunk->Heights[req.Lod], 1, m_minHeight, m_maxHeight, 1, PHY_ScalarType::PHY_FLOAT, true);
		chunk->PhysicsShape->setLocalScaling(btVector3(m_realBaseLength / c, 1, m_realBaseLength / c));

		if (req.Lod == -1)
		{
			m_addressMap.unsafe_erase(ChunkToAddress(chunk->Location)); // Look over this
			Snowfall::GetGameInstance().GetWorkerManager().EnqueueTask<nullptr_t>([this, chunk, req](int iter) {
				TrySaveChunkToAsset(chunk);
				DisposeChunk(chunk);
				return nullptr;
			}, 1);
		}
		else if (chunk->CurrentLOD < req.Lod)
		{
			chunk->CurrentLOD = req.Lod;
			Snowfall::GetGameInstance().GetWorkerManager().EnqueueTask<nullptr_t>([this, chunk, req](int iter) {
				if (req.Lod > chunk->LowestLoadedLOD)
				{
					if (chunk->MaxLoadedLOD != 0)
						TryLoadChunkLODFromAsset(chunk, 0);
					PropagateLOD(chunk);
					chunk->LowestLoadedLOD = LOD_COUNT - 1;
				}
				m_changedChunks.insert(req.Address);
				return nullptr;
			}, 1);
		}
		else if (chunk->CurrentLOD > req.Lod)
		{
			chunk->CurrentLOD = req.Lod;
			Snowfall::GetGameInstance().GetWorkerManager().EnqueueTask<nullptr_t>([this, chunk, req](int iter) {
				if (req.Lod < chunk->MaxLoadedLOD)
				{
					for (int i = chunk->MaxLoadedLOD + 1; i >= req.Lod; --i)
						TryLoadChunkLODFromAsset(chunk, i);
					chunk->MaxLoadedLOD = req.Lod;
				}
				m_changedChunks.insert(req.Address);
				return nullptr;
			}, 1);
		}
		m_updates.push_back(upd);
	}

	m_loadRequests.clear();
}

void TerrainAsset::Unload()
{
	if (m_dead)
		return;
	if (m_geometriesLoaded)
	{
		MeshManager& mManager = Snowfall::GetGameInstance().GetMeshManager();

		for (GeometryHandle& handle : m_geometries)
		{
			mManager.EraseGeometry(handle);
		}

		m_geometriesLoaded = false;
	}

	if (m_addressMap.empty())
		return;

	for (auto kp : m_addressMap)
	{
		TrySaveChunkToAsset(kp.second);
		DisposeChunk(kp.second);
	}

	SavePageFile();
}

bool TerrainAsset::IsReady()
{
	return true;
}

bool TerrainAsset::IsValid()
{
	return true;
}

IAsset *TerrainAsset::CreateCopy(std::string newPath)
{
	return nullptr;
}

void TerrainAsset::Export()
{
	for (auto kp : m_addressMap)
		TrySaveChunkToAsset(kp.second);
	SavePageFile();
}

PhysicsTerrainShape *TerrainAsset::GetPhysicsShape(int chunkAddress)
{
	return m_addressMap[chunkAddress]->PhysicsShape;
}

GeometryHandle TerrainAsset::GetGeometry(int lod)
{
	LoadGeometries();
	return m_geometries[lod];
}

int TerrainAsset::ChunkToAddress(glm::ivec2 pos)
{
	int x = pos.x << 16;
	int y = pos.y & 0xFFFF;
	return x | y;
}

glm::ivec2 TerrainAsset::AddressToChunk(int address)
{
	short x = address >> 16;
	short y = address & 0xFFFF;
	return glm::ivec2(x, y);
}

glm::ivec4 TerrainAsset::WorldPosToChunk(glm::ivec2 pos)
{
	int rW = m_realBaseLength - 1;

	int divX = pos.x / rW;
	int remX = ((pos.x < 0) ? (rW - pos.x) : (pos.x)) % rW;

	int divY = (pos.y) / rW;
	int remY = ((pos.y < 0) ? (rW - pos.y) : (pos.y)) % rW;

	return glm::ivec4(divX, divY, remX, remY);
}

void TerrainAsset::SetTerrainMaterial(MaterialAsset *asset)
{
	m_material = asset;
	m_matPath = asset->GetPath();
}

void TerrainAsset::RequestChunkMaxLOD(int pX, int pY, int lod)
{
	TerrainLoadRequest request;
	request.X = pX;
	request.Y = pY;
	request.Address = ChunkToAddress(glm::ivec2(pX, pY));
	request.Lod = lod;

	m_loadRequests.push_back(request);
}

void TerrainAsset::SetLowerBoundArea(int address, float *data)
{
	int kW = m_chunkBaseLength - 1;
	TerrainChunk *chunk = PullChunkConcurrent(address);
	if (!chunk)
		return;
	LockForWriting(address, 0);
	memcpy(chunk->LowerBoundHeights, data, sizeof(float) * kW * kW);
	UnlockWriting(address, 0);
}

void TerrainAsset::SetArea(int address, float *data)
{
	int kW = m_chunkBaseLength - 1;
	TerrainChunk *chunk = PullChunkConcurrent(address);
	if (!chunk)
		return;

	LockForWriting(address, 0);
	chunk->MaxLoadedLOD = 0;
	chunk->CurrentLOD = 0;
	chunk->LowestLoadedLOD = 0;
	for (int y = 0; y < kW; ++y)
	{
		for (int x = 0; x < kW; ++x)
			chunk->Heights[0][y * m_chunkBaseLength + x] = data[y * kW + x];
	}
	m_changedChunks.insert(address);
	UnlockWriting(address, 0);
	if (IsChunkLoaded(address))
	{
	}
}

void TerrainAsset::SetAlphaMap(int address, glm::vec4 *data)
{
	int kW = m_chunkBaseLength - 1;
	TerrainChunk *chunk = PullChunkConcurrent(address);
	if (!chunk)
		return;

	LockForWriting(address, 0);
	chunk->MaxLoadedLOD = 0;
	chunk->CurrentLOD = 0;
	chunk->LowestLoadedLOD = 0;
	for (int y = 0; y < kW; ++y)
	{
		for (int x = 0; x < kW; ++x)
			chunk->AlphaMap[0][y * m_chunkBaseLength + x] = data[y * kW + x];
	}
	m_changedChunks.insert(address);
	UnlockWriting(address, 0);
}

void TerrainAsset::ApplyOperation(TerrainOperation op, int x, int y, int w, int h, bool async)
{
	auto func = [this, op, x, y, w, h](int iter) {
		int kW = m_chunkBaseLength - 1;
		int rW = m_realBaseLength - 1;
		int address = 0;
		for (int rX = 0; rX < w; ++rX)
		{
			int divX = (rX + x) / kW;
			int remX = ((rX + x < 0) ? (kW - rX - x) : (rX + x)) % kW;
			TerrainChunk *chunk = nullptr;
			for (int rY = 0; rY < h; ++rY)
			{
				int divY = (rY + y) / kW;
				int remY = ((rY + y < 0) ? (kW - rY - y) : (rY + y)) % kW;
				if (remY == 0)
				{
					address = ChunkToAddress(glm::ivec2(divX, divY));
					if (!(chunk = PullChunkConcurrent(address)))
					{
						chunk = nullptr;
						rY += kW;
						continue;
					}
					UnlockWriting(address, 0);
					if (!EnsureLOD(chunk, 0))
					{
						rY += kW;
						continue;
					}
					while (!LockForWriting(address, 0));
					if (chunk)
					{
						m_changedChunks.insert(address);
						chunk->MaxLoadedLOD = 0;
						chunk->CurrentLOD = 0;
						chunk->LowestLoadedLOD = 0;
					}
				}
				int addr = remY * m_chunkBaseLength + remX;
				if (chunk)
				{
					float kX = rW * divX + static_cast<float>(remX) / kW * rW;
					float kY = rW * divY + static_cast<float>(remY) / kW * rW;

					op(chunk->Heights[0][addr], chunk->AlphaMap[0][addr], chunk->LowerBoundHeights[remY * kW + remX],
						glm::vec2(kX, kY), glm::ivec2(remX, remY), glm::ivec2(divX, divY));

					chunk->Heights[0][addr] = glm::clamp(chunk->Heights[0][addr], m_minHeight, m_maxHeight);
					m_changedChunks.insert(address);
				}
			}
		}
		UnlockWriting(address, 0);
		return nullptr;
	};

	if (async)
		Snowfall::GetGameInstance().GetWorkerManager().EnqueueTask<nullptr_t>(func, 1);
	else
		func(0);
}

void TerrainAsset::ApplyOperation(TerrainOperation op, int address, bool async)
{
	if (!IsChunkLoaded(address))
		return;

	auto func = [this, op, address](int iter) {
		while (!LockForWriting(address, 0));

		int kW = m_chunkBaseLength - 1;
		int rW = m_realBaseLength - 1;
		TerrainChunk *chunk = PullChunkConcurrent(address);
		if (!chunk)
			return nullptr;
		chunk->MaxLoadedLOD = 0;
		chunk->CurrentLOD = 0;
		chunk->LowestLoadedLOD = 0;
		for (int y = 0; y < kW; ++y)
		{
			for (int x = 0; x < kW; ++x)
			{
				int addr = y * m_chunkBaseLength + x;

				float kX = rW * chunk->Location.x + static_cast<float>(x) / kW * rW;
				float kY = rW * chunk->Location.y + static_cast<float>(y) / kW * rW;

				op(chunk->Heights[0][addr], chunk->AlphaMap[0][addr], chunk->LowerBoundHeights[y * kW + x], glm::vec2(kX, kY), glm::ivec2(x, y), chunk->Location);
			}
		}
		UnlockWriting(address, 0);
		m_changedChunks.insert(address);
		return nullptr;
	};

	if (async)
		Snowfall::GetGameInstance().GetWorkerManager().EnqueueTask<nullptr_t>(func, 1);
	else
		func(0);
}

void TerrainAsset::ClearLodUpdates()
{
	m_updates.clear();
}

std::vector<TerrainLoadUpdate>& TerrainAsset::GetLoadUpdates()
{
	return m_updates;
}

bool TerrainAsset::DoesChunkExist(int address)
{
	return m_addressToPath.contains(address);
}

bool TerrainAsset::IsChunkLoaded(int address)
{
	return m_addressMap.count(address) != 0;
}

void TerrainAsset::UnloadChunks(std::vector<int> addresses, bool allNotIncluded)
{
	for (auto addrKp : m_addressMap)
	{
		glm::ivec2 pos = AddressToChunk(addrKp.first);
		if (std::find(addresses.begin(), addresses.end(), addrKp.first) != addresses.end())
		{
			if (!allNotIncluded)
				RequestChunkMaxLOD(pos.x, pos.y, -1);
		}
		else if (allNotIncluded)
			RequestChunkMaxLOD(pos.x, pos.y, -1);
	}
}

glm::vec2 CartesianToSphericalNormal(glm::vec3 cartesian)
{
	float theta = glm::acos(cartesian.z);
	float phi = glm::atan(cartesian.y, cartesian.x);

	return glm::vec2(theta, phi);
}

void TerrainAsset::ApplyChangesToGPU()
{
	WorkerManager& manager = Snowfall::GetGameInstance().GetWorkerManager();
	concurrency::concurrent_unordered_set<int> upds(m_changedChunks);
	m_changedChunks.clear();
	concurrency::concurrent_unordered_set<int> upds2;
	
	for (int addr : upds)
	{
		TerrainChunk *chunk = PullChunkConcurrent(addr);
		if (!chunk)
			return;
		if (!LockForWriting(addr, chunk->CurrentLOD))
		{
			m_changedChunks.insert(addr);
			continue;
		}
		upds2.insert(addr);
		UpdateEdgeValuesWithNeighbors(chunk, chunk->CurrentLOD);
		UnlockWriting(addr, chunk->CurrentLOD);
	}
	
	for (int addr : upds2)
	{
		TerrainChunk *chunk = PullChunkConcurrent(addr);
		int lod = chunk->CurrentLOD;

		WorkerTask<CalculatedGPUData *> *worker = &manager.EnqueueTask<CalculatedGPUData *>([this, addr, lod, chunk](int iteration) {
			CalculatedGPUData *data = new CalculatedGPUData;
			
			data->chunk = chunk;

			while (!LockForReading(addr, lod));

			data->lod = chunk->CurrentLOD = lod;
			data->addr = addr;
			int c = glm::ceil(m_chunkBaseLength / glm::pow(2, lod));
			int c2 = c * c;

			std::vector<glm::vec4>& terrainData = data->terrainData = std::vector<glm::vec4>(c2);
			std::vector<glm::vec4>& normalData = data->normalData = std::vector<glm::vec4>(c2);

			int leftAddr = ChunkToAddress(chunk->Location + glm::ivec2(-1, 0)), rightAddr = ChunkToAddress(chunk->Location + glm::ivec2(1, 0)), 
				upAddr = ChunkToAddress(chunk->Location + glm::ivec2(0, 1)), downAddr = ChunkToAddress(chunk->Location + glm::ivec2(0, -1));

			TerrainChunk *left = PullChunkConcurrent(leftAddr), *right = PullChunkConcurrent(rightAddr), 
				*up = PullChunkConcurrent(upAddr), *down = PullChunkConcurrent(downAddr);

			while (!LockForReading(leftAddr, lod));
			while (!LockForReading(rightAddr, lod));
			while (!LockForReading(upAddr, lod));
			while (!LockForReading(downAddr, lod));

			for (int x = 0; x < c; ++x)
			{
				for (int y = 0; y < c; ++y)
				{
					if (chunk->CurrentLOD != lod)
					{
						UnlockReading(addr, lod);

						UnlockReading(leftAddr, lod);
						UnlockReading(rightAddr, lod);
						UnlockReading(upAddr, lod);
						UnlockReading(downAddr, lod);

						delete data;
						data = nullptr;
						return data;
					}

					float center = chunk->Heights[lod][y * c + x];

					float centerL, centerR, centerU, centerD;
					if (x < 1)
					{
						if (left && EnsureLOD(left, lod))
							centerL = left->Heights[lod][y * c + c - 2];
						else
							centerL = center;
					}
					else centerL = chunk->Heights[lod][y * c + (x - 1)];

					if (x == c - 1)
					{
						if (right && EnsureLOD(right, lod))
							centerR = right->Heights[lod][y * c + 1];
						else
							centerR = center;
					}
					else centerR = chunk->Heights[lod][y * c + (x + 1)];

					if (y == c - 1)
					{
						if (up && EnsureLOD(up, lod))
							centerU = up->Heights[lod][1 * c + x];
						else
							centerU = center;
					}
					else centerU = chunk->Heights[lod][(y + 1) * c + x];

					if (y == 0) 
					{
						if (down && EnsureLOD(down, lod))
							centerD = down->Heights[lod][(c - 2) * c + x];
						else
							centerD = center;
					}
					else centerD = chunk->Heights[lod][(y - 1) * c + x];

					unsigned alpha = glm::packUnorm4x8(chunk->AlphaMap[lod][y * c + x]);

					float twoOverC = 2.f * m_realBaseLength / c;

					glm::vec3 vertical = -glm::vec3(0.f, centerU - centerD, twoOverC);
					glm::vec3 horizontal = -glm::vec3(twoOverC, centerR - centerL, 0.f);
					glm::vec3 normal = glm::normalize(glm::cross(vertical, horizontal));

					glm::vec3 tangent = -glm::normalize(horizontal);

					terrainData[y * c + x] = glm::vec4(center, *reinterpret_cast<float *>(&alpha), 0.f, 0.f);
					normalData[y * c + x] = glm::vec4(CartesianToSphericalNormal(normal),
						CartesianToSphericalNormal(tangent));
				}
			}

			UnlockReading(addr, lod);

			UnlockReading(leftAddr, data->lod);
			UnlockReading(rightAddr, data->lod);
			UnlockReading(upAddr, data->lod);
			UnlockReading(downAddr, data->lod);

			return data;
		}, 1);

		m_workingCalcs.push_back(worker);
	}

	for (int i = 0; i < m_workingCalcs.size(); ++i)
	{
		WorkerTask<CalculatedGPUData *> *worker = m_workingCalcs[i];
		if (worker->IsDone())
		{
			CalculatedGPUData *data = worker->GetResults()[0];
			worker->Destroy();
			if (data && IsChunkLoaded(data->addr))
			{
				int c = glm::ceil(m_chunkBaseLength / glm::pow(2, data->lod));

				data->chunk->TerrainTexture->GetTextureObject().SetData(0, 0, c, c, data->lod, TexturePixelFormat::RGBA, TextureDataType::Float, data->terrainData.data());
				data->chunk->NormalTexture->GetTextureObject().SetData(0, 0, c, c, data->lod, TexturePixelFormat::RGBA, TextureDataType::Float, data->normalData.data());

				if (data->chunk->RenderedLOD == -1)
					data->chunk->RenderedLOD = data->lod;
				else if (data->chunk->CurrentLOD == data->lod)
					data->chunk->RenderedLOD = data->lod;
			}
			if (data)
				delete data;
			m_workingCalcs.erase(m_workingCalcs.begin() + i);
			--i;
		}
	}
}

void TerrainAsset::RenderTerrainToMeshManager(LayerMask mask)
{
	MeshManager& mManager = Snowfall::GetGameInstance().GetMeshManager();

	Material& mat = m_material->GetMaterial();

	RendererStateChange state;

	state.GeometryBuffer = TERRAIN_BUFFER_ID;
	state.ObjectCount = 1;
	state.LayerMask = mask;
	state.Type = PrimitiveType::Triangles;
	state.Shader = mat.MaterialShader;
	state.InstanceCount = 1;
	state.Constants = mat.Constants;

	for (auto kp : m_addressMap)
	{
		TerrainChunk *chunk = kp.second;

		if (chunk->RenderedLOD == -1)
			continue;

		if (chunk->CurrentLOD >= LOD_COUNT - 1)
			state.Specializations = { "VERTEX_MATERIAL" };
		else
			state.Specializations = { };

		std::vector<GeometryHandle> ghandles{ GetGeometry(chunk->RenderedLOD) };
		std::vector<BoundingBox> boxes{ BoundingBox() };

		float uvRatio = m_chunkBaseLength / (glm::ceil(m_chunkBaseLength / glm::pow(2, LOD_COUNT - 1)) * glm::pow(2, LOD_COUNT - 1));

		state.Constants.AddConstant(16, chunk->TerrainTexture->GetTextureObject(), m_samplers[0]);
		state.Constants.AddConstant(17, chunk->NormalTexture->GetTextureObject(), m_samplers[1]);
		state.Constants.AddConstant(18, glm::vec4(chunk->Location.x * (m_realBaseLength - 1), chunk->Location.y * (m_realBaseLength - 1), chunk->RenderedLOD, m_realBaseLength - 1));
		state.Constants.AddConstant(19, glm::vec4(uvRatio));

		mManager.WriteIndirectCommandsToCullingPass(ghandles, boxes, state);
	}
}

TerrainChunk *TerrainAsset::PullChunkConcurrent(int address)
{
	if (IsChunkLoaded(address))
	{
		try
		{
			return m_addressMap[address];
		}
		catch (std::exception& e)
		{
			return nullptr;
		}
	}
	return nullptr;
}

void TerrainAsset::TryLoadChunkLODFromAsset(TerrainChunk *chunk, int lod)
{
	if (chunk->ChunkDataAsset)
	{
		IAssetStreamIO *stream = chunk->ChunkDataAsset->GetIOStream();
		stream->OpenStreamRead();
		stream->ReadString();

		while (!LockForWriting(ChunkToAddress(chunk->Location), lod));

		int w = m_chunkBaseLength * m_chunkBaseLength;
		int wM1 = (m_chunkBaseLength - 1) * (m_chunkBaseLength - 1);
		stream->ReadStream(chunk->LowerBoundHeights, wM1);
		for (int i = 0; i < lod; ++i)
		{
			stream->SeekStreamRelative(w * 5 * sizeof(float));
			w = glm::ceil(w / 4);
		}

		stream->ReadStream(chunk->Heights[lod], w);
		stream->ReadStream(chunk->AlphaMap[lod], w);
		stream->CloseStream();

		UnlockWriting(ChunkToAddress(chunk->Location), lod);
	}
}

void TerrainAsset::TrySaveChunkToAsset(TerrainChunk *chunk)
{
	if (!m_saveChangesOnUnload)
		return;

	if (!chunk->ChunkDataAsset)
	{
		chunk->ChunkDataAsset = dynamic_cast<UserAsset *>(
			Snowfall::GetGameInstance().GetAssetManager().CreateUserData(m_path + "Chunk" + std::to_string(ChunkToAddress(chunk->Location))));
		chunk->NewlySaved = true;
	}
	if (chunk->MaxLoadedLOD == 0)
	{
		PropagateLOD(chunk);
	}

	IAssetStreamIO *stream = chunk->ChunkDataAsset->GetIOStream();

	stream->OpenStreamWrite(true);
	stream->WriteString(chunk->ChunkDataAsset->GetPath());

	int w = m_chunkBaseLength * m_chunkBaseLength;
	int wM1 = (m_chunkBaseLength - 1) * (m_chunkBaseLength - 1);
	stream->WriteStream(chunk->LowerBoundHeights, wM1);
	for (int i = 0; i <= chunk->LowestLoadedLOD; ++i)
	{
		while (!LockForReading(ChunkToAddress(chunk->Location), i));
		if (i < chunk->MaxLoadedLOD)
		{
			stream->SeekStreamRelative(w * 5 * sizeof(float));
		}
		else
		{
			stream->WriteStream(chunk->Heights[i], w);
			stream->WriteStream(chunk->AlphaMap[i], w);
		}
		UnlockReading(ChunkToAddress(chunk->Location), i);
		w = glm::ceil(w / 4);
	}
	int pos = stream->GetStreamPosition();

	stream->CloseStream();

	m_addressToPath[ChunkToAddress(chunk->Location)] = chunk->ChunkDataAsset->GetPath();
}

void TerrainAsset::DisposeChunk(TerrainChunk *chunk)
{
	if (chunk->ChunkDataAsset && chunk->NewlySaved)
	{
		chunk->ChunkDataAsset->Unload();
		delete chunk->ChunkDataAsset;
	}
	chunk->ChunkDataAsset = nullptr;

	for (int i = 0; i < LOD_COUNT; ++i)
	{
		while (!LockForWriting(ChunkToAddress(chunk->Location), i));
		delete[] chunk->AlphaMap[i];
		delete[] chunk->Heights[i];
	}

	chunk->TerrainTexture->Unload();
	delete chunk->TerrainTexture;
	chunk->TerrainTexture = nullptr;

	chunk->NormalTexture->Unload();
	delete chunk->NormalTexture;
	chunk->NormalTexture = nullptr;

	delete[] chunk->LowerBoundHeights;

	delete chunk;
}

void TerrainAsset::PropagateLOD(TerrainChunk *chunk)
{
	if (!EnsureLOD(chunk, 0))
		return;
	int w0 = m_chunkBaseLength;
	int w1 = glm::ceil(static_cast<float>(m_chunkBaseLength) / 2);
	int addr = ChunkToAddress(chunk->Location);

	for (int i = 1; i < LOD_COUNT; ++i)
	{
		while (!LockForReading(addr, i - 1));
		while (!LockForWriting(addr, i));

		float *prevLodHeight = chunk->Heights[i - 1];
		float *thisLodHeight = chunk->Heights[i];

		glm::vec4 *prevLodAlpha = chunk->AlphaMap[i - 1];
		glm::vec4 *thisLodAlpha = chunk->AlphaMap[i];
		for (int y = 0; y < w1; ++y)
		{
			int y0 = y * 2;
			int y1 = y0 + 1;
			for (int x = 0; x < w1; ++x)
			{
				int x0 = x * 2;
				int x1 = x0 + 1;

				float maxHeight = x == w1 - 1 || y == w1 - 1 || x == 0 || y == 0 ? prevLodHeight[y0 * w0 + x0] : std::max({ prevLodHeight[y0 * w0 + x0], prevLodHeight[y0 * w0 + x1],
					prevLodHeight[y1 * w0 + x0], prevLodHeight[y1 * w0 + x1] });

				glm::vec4 avgAlpha = x == w1 - 1 || y == w1 - 1 || x == 0 || y == 0 ? prevLodAlpha[y0 * w0 + x0] : (prevLodAlpha[y0 * w0 + x0] + prevLodAlpha[y0 * w0 + x1]
					+ prevLodAlpha[y1 * w0 + x0] + prevLodAlpha[y1 * w0 + x1]) / 4.f;

				thisLodHeight[y * w1 + x] = maxHeight;
				thisLodAlpha[y * w1 + x] = avgAlpha;
			}
		}
		w0 = glm::ceil(static_cast<float>(w0) / 2);
		w1 = glm::ceil(static_cast<float>(w1) / 2);

		UnlockReading(addr, i - 1);
		UnlockWriting(addr, i);
	}
	chunk->MaxLoadedLOD = 0;
	chunk->LowestLoadedLOD = LOD_COUNT - 1;
}

bool TerrainAsset::EnsureLOD(TerrainChunk *chunk, int lod)
{
	if (!chunk)
		return false;
	return chunk->MaxLoadedLOD <= lod && lod <= chunk->LowestLoadedLOD;
}

void TerrainAsset::UpdateEdgeValuesWithNeighbors(TerrainChunk *chunk, int lod)
{
	int tAddr = ChunkToAddress(chunk->Location + glm::ivec2(0, 1));
	int trAddr = ChunkToAddress(chunk->Location + glm::ivec2(1, 1));
	int rAddr = ChunkToAddress(chunk->Location + glm::ivec2(1, 0));

	int bAddr = ChunkToAddress(chunk->Location + glm::ivec2(0, -1));
	int blAddr = ChunkToAddress(chunk->Location + glm::ivec2(-1, -1));
	int lAddr = ChunkToAddress(chunk->Location + glm::ivec2(-1, 0));

	int w = glm::ceil(m_chunkBaseLength / glm::pow(2, lod));

	TerrainChunk *top, *topRight, *right, *bottom, *bottomLeft, *left;
	
	if (IsChunkLoaded(tAddr) && EnsureLOD(top = m_addressMap[tAddr], lod))
	{
		memcpy(chunk->Heights[lod] + long long(w * (w - 1)), top->Heights[lod], sizeof(float) * (w - 1));
		memcpy(chunk->AlphaMap[lod] + long long(w * (w - 1)), top->AlphaMap[lod], sizeof(glm::vec4) * (w - 1));
	}

	if (IsChunkLoaded(trAddr) && EnsureLOD(topRight = m_addressMap[trAddr], lod))
	{
		chunk->Heights[lod][w * (w - 1) + (w - 1)] = topRight->Heights[lod][0];
		chunk->AlphaMap[lod][w * (w - 1) + (w - 1)] = topRight->AlphaMap[lod][0];
	}

	if (IsChunkLoaded(rAddr) && EnsureLOD(right = m_addressMap[rAddr], lod))
	{
		for (int i = 0; i < w - 1; ++i)
		{
			chunk->Heights[lod][w * i + w - 1] = right->Heights[lod][w * i];
			chunk->AlphaMap[lod][w * i + w - 1] = right->AlphaMap[lod][w * i];
		}
	}
	
	if (IsChunkLoaded(bAddr) && EnsureLOD(bottom = m_addressMap[bAddr], lod))
	{
		m_changedChunks.insert(bAddr);
		memcpy(bottom->Heights[lod] + long long(w * (w - 1)), chunk->Heights[lod], sizeof(float) * (w - 1));
		memcpy(bottom->AlphaMap[lod] + long long(w * (w - 1)), chunk->AlphaMap[lod], sizeof(glm::vec4) * (w - 1));
	}

	if (IsChunkLoaded(blAddr) && EnsureLOD(bottomLeft = m_addressMap[blAddr], lod))
	{
		m_changedChunks.insert(blAddr);
		bottomLeft->Heights[lod][w * (w - 1) + (w - 1)] = chunk->Heights[lod][0];
		bottomLeft->AlphaMap[lod][w * (w - 1) + (w - 1)] = chunk->AlphaMap[lod][0];
	}

	if (IsChunkLoaded(lAddr) && EnsureLOD(left = m_addressMap[lAddr], lod))
	{
		m_changedChunks.insert(lAddr);
		for (int i = 0; i < w - 1; ++i)
		{
			left->Heights[lod][w * i + w - 1] = chunk->Heights[lod][w * i];
			left->AlphaMap[lod][w * i + w - 1] = chunk->AlphaMap[lod][w * i];
		}
	}
}

void TerrainAsset::ReloadPageFile()
{
	if (!m_pageFileStream || !m_saveChangesOnUnload)
		return;
	m_addressToPath.clear();
	m_pageFileStream->OpenStreamRead();

	int sig = 0;
	m_pageFileStream->ReadStream(&sig, 1);
	if (sig != 123)
	{
		m_pageFileStream->CloseStream();
		return;
	}

	m_pageFileStream->ReadString();
	int entries = 0;
	m_pageFileStream->ReadStream(&m_minHeight, 1);
	m_pageFileStream->ReadStream(&m_maxHeight, 1);
	m_pageFileStream->ReadStream(&m_realBaseLength, 1);
	m_pageFileStream->ReadStream(&m_chunkBaseLength, 1);
	m_pageFileStream->ReadStream(&entries, 1);
	m_matPath = m_pageFileStream->ReadString();
	m_material = &AssetManager::LocateAssetGlobal<MaterialAsset>(m_matPath);
	for (int i = 0; i < entries; ++i)
	{
		int address = 0;
		m_pageFileStream->ReadStream(&address, 1);
		std::string path = m_pageFileStream->ReadString();

		m_addressToPath.insert_or_assign(address, path);
	}
	m_pageFileStream->CloseStream();
	m_loadedPageFile = true;
}

void TerrainAsset::SavePageFile()
{
	if (!m_pageFileStream || !m_saveChangesOnUnload)
		return;
	m_pageFileStream->OpenStreamWrite(false);
	m_pageFileStream->WriteStream(123);
	m_pageFileStream->WriteString(m_path);

	m_pageFileStream->WriteStream(m_minHeight);
	m_pageFileStream->WriteStream(m_maxHeight);
	m_pageFileStream->WriteStream(m_realBaseLength);
	m_pageFileStream->WriteStream(m_chunkBaseLength);

	m_pageFileStream->WriteStream<int>(m_addressToPath.size());
	m_pageFileStream->WriteString(m_matPath);
	for (auto kp : m_addressToPath)
	{
		m_pageFileStream->WriteStream(kp.first);
		m_pageFileStream->WriteString(kp.second);
	}
	m_pageFileStream->CloseStream();
}

bool TerrainAsset::LockForReading(int address, int lod)
{
	TerrainChunk *chunk = PullChunkConcurrent(address);
	if (!chunk)
		return true;
	if (chunk->LockWriteLOD[lod] == 1)
		return false;
	++chunk->LockReadLOD[lod];
	return true;
}

bool TerrainAsset::LockForWriting(int address, int lod)
{
	TerrainChunk *chunk = PullChunkConcurrent(address);
	if (!chunk)
		return true;
	if (chunk->LockReadLOD[lod] >= 1 || chunk->LockWriteLOD[lod] == 1)
		return false;
	++chunk->LockWriteLOD[lod];
	return true;
}

void TerrainAsset::UnlockReading(int address, int lod)
{
	TerrainChunk *chunk = PullChunkConcurrent(address);
	if (!chunk)
		return;
	--chunk->LockReadLOD[lod];
}

void TerrainAsset::UnlockWriting(int address, int lod)
{
	TerrainChunk *chunk = PullChunkConcurrent(address);
	if (!chunk)
		return;
	--chunk->LockWriteLOD[lod];
}

void TerrainAsset::LoadGeometries()
{
	if (!m_geometriesLoaded)
	{
		MeshManager& mManager = Snowfall::GetGameInstance().GetMeshManager();

		for (int i = 0; i < LOD_COUNT; ++i)
		{
			int len = glm::ceil(m_chunkBaseLength / glm::pow(2, i));
			int len2 = len * len;
			float oneOverLen = 1.f / (len - 1);

			std::vector<glm::vec2> uv(len2);
			std::vector<int> indices(len2 * 6);

			GeometryHandle& handle = m_geometries[i] = mManager.CreateGeometry(len2, len2 * 6, TERRAIN_BUFFER_ID);
			int y = 0;
			for (float uY = 0; y < len; uY += oneOverLen, ++y)
			{
				int x = 0;
				for (float uX = 0; x < len; uX += oneOverLen, ++x)
				{
					int m11 = y * len + x;
					if (y > 0 && x > 0)
					{
						int m00 = (y - 1) * len + x - 1;
						int m10 = (y - 1) * len + x;
						int m01 = y * len + x - 1;

						indices[m00 * 6] = m11;
						indices[m00 * 6 + 1] = m10;
						indices[m00 * 6 + 2] = m00;
						indices[m00 * 6 + 3] = m01;
						indices[m00 * 6 + 4] = m11;
						indices[m00 * 6 + 5] = m00;
					}

					uv[m11] = glm::vec2(uX, uY);
				}
			}

			mManager.WriteGeometryVertices(handle, uv.data(), 0, uv.size());
			mManager.WriteGeometryIndices(handle, indices.data(), 0, indices.size());
		}
		m_geometriesLoaded = true;
	}
}
