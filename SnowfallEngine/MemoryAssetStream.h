#pragma once
#include <vector>
#include "AssetManager.h"

class MemoryAssetStream : public IAssetStreamIO
{
public:
	SNOWFALLENGINE_API MemoryAssetStream(std::vector<char>& memory);
	SNOWFALLENGINE_API virtual void OpenStreamRead() override;
	SNOWFALLENGINE_API virtual void OpenStreamWrite(bool overwrite) override;
	SNOWFALLENGINE_API virtual void CloseStream() override;
	SNOWFALLENGINE_API virtual void WriteStreamBytes(char *buffer, int length) override;
	SNOWFALLENGINE_API virtual void ReadStreamBytes(char *buffer, int length) override;
	SNOWFALLENGINE_API virtual bool CanRead() override;
	SNOWFALLENGINE_API virtual bool CanWrite() override;
	SNOWFALLENGINE_API virtual void SeekStream(int position) override;
	SNOWFALLENGINE_API virtual int GetStreamPosition() override;
	SNOWFALLENGINE_API virtual int GetStreamLength() override;
private:
	int m_position; 
	char *m_fixedMem;
	std::vector<char> *m_mem;
};

