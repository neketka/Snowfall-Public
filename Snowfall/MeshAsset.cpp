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

MeshAsset::MeshAsset(std::string path, MeshFormat format, MeshStreamReadingHint readingHint, IAssetStreamSource *stream) : m_inMemory(false), m_loaded(false), m_path(path + "." + readingHint.ObjectName), m_format(format), m_loadSuccess(true), m_stream(stream), m_hint(readingHint)
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
		if (m_format == MeshFormat::Obj)
		{
			m_stream->OpenStream();
			m_stream->SeekStream(m_hint.StreamPosBegin);
			char *buffer = new char[m_hint.StreamReadLength];
			m_stream->ReadStream(buffer, m_hint.StreamReadLength);
			m_stream->CloseStream();
			std::istringstream sstream(buffer);

			std::vector<glm::vec3> positions;
			std::vector<glm::vec3> normals;
			std::vector<glm::vec2> texcoords;
			m_mesh = new Mesh;

			int fIndex = 0;
			std::string line;
			while (std::getline(sstream, line))
			{
				std::vector<std::string> tokens;
				boost::split(tokens, line, boost::is_any_of(" "));
				if (tokens.size() == 0)
					continue;
				if (tokens[0] == "v")
				{
					if (tokens.size() < 4)
						continue;
					positions.push_back(glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
				}
				else if (tokens[0] == "vn")
				{
					if (tokens.size() < 4)
						continue;
					normals.push_back(glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
				}
				else if (tokens[0] == "vt")
				{
					if (tokens.size() < 3)
						continue;
					texcoords.push_back(glm::vec2(std::stof(tokens[1]), std::stof(tokens[2])));
				}
				else if (tokens[0] == "f")
				{
					if (tokens.size() < 4)
						continue;
					std::string splits[3] = {tokens[1], tokens[2], tokens[3]};
					for (int i = 0; i < 3; ++i)
					{
						std::vector<std::string> indices;
						boost::split(indices, splits[i], boost::is_any_of("/"));
						glm::vec3 pos = positions[std::stoi(indices[0]) - 1];
						glm::vec2 uv;
						glm::vec3 normal;

						if (indices.size() > 1)
						{
							if (indices[1] != "")
								uv = texcoords[std::stoi(indices[1]) - 1];
							if (indices.size() > 2)
								normal = normals[std::stoi(indices[2]) - 1];
						}

						m_mesh->Vertices.push_back(RenderVertex(pos, glm::vec4(1, 1, 1, 1), normal, uv));
						m_mesh->Indices.push_back(fIndex++);
					}
				}
				m_loaded = true;
			}

			delete[] buffer;
		}
	}
}

void MeshAsset::Unload()
{
	if (!m_inMemory)
		delete m_mesh;
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
	return { ".obj" };
}

void MeshAssetReader::LoadAssets(std::string ext, std::string path, IAssetStreamSource *streamSource, AssetManager& assetManager)
{
	if (ext == ".obj")
	{
		streamSource->OpenStream();
		char *buffer = new char[streamSource->GetStreamLength()];
		streamSource->ReadStream(buffer, streamSource->GetStreamLength());
		streamSource->CloseStream();
		std::istringstream sstream(buffer);
		std::string line;
		MeshStreamReadingHint hint;
		bool stopNext = false;
		while (std::getline(sstream, line))
		{
			if (line[0] == 'o')
			{
				if (stopNext)
				{
					hint.StreamReadLength = static_cast<int>(sstream.tellg()) - hint.StreamPosBegin;
					assetManager.AddAsset(new MeshAsset(path, MeshFormat::Obj, hint, streamSource));
				}
				hint.ObjectName = line.substr(2, line.length() - 2);
				hint.StreamPosBegin = sstream.tellg();
				stopNext = true;
			}
		}
		if (stopNext)
		{
			hint.StreamReadLength = streamSource->GetStreamLength() - hint.StreamPosBegin;
			assetManager.AddAsset(new MeshAsset(path, MeshFormat::Obj, hint, streamSource));
		}
	}
}
