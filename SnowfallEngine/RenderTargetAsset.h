#pragma once
#include "AssetManager.h"
#include "TextureAsset.h"
#include "Framebuffer.h"

#include "export.h"
class TextureLayerAttachment
{
public: 
	TextureLayerAttachment() {}
	TextureLayerAttachment(int tIndex, int level, int layer) : TextureIndex(tIndex), Level(level), Layer(layer) {}
	int TextureIndex;
	int Level;
	int Layer;
};

class RenderTargetAsset : public IAsset
{
public:
	SNOWFALLENGINE_API RenderTargetAsset(std::string path, std::vector<TextureAsset *> textures, std::vector<TextureLayerAttachment> attachments);
	SNOWFALLENGINE_API ~RenderTargetAsset();
	virtual std::string GetPath() const override 
	{
		return m_path;
	}

	inline TextureAsset *GetTexture(int index)
	{
		return m_textures[index];
	}

	SNOWFALLENGINE_API Framebuffer GetFramebuffer();

	SNOWFALLENGINE_API virtual void Load() override;
	SNOWFALLENGINE_API virtual void Unload() override;
	SNOWFALLENGINE_API virtual bool IsReady() override;
	SNOWFALLENGINE_API virtual bool IsValid() override;
private:
	bool m_loaded;

	Framebuffer m_fbo;
	std::vector<TextureAsset *> m_textures;
	std::vector<TextureLayerAttachment> m_attachments;

	IAssetStreamIO *m_stream;
	std::string m_path;
};

class SNOWFALLENGINE_API RenderTargetAssetReader : public IAssetReader
{
public:
	virtual std::vector<std::string> GetExtensions() override;
	virtual void LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager) override;
};