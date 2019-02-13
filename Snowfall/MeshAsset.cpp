#include "MeshAsset.h"
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
		m_loaded = true;
	}
}

void MeshAsset::Unload()
{
	if (!m_inMemory)
		delete m_mesh;
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
