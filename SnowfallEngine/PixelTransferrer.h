#pragma once
#include "Buffer.h"
#include "LinearAllocator.h"
#include "JobManager.h"

#include "export.h"

class PixelTransferOperation
{

};

class PixelTransferrer
{
public:
	SNOWFALLENGINE_API PixelTransferrer(int uploadBytes, int downloadBytes);
	SNOWFALLENGINE_API PixelTransferOperation AsyncUpload();
	SNOWFALLENGINE_API PixelTransferOperation AsyncDownload();
	SNOWFALLENGINE_API void Destroy();
private:
	Buffer<char> m_downloadBuffer;
	Buffer<char> m_uploadBuffer;
	LinearAllocator m_downloadAllocator;
	LinearAllocator m_uploadAllocator;
};
