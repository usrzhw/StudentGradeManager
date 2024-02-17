#include "String.h"
#include <iostream>
#include <cstring>
#include <vector>
using namespace std;


vector<std::string> split(const std::string& str, const std::string& p)
{
	std::string a[1000];
	int k = 0;
	int count = 0;
	for (int i = 0; i < str.size(); i++)
	{
		int flag = 0;
		for (int j = 0; j < p.size(); j++)
		{
			if (str[i] == p[j])
			{
				flag++;
			}
		}
		if (flag == 0)
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