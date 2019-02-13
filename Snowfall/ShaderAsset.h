#pragma once
#include "AssetManager.h"
#include "Shader.h"
#include <map>
#include <set>

class ShaderAsset : public IAsset
{
public:
	ShaderAsset(std::string path, std::string src);
	ShaderAsset(IAssetStreamIO *stream);
	~ShaderAsset();

	inline std::string GetSource() 
	{
		if (!IsReady())
			Load(); 
		return m_processedSource; 
	}

	Shader& GetShaderVariant(std::set<std::string> qualifiers);

	virtual std::string GetPath() const override 
	{
		return m_path;
	}

	virtual void Load() override;
	virtual void Unload() override;
	virtual bool IsReady() override;
	virtual bool IsValid() override;
private:
	bool m_loaded;
	bool m_compileSuccess;
	bool m_isStreamedSource;

	IAssetStreamIO *m_stream;
	std::string m_path;
	std::string m_rawSource;
	std::string m_processedSource;
	std::map<std::set<std::string>, Shader *> m_compiledShaders;
};

class ShaderAssetReader : public IAssetReader
{
public:
	ShaderAssetReader();

	virtual std::vector<std::string> GetExtensions() override;
	virtual void LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager) override;
};