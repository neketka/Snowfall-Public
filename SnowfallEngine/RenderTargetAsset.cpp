#include "stdafx.h"

#include "RenderTargetAsset.h"

RenderTargetAsset::RenderTargetAsset(std::string path, std::vector<TextureAsset *> textures, std::vector<TextureAsset *> newTextures, std::vector<TextureLayerAttachment> attachments, bool deleteTex)
	: m_textures(textures), m_newTextures(newTextures), m_attachments(attachments)
{
	m_textures.insert(m_textures.end(), m_newTextures.begin(), m_newTextures.end());
}

RenderTargetAsset::RenderTargetAsset(std::string path, IAssetStreamIO *stream)
{
	stream->OpenStreamRead();

	stream->ReadString();
	int counts[2];

	std::vector<TextureAsset *> assets(counts[0]);
	m_attachments.resize(counts[1]);

	stream->ReadStream(counts, 2);
	for (int i = 0; i < counts[0]; ++i)
	{
		int data[6];
		std::string assetName = "";
		stream->ReadStream(data, 6);
		if (data[0] != 0)
			assetName = stream->ReadString();
		TextureAsset *asset;
		if (data[0] == 1)
			asset = &AssetManager::LocateAssetGlobal<TextureAsset>(assetName); // Better hope that TextureAsset loads first
		else
		{
			asset = new TextureAsset(assetName, static_cast<TextureType>(data[1]), static_cast<TextureInternalFormat>(data[2]), data[3], data[4], data[5], 1);
			m_newTextures.push_back(asset);
		}
		if (data[0] == 2)
			Snowfall::GetGameInstance().GetAssetManager().AddAsset(asset);
		m_textures.push_back(asset);
	}
	stream->ReadStream(m_attachments.data(), counts[1]);
}

RenderTargetAsset::~RenderTargetAsset()
{
	Unload();
	if (m_stream)
		delete m_stream;
	for (TextureAsset *asset : m_newTextures)
		delete asset;
	//Some TextureAssets will leak!
}

Framebuffer RenderTargetAsset::GetFramebuffer()
{
	Load();
	return m_fbo;
}

void RenderTargetAsset::Load()
{
	if (!m_loaded)
	{
		std::vector<WrappedTextureView> views;
		WrappedTextureView depthView, stencilView, depthStencilView;
		bool depth = false, stencil = false, depthStencil = false;
		for (TextureLayerAttachment attachment : m_attachments)
		{
			TextureAsset *asset = m_textures[attachment.TextureIndex];
			WrappedTextureView view = asset->GetTextureObject().CreateWrappedView(attachment.Layer, attachment.Level);
			switch (view.GetFormat())
			{
			case TextureInternalFormat::Depth16I:
			case TextureInternalFormat::Depth24I:
			case TextureInternalFormat::Depth32I:
			case TextureInternalFormat::Depth32F:
				depth = true;
				depthView = view;
				break;
			case TextureInternalFormat::Depth24IStencil8UI:
			case TextureInternalFormat::Depth32FStencil8UI:
				depthStencil = true;
				depthStencilView = view;
				break;
			case TextureInternalFormat::Stencil8UI:
				stencil = true;
				stencilView = view;
				break;
			default:
				views.push_back(view);
				break;
			}
		}
		if (depthStencil)
			m_fbo = Framebuffer(views, depthStencilView);
		else
			m_fbo = Framebuffer(views, depthView, stencilView, depth, stencil);
		m_loaded = true;
	}
}

void RenderTargetAsset::Unload()
{
	if (m_loaded)
	{
		for (TextureAsset *asset : m_textures)
			asset->Unload();
		m_fbo.Destroy();
		m_loaded = false;
	}
}

bool RenderTargetAsset::IsReady()
{
	return m_loaded;
}

bool RenderTargetAsset::IsValid()
{
	return true;
}

void RenderTargetAsset::BuildPipeline(Pipeline& pipeline)
{
	pipeline.FragmentStage.Framebuffer = GetFramebuffer();
}

IAsset *RenderTargetAsset::CreateCopy(std::string newPath)
{
	return nullptr;
}

void RenderTargetAsset::Export()
{
}

void RenderTargetAsset::SetStream(IAssetStreamIO *stream)
{
	m_stream = stream;
}

std::vector<std::string> RenderTargetAssetReader::GetExtensions()
{
	return { ".rtasset" };
}

void RenderTargetAssetReader::LoadAssets(std::string ext, IAssetStreamIO *stream, AssetManager& assetManager)
{
	stream->OpenStreamRead();

	std::string path = stream->ReadString();

	stream->CloseStream();

	assetManager.AddAsset(new RenderTargetAsset(path, stream));

}
