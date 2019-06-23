#pragma once
#include "AssetManager.h"
#include "export.h"
#include "Texture.h"
#include <map>
#include <string>

class GlyphDescription
{
public:
	char Character;

	char Padding[3];

	float AtlasWidth;
	float AtlasHeight;
	float AtlasX;
	float AtlasY;
	float CharWidth;
	float CharHeight;
	float OffsetX;
	float OffsetY;
	float Advance;
};

enum class FontImageType
{
	R8Bitmap = 0, R8DistanceField = 1, RGB8DistanceField = 3, RGBA8DistanceField = 4
};

class FontAsset : public IAsset
{
public:
	SNOWFALLENGINE_API FontAsset(std::string path, IAssetStreamIO *stream);
	SNOWFALLENGINE_API ~FontAsset();

	SNOWFALLENGINE_API virtual std::string GetPath() const override;
	SNOWFALLENGINE_API virtual void SetStream(IAssetStreamIO *stream) override;
	SNOWFALLENGINE_API virtual void Load() override;
	SNOWFALLENGINE_API virtual void Unload() override;
	SNOWFALLENGINE_API virtual bool IsReady() override;
	SNOWFALLENGINE_API virtual bool IsValid() override;
	SNOWFALLENGINE_API Texture GetAtlas();
	SNOWFALLENGINE_API GlyphDescription GetGlyph(char character);
	SNOWFALLENGINE_API FontImageType GetImageType();
	SNOWFALLENGINE_API std::string GetName();
	SNOWFALLENGINE_API virtual IAsset *CreateCopy(std::string newPath) override;
	SNOWFALLENGINE_API virtual void Export() override;

	SNOWFALLENGINE_API float GetTextLength(std::string text, float size);
	SNOWFALLENGINE_API void GetClosestSeparator(std::string text, float size, float closestPixelPos, float& pixelPos, int& charPos);

private:
	std::string m_name;
	std::vector<GlyphDescription> m_glyphs;
	Texture m_texture;
	FontImageType m_type;

	IAssetStreamIO *m_stream;
	std::string m_path;
	bool m_loaded;
};

class FontAssetReader : public IAssetReader
{
public:
	FontAssetReader() {}
	virtual std::vector<std::string> GetExtensions() override;
	virtual void LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager) override;
};