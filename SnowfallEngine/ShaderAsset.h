#pragma once
#include "AssetManager.h"
#include "Shader.h"
#include <map>
#include <set>

#include "export.h"

class ShaderAsset : public IAsset
{
public:
	SNOWFALLENGINE_API ShaderAsset(std::string path, std::string src);
	SNOWFALLENGINE_API ShaderAsset(IAssetStreamIO *stream);
	SNOWFALLENGINE_API ~ShaderAsset();

	inline std::string GetSource() 
	{
		if (!IsReady())
			Load(); 
		return m_processedSource; 
	}

	SNOWFALLENGINE_API Shader& GetShaderVariant(std::set<std::string> qualifiers);

	virtual std::string GetPath() const override 
	{
		return m_path;
	}

	SNOWFALLENGINE_API virtual void Load() override;
	SNOWFALLENGINE_API virtual void Unload() override;
	SNOWFALLENGINE_API virtual bool IsReady() override;
	SNOWFALLENGINE_API virtual bool IsValid() override;

	virtual IAsset *CreateCopy(std::string newPath, IAssetStreamIO *output) override;
	virtual void Export() override;
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

class SNOWFALLENGINE_API ShaderAssetReader : public IAssetReader
{
public:
	ShaderAssetReader();

	virtual std::vector<std::string> GetExtensions() override;
	virtual void LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager) override;
};