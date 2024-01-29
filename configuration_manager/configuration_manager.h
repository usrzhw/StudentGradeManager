#pragma once
#include <string>

namespace Config
{
	void SaveConfig(void);
	void LoadConfig(void);
	int Port(void);
	void Port(int port);
	std::string ModulePath(void);
	void ModulePath(const std::string& path);
}