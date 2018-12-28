#include "ShaderAsset.h"
#include "ShaderPreprocessor.h"
#include "Snowfall.h"
#include "NullStreamSource.h"

ShaderAsset::ShaderAsset(std::string path, std::string src) : m_path(path), m_stream(new NullStreamSource()), m_rawSource(src), m_compileSuccess(true)
{
}

ShaderAsset::ShaderAsset(std::string path, IAssetStreamSource *stream) : m_path(path), m_stream(stream), m_isStreamedSource(true), m_compileSuccess(true)
{
}

ShaderAsset::~ShaderAsset()
{
	Unload();
	delete m_stream;
}

Shader& ShaderAsset::GetShaderVariant(std::set<std::string> qualifiers)
{
	if (!IsReady())
		Load();
	auto shader = m_compiledShaders.find(qualifiers);

	if (shader != m_compiledShaders.end())
		return *shader->second;

	Shader *s = new Shader(m_processedSource, qualifiers);
	m_compiledShaders.insert({ qualifiers, s });

	if (!s->IsCompileSuccess())
		m_compileSuccess = false;
	return *s;
}

void ShaderAsset::Load()
{
	if (m_isStreamedSource)
	{
		m_stream->OpenStream();
		std::vector<char> vec = std::vector<char>(m_stream->GetStreamLength());
		m_stream->ReadStream(vec.data(), m_stream->GetStreamLength());
		m_rawSource = std::string(vec.data());
		m_stream->CloseStream();
	}
	auto spreprocess = Snowfall::GetGameInstance().GetShaderPreprocessor();
	auto preshader = spreprocess.PreprocessShader(m_rawSource);
	m_processedSource = preshader.GetProcessedSource();
	m_loaded = false;
}

void ShaderAsset::Unload()
{
	for (auto compShader : m_compiledShaders)
	{
		compShader.second->Destroy();
		delete compShader.second;
	}
}

bool ShaderAsset::IsReady()
{
	return m_loaded;
}

bool ShaderAsset::IsValid()
{
	return m_compileSuccess;
}

ShaderAssetReader::ShaderAssetReader()
{
}

std::vector<std::string> ShaderAssetReader::GetExtensions()
{
	return std::vector<std::string>({ ".glsl" });
}

void ShaderAssetReader::LoadAssets(std::string ext, std::string path, IAssetStreamSource *streamSource, AssetManager& assetManager)
{
	assetManager.AddAsset(new ShaderAsset(path, streamSource));
}
