#pragma once
#include "AssetManager.h"
#include "export.h"

class UserAsset : public IAsset
{
public:
	SNOWFALLENGINE_API UserAsset(IAssetStreamIO *stream);
	SNOWFALLENGINE_API UserAsset(std::string path, IAssetStreamIO *stream);
	SNOWFALLENGINE_API ~UserAsset();

	SNOWFALLENGINE_API virtual std::string GetPath() const override;
	SNOWFALLENGINE_API virtual void SetStream(IAssetStreamIO *stream) override;
	SNOWFALLENGINE_API virtual void Load() override;
	SNOWFALLENGINE_API virtual void Unload() override;
	SNOWFALLENGINE_API virtual bool IsReady() override;
	SNOWFALLENGINE_API virtual bool IsValid() override;

	SNOWFALLENGINE_API void SetData(char *data, int offset, int length);
	SNOWFALLENGINE_API void SetDataSize(int bytes);
	SNOWFALLENGINE_API char *GetData();

	SNOWFALLENGINE_API virtual IAsset *CreateCopy(std::string newPath, IAssetStreamIO *output) override;
	SNOWFALLENGINE_API virtual void Export() override;
private:
	char *m_data;
	bool m_loaded;
	std::string m_path;
	IAssetStreamIO *m_stream;
};

