#include "stdafx.h"

#include "ShaderAsset.h"
#include "ShaderPreprocessor.h"
#include "NullStreamSource.h"

ShaderAsset::ShaderAsset(std::string path, std::string src) : m_path(path), m_stream(new NullStreamSource()), m_rawSource(src), m_compileSuccess(true)
{
}

ShaderAsset::ShaderAsset(std::string path, IAssetStreamIO *stream) : m_stream(stream), m_isStreamedSource(true), m_compileSuccess(true), m_path(path)
{
}

ShaderAsset::~ShaderAsset()
{
	Unload();
	if (m_stream)
		delete m_stream;
}

Shader& ShaderAsset::GetShaderVariant(std::set<std::string> qualifiers)
{
	if (!IsReady())
		Load();
	auto shader = m_compiledShaders.find(qualifiers);

	if (shader != m_compiledShaders.end())
		return *shader->second;

	std::string variant;
	int i = 0;
	for (std::string q : qualifiers)
	{
		variant += q + ((i + 1) == qualifiers.size() ? "" : ", ");
		++i;
	}

	Snowfall::GetGameInstance().Log(LogType::Message, "Compiling shader: " + m_path + "{ " + variant + " }");
	Shader *s = new Shader(m_preprocessed.GetProcessedSource(), qualifiers);
	m_compiledShaders.insert({ qualifiers, s });

	if (!s->IsCompileSuccess())
		m_compileSuccess = false;
	return *s;
}

void ShaderAsset::Load()
{
	if (m_loaded)
		return;
	if (m_isStreamedSource)
	{
		m_stream->OpenStreamRead();
		m_stream->SeekStream(sizeof(unsigned int) + m_path.length());
		m_rawSource = m_stream->ReadString();
	}
	auto spreprocess = Snowfall::GetGameInstance().GetShaderPreprocessor();
	m_preprocessed = spreprocess.PreprocessShader(m_rawSource);

	m_loaded = true;
}

void ShaderAsset::Unload()
{
	for (auto compShader : m_compiledShaders)
	{
		compShader.second->Destroy();
		delete compShader.second;
	}
	m_loaded = false;
}

bool ShaderAsset::IsReady()
{
	return m_loaded;
}

bool ShaderAsset::IsValid()
{
	return m_compileSuccess;
}

IAsset *ShaderAsset::CreateCopy(std::string newPath)
{
	return nullptr;
}

void ShaderAsset::Export()
{
}

void ShaderAsset::SetStream(IAssetStreamIO *stream)
{
}

ShaderAssetReader::ShaderAssetReader()
{
}

std::vector<std::string> ShaderAssetReader::GetExtensions()
{
	return std::vector<std::string>({ ".sasset" });
}

void ShaderAssetReader::LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager)
{
	stream->OpenStreamRead();
	std::string path = stream->ReadString();
	stream->CloseStream();
	assetManager.AddAsset(new ShaderAsset(path, stream));
}
