#include "PixelTransferrer.h"

PixelTransferrer::PixelTransferrer(int uploadBytes, int downloadBytes)
	: m_uploadBuffer(uploadBytes, BufferOptions(false, true, true, true, true, true)),
	m_downloadBuffer(downloadBytes, BufferOptions(true, false, true, true, true, true)),
	m_uploadAllocator(uploadBytes),
	m_downloadAllocator(downloadBytes)
{
	m_uploadBuffer.MapBuffer(0, uploadBytes, MappingOptions(false, true, true, true, false, false, false, false));
	m_downloadBuffer.MapBuffer(0, downloadBytes, MappingOptions(true, false, true, true, false, false, false, false));
}

void PixelTransferrer::Destroy()
{
}
