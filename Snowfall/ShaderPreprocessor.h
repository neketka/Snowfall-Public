#pragma once
#include <string>
#include <vector>
#include "AssetManager.h"

class PreprocessedShader
{
public:
	PreprocessedShader(std::string src, std::vector<std::string> variants) 
		: m_variants(variants), m_src(src) {}

	inline std::string GetProcessedSource() { return m_src; }
	inline std::vector<std::string> GetVariantQualifiers() { return m_variants; }
private:
	std::string m_src;
	std::vector<std::string> m_variants;
};

class ShaderPreprocessor
{
public:
	ShaderPreprocessor(AssetManager& assetManager);
	PreprocessedShader PreprocessShader(std::string src);
private:
	AssetManager& m_assetManager;
};

