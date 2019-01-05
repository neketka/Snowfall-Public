#include "ShaderPreprocessor.h"
#include "ShaderAsset.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

class Directive
{
public:
	std::string DirectiveName;
	std::string Arguments;
	int Position;
};

enum class ParseMode
{
	None, ParsingDirective, ParsingArgument
};

std::vector<Directive> FindAndRemoveDirectives(std::string& src, std::vector<std::string> directives)
{
	src += "\n";
	std::vector<Directive> directivesList;
	ParseMode mode = ParseMode::None;
	std::string code;
	Directive working;
	int removalPosition = 0;
	bool skipNewline = false;

	for (int pos = 0; pos < src.length(); ++pos)
	{
		char ch = src[pos];
		switch (mode)
		{
		case ParseMode::None:
			if (ch == '#')
			{
				working.Position = pos - removalPosition;
				mode = ParseMode::ParsingDirective;
			}
			else
				code += ch;
			break;
		case ParseMode::ParsingDirective:
			if (ch == ' ')
			{
				if (std::find(directives.begin(), directives.end(), working.DirectiveName) != directives.end())
					mode = ParseMode::ParsingArgument;
				else
				{
					code += "#" + working.DirectiveName + " ";
					working.DirectiveName = "";
					mode = ParseMode::None;
				}
			}
			else if (ch == '\n')
			{
				if (std::find(directives.begin(), directives.end(), working.DirectiveName) != directives.end())
				{
					removalPosition += pos - working.Position;
					directivesList.push_back(working);
					code += ch;

					working.Position = -1;
					working.DirectiveName = "";
					working.Arguments = "";

					mode = ParseMode::None;
				}
				else
				{
					code += "#" + working.DirectiveName + ch;
					working.DirectiveName = "";
					mode = ParseMode::None;
				}
			}
			else
				working.DirectiveName += ch;
			break;
		case ParseMode::ParsingArgument:
			if (ch == '\n')
			{
				if (skipNewline)
					break;
				code += ch;
				removalPosition += pos - working.Position;
				directivesList.push_back(working);

				working.Position = -1;
				working.DirectiveName = "";
				working.Arguments = "";

				mode = ParseMode::None;
			}
			else if (ch == '\\')
				skipNewline = true;
			else
				working.Arguments += ch;
			break;
		}
	}
	src = code;
	return directivesList;
}

ShaderPreprocessor::ShaderPreprocessor(AssetManager& assetManager) : m_assetManager(assetManager)
{
}

PreprocessedShader ShaderPreprocessor::PreprocessShader(std::string src)
{
	std::vector<Directive> directives = FindAndRemoveDirectives(src, std::vector<std::string>({ "include" }));
	std::vector<std::string> variants;
	for (Directive dir : directives)
	{
		if (dir.DirectiveName == "include")
		{
			ShaderAsset& asset = m_assetManager.LocateAsset<ShaderAsset>(dir.Arguments.substr(1, dir.Arguments.length() - 3));
			asset.Load();

			if (asset.IsValid())
				src = src.insert(dir.Position, asset.GetSource());
			else
				src = src.insert(dir.Position, "#error Cannot open file \"" + dir.Arguments + "\"");
		}
		if (dir.DirectiveName == "variants")
		{
			std::istringstream iss(dir.Arguments);
			variants.insert(variants.end(), std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>());
		}
	}
	return PreprocessedShader(src, variants);
}
