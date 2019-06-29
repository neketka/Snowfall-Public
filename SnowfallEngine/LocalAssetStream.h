#pragma once
#include "AssetManager.h"
#include <fstream>

#include "export.h"

class LocalAssetStream : public IAssetStreamIO
{
public:
	SNOWFALLENGINE_API LocalAssetStream(std::string path);
	SNOWFALLENGINE_API virtual ~LocalAssetStream() override {}
	SNOWFALLENGINE_API virtual void OpenStreamRead() override;
	SNOWFALLENGINE_API virtual void OpenStreamWrite() override;

	SNOWFALLENGINE_API virtual void CloseStream() override;

	SNOWFALLENGINE_API virtual void ReadStreamBytes(char* buffer, int length) override;
	SNOWFALLENGINE_API virtual void WriteStreamBytes(char * buffer, int length) override;

	SNOWFALLENGINE_API virtual std::string GetFilePath() { return m_path; }

	SNOWFALLENGINE_API virtual bool CanRead() override;
	SNOWFALLENGINE_API virtual bool CanWrite() override;

	SNOWFALLENGINE_API virtual void SeekStream(int position) override;
	SNOWFALLENGINE_API virtual int GetStreamPosition() override;
	SNOWFALLENGINE_API virtual int GetStreamLength() override;
private:
	std::string m_path;
	std::ifstream m_istream;
	std::ofstream m_ostream;
	int m_length;
	bool reading;
	bool writing;
};

