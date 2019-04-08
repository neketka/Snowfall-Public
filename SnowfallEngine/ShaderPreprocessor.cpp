#include "ShaderPreprocessor.h"
#include "ShaderAsset.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

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
			else if (ch == '\n' || ch == '\r')
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
					working.Arguments = "";
					mode = ParseMode::None;
				}
			}
			else
				working.DirectiveName += ch;
			break;
		case ParseMode::ParsingArgument:
			if (ch == '\n' || ch == '\r')
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
	std::vector<Directive> directives = FindAndRemoveDirectives(src, std::vector<std::string>({ "include", "passes", "downscale", "downsample" }));

	std::vector<std::string> variants;
	std::vector<std::string> passes;
	std::vector<DownsamplePass> downsamplepasses;
	std::vector<int> downscalepasses;

	int includeAdjust = 0;
	for (Directive dir : directives)
	{
		if (dir.DirectiveName == "include")
		{
			ShaderAsset& asset = m_assetManager.LocateAsset<ShaderAsset>(dir.Arguments.substr(1, dir.Arguments.length() - 2));
			asset.Load();

			if (asset.IsValid())
			{
				src = src.insert(dir.Position + includeAdjust, asset.GetSource() + "\n#line 0\n");
				includeAdjust += asset.GetSource().length() + 9;
			}
			else
				src = src.insert(dir.Position, "#error Cannot open file \"" + dir.Arguments + "\"");
		}

		else if (dir.DirectiveName == "variants")
		{
			std::istringstream iss(dir.Arguments);
			variants.insert(variants.end(), std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>());
		}

		else if (dir.DirectiveName == "passes")
		{
			std::istringstream iss(dir.Arguments);
			passes.insert(passes.end(), std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>());
		}

		else if (dir.DirectiveName == "downsample")
		{
			std::vector<std::string> tokens;
			boost::split(tokens, dir.Arguments, boost::is_any_of(" "));
			if (tokens.size() != 2)
				src.insert(dir.Position, "#error downsample directive syntax error");
			else
			{
				DownsamplePass pass;
				pass.Pass = std::stoi(tokens[0]);
				pass.Level = std::stoi(tokens[1]);
				downsamplepasses.push_back(pass);
			}
		}

		else if (dir.DirectiveName == "downscale")
		{
			downscalepasses.push_back(std::stoi(dir.Arguments));
		}
	}
	return PreprocessedShader(src, variants, passes, downsamplepasses, downscalepasses);
}
