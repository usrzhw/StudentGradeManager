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
	std::string LogDir();
	void LogDir(const std::string& path);
	int PrintLogLevel();
	void PrintLogLevel(int level);
	int SaveLogLevel();
	void SaveLogLevel(int level);
}