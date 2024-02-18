#pragma once
#include "BaseType.h"
#include <string>
#include <iostream>
#include <vector>
std::vector<std::string> split(const std::string & str, const std::string & pattern);
namespace RbsLib::String
{
	namespace Convert
	{
		template <typename T>
		auto StringToNumber(const std::string& str)->T
		{
			T a = 0;
			for (auto it : str)
			{
				a = a * 10 + it - '0';
			}
			return a;
		}
	}
}