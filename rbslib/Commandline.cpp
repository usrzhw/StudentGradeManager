#include "Commandline.h"
#include <exception>

using namespace RbsLib::Command;

bool CommandLine::Parse(const std::string& command)
{
	this->Clear();
	std::string arg;
	bool is_in = false;//标识是否在有效解析部分中
	bool q = false;//标识是否在双引号引起的部分中

	for (auto it : command)
	{
		if (q == true)
		{
			//当前在引号中
			if (it == '\"')
			{
				//离开引号区域
				q = false;
			}
			else arg.push_back(it);
		}
		else
		{
			//当前不在引号中
			if (it == ' ')
			{
				if (is_in == true)
				{
					//终止有效部分解析
					is_in = false;
					if (arg != "") args.push_back(arg);
					arg.clear();
				}
			}
			else
			{
				//进入有效解析区域
				is_in = true;
				if (it == '\"') q = true;//进入引号区域
				else arg.push_back(it);
			}
		}
	}
	if (!arg.empty()) args.push_back(arg);
	return true;
}

CommandLine::CommandLine(int argc, const char** argv)
{
	if (argc>0)
	{
		for (int i = 0; i < argc; ++i)
		{
			this->args.push_back(argv[i]);
		}
	}
}

bool CommandLine::IsFlagExist(const std::string& flag)
{
	if (std::find(this->args.begin(), this->args.end(), flag) != this->args.end())
	{
		return true;
	}
	return false;
}

std::string CommandLine::GetNextToken(const std::string& flag) const
{
	auto it = std::find(this->args.begin(), this->args.end(), flag);
	if (it != this->args.end()&&(++it)!=args.end())
	{
		return *it;
	}
	return std::string();
}

std::string CommandLine::operator[](int index) const
{
	return this->args.size() > index ? this->args[index] : std::string();
}

std::string CommandLine::operator[](const std::string& flag) const
{
	return this->GetNextToken(flag);
}

int CommandLine::GetSize() const
{
	return this->args.size();
}

void CommandLine::Clear()
{
	this->args.clear();
}

void CommandLine::Push(const std::string& arg)
{
	this->args.push_back(arg);
}
