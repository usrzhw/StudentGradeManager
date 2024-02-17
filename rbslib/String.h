#pragma once
#include "BaseType.h"
#include <string>
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