#pragma once
#include "AssetManager.h"
#include "Material.h"
#include "TextureAsset.h"
#include <vector>

#include "export.h"

class SamplerProperty
{
public:
	SamplerProperty(int sIndex, int tIndex, int uIndex) : SamplerIndex(sIndex), TextureIndex(tIndex), UniformIndex(uIndex) {}
	SamplerProperty() {}
	int SamplerIndex;
	int TextureIndex;
	int UniformIndex;
};

class MaterialAsset : public IAsset
{
public:
	SNOWFALLENGINE_API MaterialAsset(IAssetStreamIO *stream);
	SNOWFALLENGINE_API MaterialAsset(std::string path, Material material);
	SNOWFALLENGINE_API MaterialAsset(std::string path, std::vector<Sampler> sampler, std::vector<TextureAsset *> textures, std::vector<SamplerProperty> properties, Material baseProperties);
	SNOWFALLENGINE_API ~MaterialAsset();

	SNOWFALLENGINE_API virtual std::string GetPath() const override;
	SNOWFALLENGINE_API virtual void Load() override;
	SNOWFALLENGINE_API virtual void Unload() override;
	SNOWFALLENGINE_API virtual bool IsReady() override;
	SNOWFALLENGINE_API virtual bool IsValid() override;

	SNOWFALLENGINE_API Material& GetMaterial();
	SNOWFALLENGINE_API Material& GetBaseProperties();
private:
	std::vector<Sampler> m_samplers;
	std::vector<TextureAsset *> m_textures;
	std::vector<SamplerProperty> m_smProperies;
	Material m_material;
	Material m_baseProperties;
	bool m_inMemory;
	bool m_loaded;
	bool m_loadSuccess;
	bool m_permanentStore;
	IAssetStreamIO *m_stream;
	std::string m_path;
};

class SNOWFALLENGINE_API MaterialAssetReader : public IAssetReader
{
	virtual std::vector<std::string> GetExtensions() override;
	virtual void LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager) override;
};
