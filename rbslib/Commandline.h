#pragma once

#include <string>
#include <vector>

namespace RbsLib::Command
{
	class CommandLine
	{
	private:
		std::vector<std::string> args;
	public:
		bool Parse(const std::string& command);
		CommandLine() = default;
		CommandLine(int argc, const char** argv);
		bool IsFlagExist(const std::string& flag);
		std::string GetNextToken(const std::string& flag) const;
		std::string operator[](int index) const;
		std::string operator[](const std::string& flag) const;
		int GetSize() const;
		void Clear();
		void Push(const std::string& arg);
	};
}
