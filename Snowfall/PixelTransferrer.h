#pragma once
#include "Buffer.h"
#include "LinearAllocator.h"
#include "JobManager.h"

class PixelTransferOperation
{

};

class PixelTransferrer
{
public:
	PixelTransferrer(int uploadBytes, int downloadBytes);
	PixelTransferOperation AsyncUpload();
	PixelTransferOperation AsyncDownload();
	void Destroy();
private:
	Buffer<char> m_downloadBuffer;
	Buffer<char> m_uploadBuffer;
	LinearAllocator m_downloadAllocator;
	LinearAllocator m_uploadAllocator;
};
