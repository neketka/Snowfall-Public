#pragma once
#include "AssetManager.h"
#include <fstream>

class LocalAssetStream : public IAssetStreamIO
{
public:
	LocalAssetStream(std::string path);
	virtual void OpenStreamRead() override;
	virtual void OpenStreamWrite() override;

	virtual void CloseStream() override;

	virtual void ReadStreamBytes(char* buffer, int length) override;
	virtual void WriteStreamBytes(char * buffer, int length) override;

	virtual std::string GetFilePath() { return m_path; }

	virtual bool CanRead() override;
	virtual bool CanWrite() override;

	virtual void SeekStream(int position) override;
	virtual int GetStreamPosition() override;
	virtual int GetStreamLength() override;
private:
	std::string m_path;
	std::ifstream m_istream;
	std::ofstream m_ostream;
	int m_length;
	bool reading;
	bool writing;
};

