#pragma once
#include "AssetManager.h"
#include "Texture.h"

enum class TextureType
{
	Texture1D = 0, Texture1DArray = 1, Texture2D = 2, Texture2DArray = 3, TextureCubemap = 4, TextureCubemapArray = 5, Texture3D = 6
};

class TextureHeader
{
public:
	TextureType TextureType;
	TexturePixelFormat PixelFormat;
	bool IsFloat;
	int BaseWidth;
	int BaseHeight;
	int BaseDepth;
	int FrameCount;
	float FramesPerSecond;
	int MipmapCount;
};

class TextureAsset : public IAsset
{
public:
	TextureAsset(IAssetStreamSource *stream);
	~TextureAsset();

	virtual std::string GetPath() const override;
	virtual void Load() override;
	virtual void Unload() override;
	virtual bool IsReady() override;
	virtual bool IsValid() override;

	void SetLOD(float lod);

	void initializeTexture();

	Texture GetTextureObject();

private:
	Texture m_texture;

	bool m_validTextureObject;
	bool m_loadedAnyMipmap;
	bool m_loadingBackground;

	IAssetStreamSource *m_stream;
	int m_firstPos;
	int m_topPos;

	int m_minMipmapSetting;
	int m_minMipmapLoaded;
	int m_mipmaps;
	int m_baseWidth;
	int m_baseHeight;
	int m_baseDepth;
	int m_fps;
	float m_frameCount;
	TextureType m_type;
	TextureInternalFormat m_internalFormat;
	TexturePixelFormat m_pixelFormat;
	TextureDataType m_pixelType;

	std::string m_path;
};

class TextureAssetReader : public IAssetReader
{
	virtual std::vector<std::string> GetExtensions() override;
	virtual void LoadAssets(std::string ext, IAssetStreamSource *streamSource, AssetManager& assetManager) override;
};