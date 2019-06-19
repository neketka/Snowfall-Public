#include "stdafx.h"

#include "MaterialAsset.h"

MaterialAsset::MaterialAsset(std::string path, IAssetStreamIO *stream) : m_inMemory(false), m_loaded(false), m_loadSuccess(true), m_stream(stream), m_path(path)
{
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
		for (unsigned i = 0; i < counts[0]; ++i)
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
		for (unsigned i = 0; i < counts[1]; ++i)
			m_textures.push_back(&AssetManager::LocateAssetGlobal<TextureAsset>(m_stream->ReadString()));
		for (unsigned i = 0; i < counts[2]; ++i)
		{
			int data[2];
			m_stream->ReadStream(data, 2);
			tsPairs.push_back({ data[0], data[1] });
		}
		for (unsigned i = 0; i < counts[3]; ++i)
		{
			int data[2];
			m_stream->ReadStream(data, 2);
			m_baseProperties.Constants.AddConstant(data[0], data[1]);
		}
		for (unsigned i = 0; i < counts[4]; ++i)
		{
			int loc;
			float val;
			m_stream->ReadStream(&loc, 1);
			m_stream->ReadStream(&val, 1);
			m_baseProperties.Constants.AddConstant(loc, val);
		}
		for (unsigned i = 0; i < counts[5]; ++i)
		{
			int loc;
			glm::vec2 val;
			m_stream->ReadStream(&loc, 1);
			m_stream->ReadStream(&val, 1);
			m_baseProperties.Constants.AddConstant(loc, val);
		}
		for (unsigned i = 0; i < counts[6]; ++i)
		{
			int loc;
			glm::vec3 val;
			m_stream->ReadStream(&loc, 1);
			m_stream->ReadStream(&val, 1);
			m_baseProperties.Constants.AddConstant(loc, val);
		}
		for (unsigned i = 0; i < counts[7]; ++i)
		{
			int loc;
			glm::vec4 val;
			m_stream->ReadStream(&loc, 1);
			m_stream->ReadStream(&val, 1);
			m_baseProperties.Constants.AddConstant(loc, val);
		}
		for (unsigned i = 0; i < counts[8]; ++i)
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

void MaterialAsset::CreateRenderPass(CommandBuffer& buffer, Pipeline& pipeline, std::set<std::string> variant)
{
	Material& mat = GetMaterial();
	pipeline.Shader = mat.MaterialShader->GetShaderVariant(variant);
	buffer.BindPipelineCommand(pipeline);
	buffer.BindConstantsCommand(mat.Constants);
}

IAsset *MaterialAsset::CreateCopy(std::string newPath)
{
	if (m_inMemory)
		return new MaterialAsset(newPath, m_samplers, m_textures, m_smProperies, m_baseProperties);
	else if (m_permanentStore)
		return new MaterialAsset(newPath, m_baseProperties);
	return new MaterialAsset(newPath, m_stream);
}

void MaterialAsset::Export()
{
	if (m_stream)
	{
		m_stream->OpenStreamWrite();
		auto consts = m_baseProperties.Constants;
		
		m_stream->WriteString(m_path);

		unsigned counts[] = { 
			static_cast<unsigned>(m_samplers.size()), static_cast<unsigned>(m_textures.size()), static_cast<unsigned>(m_smProperies.size()),
			static_cast<unsigned>(consts.GetIntBindings().size()), static_cast<unsigned>(consts.GetFloatBindings().size()), 
			static_cast<unsigned>(consts.GetVec2Bindings().size()), static_cast<unsigned>(consts.GetVec3Bindings().size()), 
			static_cast<unsigned>(consts.GetVec4Bindings().size()), static_cast<unsigned>(m_smProperies.size())
		};

		m_stream->WriteStream(counts, 9);
		m_stream->WriteStream(&m_baseProperties.PerObjectParameterCount, 1);
		m_stream->WriteString(m_baseProperties.MaterialShader->GetPath());

		for (Sampler s : m_samplers)
		{
			unsigned data[] = { 
				static_cast<unsigned>(s.GetWrapMode(TextureChannel::S)), static_cast<unsigned>(s.GetWrapMode(TextureChannel::T)),
				static_cast<unsigned>(s.GetWrapMode(TextureChannel::R)), static_cast<unsigned>(s.GetMinificationFilter()),
				static_cast<unsigned>(s.GetMagnificationFilter())
			};
			char aniso = s.GetAnisotropicFiltering() ? 1 : 0;
			m_stream->WriteStream(data, 5);
			m_stream->WriteStream(&aniso, 1);
		}

		for (TextureAsset *tex : m_textures)
			m_stream->WriteString(tex->GetPath());

		for (SamplerProperty prop : m_smProperies)
		{
			m_stream->WriteStream(prop.SamplerIndex);
			m_stream->WriteStream(prop.TextureIndex);
		}

		for (std::pair<int, int> iVal : consts.GetIntBindings())
		{
			m_stream->WriteStream(iVal.first);
			m_stream->WriteStream(iVal.second);
		}

		for (std::pair<int, float> fVal : consts.GetFloatBindings())
		{
			m_stream->WriteStream(fVal.first);
			m_stream->WriteStream(fVal.second);
		}

		for (std::pair<int, glm::vec2> v2Val : consts.GetVec2Bindings())
		{
			m_stream->WriteStream(v2Val.first);
			m_stream->WriteStream(v2Val.second);
		}
		
		for (std::pair<int, glm::vec3> v3Val : consts.GetVec3Bindings())
		{
			m_stream->WriteStream(v3Val.first);
			m_stream->WriteStream(v3Val.second);
		}

		for (std::pair<int, glm::vec4> v4Val : consts.GetVec4Bindings())
		{
			m_stream->WriteStream(v4Val.first);
			m_stream->WriteStream(v4Val.second);
		}

		int i = 0;
		for (SamplerProperty prop : m_smProperies)
		{
			m_stream->WriteStream(prop.UniformIndex);
			m_stream->WriteStream(i);
			++i;
		}

		m_stream->CloseStream();
	}
}

void MaterialAsset::SetStream(IAssetStreamIO *stream)
{
	m_inMemory = false;
	m_permanentStore = false;
	m_stream = stream;
}

std::vector<std::string> MaterialAssetReader::GetExtensions()
{
	return { ".mtasset" };
}

void MaterialAssetReader::LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager)
{
	stream->OpenStreamRead();
	std::string path = stream->ReadString();
	stream->CloseStream();

	assetManager.AddAsset(new MaterialAsset(path, stream));
}
