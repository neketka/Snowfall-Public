#include "MaterialAsset.h"

MaterialAsset::MaterialAsset(IAssetStreamIO *stream) : m_inMemory(false), m_loaded(false), m_loadSuccess(true), m_stream(stream)
{
	stream->OpenStreamRead();
	m_path = stream->ReadString();
	stream->CloseStream();
}

MaterialAsset::MaterialAsset(std::string path, Material material) : m_permanentStore(true), m_baseProperties(material), m_path(path)
{
}

MaterialAsset::MaterialAsset(std::string path, std::vector<Sampler> sampler, std::vector<TextureAsset *> textures, std::vector<SamplerProperty> smProperty, Material baseProperties)
	: m_inMemory(true), m_loaded(true), m_path(path), m_loadSuccess(true), m_samplers(sampler), m_baseProperties(baseProperties), m_textures(textures), m_smProperies(smProperty)
{
}

MaterialAsset::~MaterialAsset()
{
	if (m_inMemory)
	{
		for (Sampler s : m_samplers)
			s.Destroy();
	}
	delete m_stream;
}

std::string MaterialAsset::GetPath() const
{
	return m_path;
}

void MaterialAsset::Load()
{
	if (!m_inMemory && !m_loaded)
	{
		m_stream->OpenStreamRead();
		m_baseProperties = Material();
		m_stream->SeekStream(sizeof(unsigned int) + m_path.length());

		unsigned counts[9]; //sampler, textures, tspairs, int, float, vec2, vec3, vec4, tspair index
		std::vector<std::pair<int, int>> tsPairs(counts[8]);
		m_stream->ReadStream(counts, 9);
		m_stream->ReadStream(&m_material.PerObjectParameterCount, 1);
		m_baseProperties.MaterialShader = &AssetManager::LocateAssetGlobal<ShaderAsset>(m_stream->ReadString());
		for (int i = 0; i < counts[0]; ++i)
		{
			unsigned data[5];
			bool aniso = false;
			m_stream->ReadStream(data, 5);
			m_stream->ReadStream(&aniso, 1);
			Sampler s;
			s.SetWrapMode(TextureChannel::S, static_cast<WrapMode>(data[0]));
			s.SetWrapMode(TextureChannel::T, static_cast<WrapMode>(data[1]));
			s.SetWrapMode(TextureChannel::R, static_cast<WrapMode>(data[2]));
			s.SetMinificationFilter(static_cast<MinificationFilter>(data[3]));
			s.SetMagnificationFilter(static_cast<MagnificationFilter>(data[4]));
			s.SetAnisotropicFiltering(aniso);
			m_samplers.push_back(s);
		}
		for (int i = 0; i < counts[1]; ++i)
			m_textures.push_back(&AssetManager::LocateAssetGlobal<TextureAsset>(m_stream->ReadString()));
		for (int i = 0; i < counts[2]; ++i)
		{
			int data[2];
			m_stream->ReadStream(data, 2);
			tsPairs.push_back({ data[0], data[1] });
		}
		for (int i = 0; i < counts[3]; ++i)
		{
			int data[2];
			m_stream->ReadStream(data, 2);
			m_baseProperties.Constants.AddConstant(data[0], data[1]);
		}
		for (int i = 0; i < counts[4]; ++i)
		{
			int loc;
			float val;
			m_stream->ReadStream(&loc, 1);
			m_stream->ReadStream(&val, 1);
			m_baseProperties.Constants.AddConstant(loc, val);
		}
		for (int i = 0; i < counts[5]; ++i)
		{
			int loc;
			glm::vec2 val;
			m_stream->ReadStream(&loc, 1);
			m_stream->ReadStream(&val, 1);
			m_baseProperties.Constants.AddConstant(loc, val);
		}
		for (int i = 0; i < counts[6]; ++i)
		{
			int loc;
			glm::vec3 val;
			m_stream->ReadStream(&loc, 1);
			m_stream->ReadStream(&val, 1);
			m_baseProperties.Constants.AddConstant(loc, val);
		}
		for (int i = 0; i < counts[7]; ++i)
		{
			int loc;
			glm::vec4 val;
			m_stream->ReadStream(&loc, 1);
			m_stream->ReadStream(&val, 1);
			m_baseProperties.Constants.AddConstant(loc, val);
		}
		for (int i = 0; i < counts[5]; ++i)
		{
			int data[2];
			m_stream->ReadStream(data, 2);
			SamplerProperty prop;
			prop.UniformIndex = data[0];
			prop.SamplerIndex = tsPairs[data[1]].first;
			prop.TextureIndex = tsPairs[data[1]].second;
		}
		m_stream->CloseStream();
		m_loaded = true;
	}
}

void MaterialAsset::Unload()
{
	if (!m_inMemory)
	{
		for (Sampler s : m_samplers)
			s.Destroy();
		for (TextureAsset *ta : m_textures)
			ta->Unload();
	}
	m_loaded = false;
}

bool MaterialAsset::IsReady()
{
	return m_loaded;
}

bool MaterialAsset::IsValid()
{
	return true;
}

Material& MaterialAsset::GetMaterial()
{
	if (m_permanentStore)
		return m_baseProperties;
	if (!m_loaded)
		Load();
	m_material = m_baseProperties;
	for (SamplerProperty prop : m_smProperies)
		m_material.Constants.AddConstant(prop.UniformIndex, m_textures[prop.TextureIndex]->GetTextureObject(), m_samplers[prop.SamplerIndex]);
	return m_material;
}

Material& MaterialAsset::GetBaseProperties()
{
	return m_baseProperties;
}

std::vector<std::string> MaterialAssetReader::GetExtensions()
{
	return { ".mtasset" };
}

void MaterialAssetReader::LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager)
{
	assetManager.AddAsset(new MaterialAsset(stream));
}
