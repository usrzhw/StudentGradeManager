#pragma once
#include <string>
#include <functional>
#include <map>
#include "../rbslib/Network.h"
namespace ModuleSDK
{
	typedef std::function<void(const RbsLib::Network::HTTP::Request& request)> ModuleAction_t;
	typedef std::function<void (int,const char**)> CommandAction_t;
	struct ModuleInfo
	{
		std::string module_name;
		std::string module_version;
		std::string module_description;
		std::map<std::string, ModuleAction_t> methods;
		std::map<std::string, CommandAction_t> commands;
		ModuleInfo() = default;
		ModuleInfo(const std::string& name, const std::string& version, const std::string& description);
		void Add(const std::string& method_name, const ModuleAction_t& func);
		auto Get(const std::string& method_name)const-> ModuleAction_t;
		void AddCommand(const std::string& command_name, const CommandAction_t& func);
	};
	//模块应该至少包含Init函数用于初始化
}
extern "C" ModuleSDK::ModuleInfo Init(void);