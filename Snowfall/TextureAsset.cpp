#include "TextureAsset.h"

TextureAsset::TextureAsset(IAssetStreamSource *stream) : m_stream(stream)
{
	stream->OpenStream();

	unsigned int size = 0;
	stream->ReadStream(reinterpret_cast<char *>(&size), sizeof(unsigned int));
	char *buffer = new char[size + 1];
	buffer[size] = '\0';
	stream->ReadStream(buffer, size);

	m_path = std::string(buffer);
	delete[] buffer;

	TextureHeader header;
	stream->ReadStream(reinterpret_cast<char *>(&header), sizeof(TextureHeader));

	m_type = header.TextureType;
	m_pixelFormat = header.PixelFormat;
	m_pixelType = header.IsFloat ? TextureDataType::Float : TextureDataType::UnsignedByte;
	m_baseWidth = header.BaseWidth;
	m_baseHeight = header.BaseHeight;
	m_baseDepth = header.BaseDepth;
	m_frameCount = header.FrameCount;
	m_fps = header.FramesPerSecond;
	m_mipmaps = header.MipmapCount;

	m_minMipmapLoaded = m_mipmaps;
	m_firstPos = stream->GetStreamPosition();
	m_topPos = m_firstPos;

	stream->CloseStream();

	switch (m_pixelFormat)
	{
	case TexturePixelFormat::R:
		break;
	case TexturePixelFormat::RG:
		break;
	case TexturePixelFormat::RGB:
	case TexturePixelFormat::BGR:
		m_internalFormat = TextureInternalFormat::RGB8;
		break;
	case TexturePixelFormat::BGRA:
	case TexturePixelFormat::RGBA:
		m_internalFormat = TextureInternalFormat::RGBA8;
		break;
	case TexturePixelFormat::Depth:
		break;
	case TexturePixelFormat::Stencil:
		break;
	}
}

TextureAsset::~TextureAsset()
{
}

std::string TextureAsset::GetPath() const
{
	return m_path;
}

void TextureAsset::Load()
{
	if (!m_validTextureObject)
		initializeTexture();
	if (m_minMipmapLoaded > m_minMipmapSetting)
	{
		m_stream->OpenStream();
		m_stream->SeekStream(m_topPos);

		for (; m_minMipmapLoaded > m_minMipmapSetting; --m_minMipmapLoaded)
		{
			int level = 0;
			int dataLength = 0;
			m_stream->ReadStream(&level, sizeof(int));
			m_stream->ReadStream(&dataLength, sizeof(int));

			char *data = new char[dataLength];
			m_stream->ReadStream(data, dataLength);

			float power = 1 / std::pow(2, level);

			switch (m_type)
			{
			case TextureType::Texture1D:
				break;
			case TextureType::Texture1DArray:
				break;
			case TextureType::Texture2D:
				m_texture.SetData(0, 0, static_cast<int>(std::floor(m_baseWidth * power)), static_cast<int>(std::floor(m_baseHeight * power)),
					level, m_pixelFormat, m_pixelType, data);
				break;
			case TextureType::Texture2DArray:
				break;
			case TextureType::Texture3D:
				break;
			case TextureType::TextureCubemap:
				break;
			case TextureType::TextureCubemapArray:
				break;
			}
			m_texture.SetMipmapRange(m_minMipmapLoaded, m_mipmaps);
		}

		m_stream->CloseStream();
		m_topPos = m_stream->GetStreamPosition();
	}
}

void TextureAsset::Unload()
{
	if (m_validTextureObject)
	{
		m_texture.Destroy();
		m_validTextureObject = false;
		m_loadedAnyMipmap = false;
		m_minMipmapLoaded = m_mipmaps;
		m_minMipmapSetting = m_mipmaps - 1;
		m_topPos = m_firstPos;
	}
}

bool TextureAsset::IsReady()
{
	return m_validTextureObject;
}

bool TextureAsset::IsValid()
{
	return true;
}

void TextureAsset::SetLOD(float lod)
{
	if (lod == 1)
		Unload();
	m_minMipmapSetting = lod * m_mipmaps;
	Load();
}

void TextureAsset::initializeTexture()
{
	switch (m_type)
	{
	case TextureType::Texture1D:
		break;
	case TextureType::Texture1DArray:
		break;
	case TextureType::Texture2D:
		m_texture = Texture(m_baseWidth, m_baseHeight, false, m_mipmaps, m_internalFormat);
		break;
	case TextureType::Texture2DArray:
		break;
	case TextureType::Texture3D:
		break;
	case TextureType::TextureCubemap:
		break;
	case TextureType::TextureCubemapArray:
		break;
	}
}

Texture TextureAsset::GetTextureObject()
{
	if (!m_validTextureObject)
		Load();
	return m_texture;
}

std::vector<std::string> TextureAssetReader::GetExtensions()
{
	return { ".tasset" };
}

void TextureAssetReader::LoadAssets(std::string ext, IAssetStreamSource *streamSource, AssetManager& assetManager)
{
	assetManager.AddAsset(new TextureAsset(streamSource));
}
