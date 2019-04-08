#pragma once
#include <string>
#include <vector>
#include "AssetManager.h"

#include "export.h"

class DownsamplePass
{
public:
	int Pass;
	int Level;
};

class PreprocessedShader
{
public:
	PreprocessedShader() {}
	PreprocessedShader(std::string src, std::vector<std::string> variants, std::vector<std::string> passes, 
		std::vector<DownsamplePass> downsamplePasses, std::vector<int> downscalePasses)
		: m_variants(variants), m_src(src), m_passes(passes), m_downsamplePasses(downsamplePasses), m_downscalePasses(downscalePasses) {}

	inline std::string GetProcessedSource() { return m_src; }
	inline std::vector<std::string> GetVariantQualifiers() { return m_variants; }
	inline std::vector<std::string> GetPasses() { return m_passes; }
	inline std::vector<DownsamplePass> GetDownsamplePasses() { return m_downsamplePasses; }
	inline std::vector<int> GetDownscalePasses() { return m_downscalePasses; }
private:
	std::vector<DownsamplePass> m_downsamplePasses;
	std::vector<int> m_downscalePasses;
	std::vector<std::string> m_passes;
	std::vector<std::string> m_variants;
	std::string m_src;
};

class SNOWFALLENGINE_API ShaderPreprocessor
{
public:
	ShaderPreprocessor(AssetManager& assetManager);
	PreprocessedShader PreprocessShader(std::string src);
private:
	AssetManager& m_assetManager;
};

