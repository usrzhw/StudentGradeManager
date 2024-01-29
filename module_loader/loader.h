#pragma once
#include <mutex>
#include <memory>
#include <exception>
#include <string>
#include <map>
#include "../rbslib/Storage.h"
#include "../module_sdk/module_sdk.h"
namespace Module
{
	class ModuleLoaderException : public std::exception
	{
	private:
		std::string reason;
	public:
		ModuleLoaderException(const std::string& reason) noexcept;
		const char* what(void)const noexcept override;
	};
	class ModuleObject
	{
	private:
		std::shared_ptr<void*> dl;
	public:
		ModuleSDK::ModuleInfo module_info;
		ModuleObject(const RbsLib::Storage::StorageFile& path);
	};
	class ModuleManager
	{
	private:
		std::map<std::string, ModuleObject> modules;
	public:
		void LoadModules(const RbsLib::Storage::StorageFile& dir);
		void UnloadModule(const std::string& module_name);
		void UnloadAllModules(void);
		ModuleSDK::ModuleAction_t GetAction(const std::string& module_name, const std::string& action_name);
	};
}