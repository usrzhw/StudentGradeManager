#include "module_sdk.h"
#include "module_sdk.h"

ModuleSDK::ModuleInfo::ModuleInfo(const std::string& name, const std::string& version, const std::string& description)
	:module_name(name), module_version(version), module_description(description)
{
}

void ModuleSDK::ModuleInfo::Add(const std::string& method_name, const ModuleAction_t& func)
{
	if (method_name == "") return;
	this->methods[method_name] = func;
}

auto ModuleSDK::ModuleInfo::Get(const std::string& method_name) const -> std::function<void(const RbsLib::Network::HTTP::Request& request)>
{
	if (this->methods.find(method_name) == this->methods.end())
		return nullptr;
	return this->methods.find(method_name)->second;
}

void ModuleSDK::ModuleInfo::AddCommand(const std::string& command_name, const CommandAction_t& func)
{
	if (command_name == "") return;
	this->commands[command_name] = func;
}
