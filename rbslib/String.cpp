#include "String.h"
#include <iostream>
#include <cstring>
#include <vector>
using namespace std;


std::vector<string> split(const std::string& str, const std::string& pattern)
{
	char* strc = new char[strlen(str.c_str()) + 1];
	strcpy(strc, str.c_str());
	vector<string> res;
	char* temp = strtok(strc, pattern.c_str());
	while (temp != NULL)
	{
		res.push_back(string(temp));
		temp = strtok(NULL, pattern.c_str());
	}
	delete[] strc;
	return res;

}