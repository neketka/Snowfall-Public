#pragma once
#include "AssetManager.h"

class NullStreamSource : public IAssetStreamIO
{
public:
	NullStreamSource() {}

	virtual void OpenStreamRead() override {}
	virtual void OpenStreamWrite() override {}
	virtual void CloseStream() override {}
	virtual void WriteStreamBytes(char * buffer, int length) override {}
	virtual void ReadStreamBytes(char * buffer, int length) override {}
	virtual bool CanRead() override { return false; }
	virtual bool CanWrite() override { return false; }
	virtual void SeekStream(int position) override {}
	virtual int GetStreamPosition() override { return -1; }
	virtual int GetStreamLength() override { return 0; }
};

