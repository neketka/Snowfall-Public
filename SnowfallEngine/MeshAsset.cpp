#include "MeshAsset.h"
#include "Snowfall.h"

#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

MeshAsset::MeshAsset(std::string path, Mesh mesh) : m_inMemory(true), m_loaded(true), m_path(path), m_loadSuccess(true)
{
	m_mesh = new Mesh;
	m_mesh->Vertices = mesh.Vertices;
	m_mesh->Indices = mesh.Indices;
}

MeshAsset::MeshAsset(IAssetStreamIO *stream) : m_inMemory(false), m_loaded(false), m_loadSuccess(true), m_stream(stream)
{
	stream->OpenStreamRead();
	m_path = stream->ReadString();
	stream->CloseStream();
}

MeshAsset::~MeshAsset()
{
	if (m_inMemory)
		delete m_mesh;
	delete m_stream;
}

std::string MeshAsset::GetPath() const
{
	return m_path;
}

void MeshAsset::Load()
{
	if (!m_inMemory && !m_loaded)
	{
		m_stream->OpenStreamRead();
		m_stream->SeekStream(sizeof(unsigned int) + m_path.length());
		int vlen = 0;
		int ilen = 0;
		m_stream->ReadStream(&vlen, 1);
		m_stream->ReadStream(&ilen, 1);
		
		m_mesh = new Mesh(std::vector<RenderVertex>(vlen), std::vector<int>(ilen));

		m_stream->ReadStream(m_mesh->Vertices.data(), vlen);
		m_stream->ReadStream(m_mesh->Indices.data(), ilen);

		m_stream->CloseStream();
	}
}

void MeshAsset::Unload()
{
	if (!m_loaded)
		return;
	if (!m_inMemory)
		delete m_mesh;
	if (m_handle.VertexAlloc.Allocated())
	{
		m_handle = GeometryHandle();
		Snowfall::GetGameInstance().GetMeshManager().EraseGeometry(m_handle);
	}
	m_loaded = false;
}

bool MeshAsset::IsReady()
{
	return m_loaded;
}

bool MeshAsset::IsValid()
{
	return m_loadSuccess;
}

GeometryHandle& MeshAsset::GetGeometry()
{
	if (!IsReady())
		Load();
	if (!m_handle.VertexAlloc.Allocated())
	{
		m_handle = Snowfall::GetGameInstance().GetMeshManager().CreateGeometry(m_mesh->Vertices.size(), m_mesh->Indices.size());
		Snowfall::GetGameInstance().GetMeshManager().WriteGeometryVertices(m_handle, m_mesh->Vertices.data(), 0, m_mesh->Vertices.size());
		Snowfall::GetGameInstance().GetMeshManager().WriteGeometryIndices(m_handle, m_mesh->Indices.data(), 0, m_mesh->Indices.size());
	}
	return m_handle;
}

Mesh& MeshAsset::GetMesh()
{
	if (!IsReady())
		Load();
	return *m_mesh;
}

std::vector<std::string> MeshAssetReader::GetExtensions()
{
	return { ".masset" };
}

void MeshAssetReader::LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager)
{
	assetManager.AddAsset(new MeshAsset(stream));
}
