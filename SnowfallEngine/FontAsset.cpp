#include "stdafx.h"

#include "FontAsset.h"

FontAsset::FontAsset(std::string path, IAssetStreamIO *stream) : m_stream(stream), m_path(path)
{
}

FontAsset::~FontAsset()
{
	Unload();
	delete m_stream;
}

std::string FontAsset::GetPath() const
{
	return m_path;
}

void FontAsset::Load()
{
	if (!m_loaded)
	{
		int dim[2];
		int lens[2];

		m_stream->OpenStreamRead();
		m_stream->SeekStream(m_path.length() + 4);
		m_name = m_stream->ReadString();
		m_stream->ReadStream(dim, 2);
		m_stream->ReadStream(&m_type, 1);
		m_stream->ReadStream(lens, 2);

		std::vector<char> imgData(lens[0]);
		m_stream->ReadStream(imgData.data(), lens[0]);

		m_texture = Texture(dim[0], dim[1], false, 1, m_type == FontImageType::R8Bitmap || m_type == FontImageType::R8DistanceField ?
			TextureInternalFormat::R8 : m_type == FontImageType::RGB8DistanceField ? TextureInternalFormat::RGB8 :
			TextureInternalFormat::RGBA8);

		m_texture.SetData(0, 0, dim[0], dim[1], 0, m_type == FontImageType::R8Bitmap || m_type == FontImageType::R8DistanceField ?
			TexturePixelFormat::R : m_type == FontImageType::RGB8DistanceField ? TexturePixelFormat::BGR :
			TexturePixelFormat::BGRA, TextureDataType::UnsignedByte, imgData.data());
		m_glyphs = std::vector<GlyphDescription>(256);

		for (int i = 0; i < lens[1]; ++i)
		{
			GlyphDescription desc;
			m_stream->ReadStream(&desc, 1);
			m_glyphs[desc.Character] = desc;
		}

		m_stream->CloseStream();

		m_loaded = true;
	}
}

void FontAsset::Unload()
{
	if (m_loaded)
	{
		m_glyphs.clear();
		m_texture.Destroy();
		m_loaded = false;
	}
}

bool FontAsset::IsReady()
{
	return m_loaded;
}

bool FontAsset::IsValid()
{
	return true;
}

Texture FontAsset::GetAtlas()
{
	Load();
	return m_texture;
}

GlyphDescription FontAsset::GetGlyph(char character)
{
	Load();
	return m_glyphs[character];
}

FontImageType FontAsset::GetImageType()
{
	Load();
	return m_type;
}

std::string FontAsset::GetName()
{
	Load();
	return m_name;
}

IAsset *FontAsset::CreateCopy(std::string newPath)
{
	return nullptr;
}

void FontAsset::Export()
{
}

void FontAsset::SetStream(IAssetStreamIO *stream)
{
}

std::vector<std::string> FontAssetReader::GetExtensions()
{
	return { ".fntasset" };
}

void FontAssetReader::LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager)
{
	stream->OpenStreamRead();
	std::string path = stream->ReadString();
	stream->CloseStream();

	assetManager.AddAsset(new FontAsset(path, stream));
}
