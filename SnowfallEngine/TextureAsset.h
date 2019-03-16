#pragma once
#include "AssetManager.h"
#include "Texture.h"

#include "export.h"

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
	SNOWFALLENGINE_API TextureAsset(IAssetStreamIO *stream);
	SNOWFALLENGINE_API TextureAsset(std::string path, TextureType type, TextureInternalFormat format, int width, int height, int depth, int levels);
	SNOWFALLENGINE_API ~TextureAsset();

	SNOWFALLENGINE_API virtual std::string GetPath() const override;
	SNOWFALLENGINE_API virtual void Load() override;
	SNOWFALLENGINE_API virtual void Unload() override;
	SNOWFALLENGINE_API virtual bool IsReady() override;
	SNOWFALLENGINE_API virtual bool IsValid() override;

	inline TextureInternalFormat GetFormat() { return m_internalFormat; }
	inline TextureType GetType() { return m_type; }

	SNOWFALLENGINE_API void ResizeDepth(int newDepth);

	SNOWFALLENGINE_API void SetLOD(float lod);

	SNOWFALLENGINE_API void initializeTexture();

	SNOWFALLENGINE_API Texture GetTextureObject();

	virtual IAsset *CreateCopy(std::string newPath, IAssetStreamIO *output) override;
	virtual void Export() override;
private:
	Texture m_texture;

	bool m_validTextureObject;
	bool m_loadedAnyMipmap;
	bool m_loadingBackground;
	bool m_inMemory;

	IAssetStreamIO *m_stream;
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
	SNOWFALLENGINE_API virtual std::vector<std::string> GetExtensions() override;
	SNOWFALLENGINE_API virtual void LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager) override;
};