#include "TextureAsset.h"

TextureAsset::TextureAsset(IAssetStreamIO *stream) : m_stream(stream)
{
	stream->OpenStreamRead();

	m_path = m_stream->ReadString();

	TextureHeader header;
	stream->ReadStream(&header, 1);

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
		m_internalFormat = TextureInternalFormat::R8;
		break;
	case TexturePixelFormat::RG:
		m_internalFormat = TextureInternalFormat::RG8;
		break;
	case TexturePixelFormat::RGB:
		m_internalFormat = TextureInternalFormat::RGB8;
	case TexturePixelFormat::BGR:
		m_internalFormat = TextureInternalFormat::RGB8;
		break;
	case TexturePixelFormat::BGRA:
		m_internalFormat = TextureInternalFormat::RGBA8;
	case TexturePixelFormat::RGBA:
		m_internalFormat = TextureInternalFormat::RGBA8;
		break;
	case TexturePixelFormat::Depth:
		m_internalFormat = TextureInternalFormat::Depth32F;
		break;
	case TexturePixelFormat::Stencil:
		m_internalFormat = TextureInternalFormat::Stencil8UI;
		break;
	}
}

TextureAsset::TextureAsset(std::string path, TextureType type, TextureInternalFormat format, int width, int height, int depth, int levels)
{
	m_type = type;
	m_baseWidth = width;
	m_baseHeight = height;
	m_baseDepth = depth;
	m_mipmaps = levels;
	m_internalFormat = format;
	m_inMemory = true;
	m_path = path;
}

TextureAsset::~TextureAsset()
{
	Unload();
}

std::string TextureAsset::GetPath() const
{
	return m_path;
}

void TextureAsset::Load()
{
	if (!m_validTextureObject)
		initializeTexture();
	if (m_inMemory)
		return;
	if (m_minMipmapLoaded > m_minMipmapSetting)
	{
		m_stream->OpenStreamRead();
		m_stream->SeekStream(m_topPos);

		for (; m_minMipmapLoaded > m_minMipmapSetting; --m_minMipmapLoaded)
		{
			int level = 0;
			int dataLength = 0;
			m_stream->ReadStream(&level, 1);
			m_stream->ReadStream(&dataLength, 1);

			char *data = new char[dataLength];
			m_stream->ReadStream(data, dataLength);

			float power = 1 / std::pow(2, level);
			int stride;

			switch (m_type)
			{
			case TextureType::Texture2D:
				m_texture.SetData(0, 0, static_cast<int>(std::floor(m_baseWidth * power)), static_cast<int>(std::floor(m_baseHeight * power)),
					level, m_pixelFormat, m_pixelType, data);
				break;
			case TextureType::Texture2DArray:
				break;
			case TextureType::Texture3D:
				break;
			case TextureType::TextureCubemap:
				stride = static_cast<int>(std::floor(m_baseWidth * power)) * static_cast<int>(std::floor(m_baseHeight * power)) * GetByteDepth(m_pixelFormat, m_pixelType);
				for (int i = 0; i < 6; ++i)
				{
					m_texture.SetData(0, 0, i, static_cast<int>(std::floor(m_baseWidth * power)), static_cast<int>(std::floor(m_baseHeight * power)), 1,
						level, m_pixelFormat, m_pixelType, data + (i * stride));
				}
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

void TextureAsset::ResizeDepth(int newDepth)
{
	Texture old = m_texture;
	int oldDepth = m_baseDepth;

	m_baseDepth = newDepth;
	initializeTexture();

	for (int i = 0; i < m_mipmaps; ++i)
		old.CopyPixels(m_texture, 0, 0, 0, i, 0, 0, 0, i, m_baseWidth, m_baseHeight, std::min<int>({ oldDepth, m_baseDepth }));
	old.Destroy();
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
	case TextureType::Texture2D:
		m_texture = Texture(m_baseWidth, m_baseHeight, false, m_mipmaps, m_internalFormat);
		break;
	case TextureType::Texture2DArray:
		m_texture = Texture(m_baseWidth, m_baseHeight, m_baseDepth, false, m_mipmaps, m_internalFormat);
		break;
	case TextureType::Texture3D:
		m_texture = Texture(m_baseWidth, m_baseHeight, m_baseDepth, m_mipmaps, m_internalFormat);
		break;
	case TextureType::TextureCubemap:
		m_texture = Texture(m_baseWidth, m_baseHeight, true, m_mipmaps, m_internalFormat);
		break;
	case TextureType::TextureCubemapArray:
		m_texture = Texture(m_baseWidth, m_baseHeight, m_baseDepth, true, m_mipmaps, m_internalFormat);
		break;
	}
	m_validTextureObject = true;
}

Texture TextureAsset::GetTextureObject()
{
	if (!m_validTextureObject)
		Load();
	return m_texture;
}

IAsset *TextureAsset::CreateCopy(std::string newPath, IAssetStreamIO *output)
{
	return nullptr;
}

void TextureAsset::Export()
{
}

std::vector<std::string> TextureAssetReader::GetExtensions()
{
	return { ".tasset" };
}

void TextureAssetReader::LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager)
{
	assetManager.AddAsset(new TextureAsset(stream));
}
