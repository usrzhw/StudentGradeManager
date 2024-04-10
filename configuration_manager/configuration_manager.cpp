#include "configuration_manager.h"
#include "../json/CJsonObject.h"
#include "../rbslib/FileIO.h"
#include "../rbslib/Storage.h"

static std::string CONFIG_PATH = "./Config.json";
static int PORT = 8081;
static std::string MODULE_PATH = "app";
static std::string LOG_DIR = "logs";
static int PRINT_LOG_LEVEL = 0;
static int SAVE_LOG_LEVEL = 0;
void Config::SaveConfig(void)
{
	RbsLib::Storage::StorageFile config_file(CONFIG_PATH);
	auto fp = config_file.Open(RbsLib::Storage::FileIO::OpenMode::Write |
		RbsLib::Storage::FileIO::OpenMode::Replace,RbsLib::Storage::FileIO::SeekBase::begin);
	neb::CJsonObject obj;
	obj.Add("Port", PORT);
	obj.Add("ModulePath", MODULE_PATH);
	obj.Add("LogPath", LOG_DIR);
	obj.Add("PrintLogLevel", PRINT_LOG_LEVEL);
	obj.Add("SaveLogLevel", SAVE_LOG_LEVEL);
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
}

void Config::LoadConfig(void)
{
	RbsLib::Storage::StorageFile config_file(CONFIG_PATH);
	auto fp = config_file.Open(RbsLib::Storage::FileIO::OpenMode::Read, RbsLib::Storage::FileIO::SeekBase::begin);
	neb::CJsonObject obj(fp.Read(config_file.GetFileSize()).ToString());
	if (obj.KeyExist("Port")) obj.Get("Port", PORT);
	if (obj.KeyExist("ModulePath"))obj.Get("ModulePath", MODULE_PATH);
	if (obj.KeyExist("LogPath")) obj.Get("LogPath", LOG_DIR);
	if (obj.KeyExist("PrintLogLevel")) obj.Get("PrintLogLevel", PRINT_LOG_LEVEL);
	if (obj.KeyExist("SaveLogLevel")) obj.Get("SaveLogLevel", SAVE_LOG_LEVEL);
}

int Config::Port(void)
{
	return PORT;
}

void Config::Port(int port)
{
	PORT = PORT;
}

std::string Config::ModulePath(void)
{
	return MODULE_PATH;
}

void Config::ModulePath(const std::string& path)
{
	MODULE_PATH = path;
}

std::string Config::LogDir()
{
	return LOG_DIR;
}

void Config::LogDir(const std::string& path)
{
	LOG_DIR = path;
}

int Config::PrintLogLevel()
{
	return PRINT_LOG_LEVEL;
}

void Config::PrintLogLevel(int level)
{
	PRINT_LOG_LEVEL = level;
}

int Config::SaveLogLevel()
{
	return SAVE_LOG_LEVEL;
}

void Config::SaveLogLevel(int level)
{
	SAVE_LOG_LEVEL = level;
}
