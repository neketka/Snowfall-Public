#pragma once
#include "AssetManager.h"

class NullStreamSource : public IAssetStreamSource
{
public:
	NullStreamSource() {}

	virtual void OpenStream() override {}
	virtual void CloseStream() override {}
	virtual void ReadStream(char * buffer, int length) override {}
	virtual void SeekStream(int position) override {}
	virtual int GetStreamPosition() override { return -1; }
	virtual int GetStreamLength() override { return 0; }
};

