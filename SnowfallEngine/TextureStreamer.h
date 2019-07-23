#pragma once
#include "SubmitBuffer.h"
#include "Texture.h"

class PixelTransferOp
{
public:
	bool Downloading;
	GLsync SyncObject;
	char *Memory;
};

class TexturePackOperation
{
public:
	bool IsComplete();
	void Destroy();
};

class TextureStreamer
{
public:
private:
	SubmitBuffer<char> m_packBuffer;
	SubmitBuffer<char> m_unpackBuffer;
};