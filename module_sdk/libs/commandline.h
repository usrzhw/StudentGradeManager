#pragma once

#include <string>
#include <vector>

namespace CommandLine
{
	static std::string Execute(const std::vector<std::string>& args);
	static const std::vector<std::string> Parse(const std::string& command);
}