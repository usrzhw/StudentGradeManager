#include "String.h"
#include <iostream>
using namespace space;

int compare(const char& str, const std::string& p)
{
	int flag = 0;
	for (int i = 0; i < p.size(); i++)
	{
		if (!strcmp(str, p[i])) flag++;
	}
	if (flag == 0) return 0;
	else return 1;
}

vector<std::string> split(const std::string& str, const std::string& p)
{
	std::string a[1000];
	int k = 0;
	int count = 0;
	for (int i = 0; i < str.size(); i++)
	{
		int shu = compare(str[i], p);
		if (shu == 0)
		{
			a[k][count++] = str[i];
		}
		else
		{
			count = 0;
			k++;
		}
	}
}