#include "configuration_manager.h"
#include "../json/CJsonObject.h"
#include "../rbslib/FileIO.h"
#include "../rbslib/Storage.h"

static std::string CONFIG_PATH = "./Config.json";
static int PORT = 8081;
static std::string MODULE_PATH = "app";
void Config::SaveConfig(void)
{
	RbsLib::Storage::StorageFile config_file(CONFIG_PATH);
	auto fp = config_file.Open(RbsLib::Storage::FileIO::OpenMode::Write |
		RbsLib::Storage::FileIO::OpenMode::Replace,RbsLib::Storage::FileIO::SeekBase::begin);
	neb::CJsonObject obj;
	obj.Add("Port", PORT);
	obj.Add("ModulePath", MODULE_PATH);
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
}

void Config::LoadConfig(void)
{
	RbsLib::Storage::StorageFile config_file(CONFIG_PATH);
	auto fp = config_file.Open(RbsLib::Storage::FileIO::OpenMode::Read, RbsLib::Storage::FileIO::SeekBase::begin);
	neb::CJsonObject obj(fp.Read(config_file.GetFileSize()).ToString());
	if (obj.KeyExist("Port")) obj.Get("Port", PORT);
	if (obj.KeyExist("ModulePath"))obj.Get("ModulePath", MODULE_PATH);
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
