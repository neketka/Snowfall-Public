#pragma once
#include "AssetManager.h"
#include "Material.h"
#include "TextureAsset.h"
#include <vector>

class SamplerProperty
{
public:
	int SamplerIndex;
	int TextureIndex;
	int UniformIndex;
};

class MaterialAsset : public IAsset
{
public:
	MaterialAsset(IAssetStreamIO *stream);
	MaterialAsset(std::string path, Material material);
	MaterialAsset(std::string path, std::vector<Sampler> sampler, std::vector<TextureAsset *> textures, std::vector<SamplerProperty> properties, Material baseProperties);
	~MaterialAsset();

	virtual std::string GetPath() const override;
	virtual void Load() override;
	virtual void Unload() override;
	virtual bool IsReady() override;
	virtual bool IsValid() override;

	Material& GetMaterial();
	Material& GetBaseProperties();
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

class MaterialAssetReader : public IAssetReader
{
	virtual std::vector<std::string> GetExtensions() override;
	virtual void LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager) override;
};
