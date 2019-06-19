#include "stdafx.h"

#include "MeshAsset.h"
#include "Snowfall.h"

#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

MeshAsset::MeshAsset(std::string path, Mesh mesh) : m_inMemory(true), m_loaded(false), m_path(path), m_loadSuccess(true)
{
	m_mesh = new Mesh;
	m_mesh->Vertices = mesh.Vertices;
	m_mesh->Indices = mesh.Indices;
	m_handle = GeometryHandle();
}

MeshAsset::MeshAsset(std::string path, IAssetStreamIO *stream) : m_inMemory(false), m_loaded(false), m_loadSuccess(true), m_stream(stream), m_path(path), m_mesh()
{
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
		m_loaded = true;
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
		Snowfall::GetGameInstance().GetMeshManager().EraseGeometry(m_handle);
		m_handle = GeometryHandle();
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
		m_loaded = true;
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

void MeshAsset::DrawMeshDirect(CommandBuffer& buffer, int instances)
{
	GeometryHandle handle = GetGeometry();
	buffer.DrawIndexedCommand(PrimitiveType::Triangles, handle.IndexAlloc.GetLength(), instances,
		handle.IndexAlloc.GetPosition() * 4, handle.VertexAlloc.GetPosition(), 0);
}

IAsset *MeshAsset::CreateCopy(std::string newPath)
{
	if (m_inMemory)
		return new MeshAsset(newPath, *m_mesh);
	return new MeshAsset(newPath, m_stream);
}

void MeshAsset::Export()
{
}

void MeshAsset::SetStream(IAssetStreamIO *stream)
{
}

std::vector<std::string> MeshAssetReader::GetExtensions()
{
	return { ".masset" };
}

void MeshAssetReader::LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager)
{
	stream->OpenStreamRead();
	std::string path = stream->ReadString();
	stream->CloseStream();

	assetManager.AddAsset(new MeshAsset(path, stream));
}
