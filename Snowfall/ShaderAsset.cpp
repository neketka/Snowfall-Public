#include "ShaderAsset.h"
#include "ShaderPreprocessor.h"
#include "Snowfall.h"
#include "NullStreamSource.h"

ShaderAsset::ShaderAsset(std::string path, std::string src) : m_path(path), m_stream(new NullStreamSource()), m_rawSource(src), m_compileSuccess(true)
{
}

ShaderAsset::ShaderAsset(IAssetStreamSource *stream) : m_stream(stream), m_isStreamedSource(true), m_compileSuccess(true)
{
	stream->OpenStream();
	unsigned int size = 0;
	stream->ReadStream(&size, sizeof(unsigned int));
	char *buffer = new char[size + 1];
	buffer[size] = '\0';
	stream->ReadStream(buffer, size);
	stream->CloseStream();

	m_path = std::string(buffer);
	delete buffer;
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
		m_stream->SeekStream(sizeof(unsigned int) + m_path.length());
		unsigned int slen = 0;
		m_stream->ReadStream(&slen, sizeof(unsigned int));
		char *buffer = new char[slen + 1];
		buffer[slen] = '\0';
		m_stream->ReadStream(buffer, slen);
		int len = m_stream->GetStreamLength();
		m_stream->CloseStream();

		m_rawSource = std::string(buffer);
		delete buffer;
	}
	auto spreprocess = Snowfall::GetGameInstance().GetShaderPreprocessor();
	auto preshader = spreprocess.PreprocessShader(m_rawSource);
	m_processedSource = preshader.GetProcessedSource();
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

ShaderAssetReader::ShaderAssetReader()
{
}

std::vector<std::string> ShaderAssetReader::GetExtensions()
{
	return std::vector<std::string>({ ".sasset" });
}

void ShaderAssetReader::LoadAssets(std::string ext, IAssetStreamSource *streamSource, AssetManager& assetManager)
{
	assetManager.AddAsset(new ShaderAsset(streamSource));
}
