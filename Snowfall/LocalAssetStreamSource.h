#pragma once
#include "AssetManager.h"
#include <fstream>

class LocalAssetStreamSource : public IAssetStreamSource
{
public:
	LocalAssetStreamSource(std::string path);
	virtual void OpenStream() override;
	virtual void CloseStream() override;
	virtual void ReadStream(char* buffer, int length) override;
	virtual void SeekStream(int position) override;
	virtual int GetStreamPosition() override;
	virtual int GetStreamLength() override;
private:
	std::string m_path;
	std::ifstream m_stream;
	int m_length;
};

