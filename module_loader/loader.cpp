#include "loader.h"
#include <dlfcn.h>
#include <cstring>

Module::ModuleObject::ModuleObject(const RbsLib::Storage::StorageFile& path)
{
	void* temp = dlopen(path.Path().c_str(), RTLD_LAZY);
	if (!temp) throw ModuleLoaderException(std::string("Open shared lib failed: ") + dlerror());
	this->dl = std::shared_ptr<void*>(new void* (temp), [](void** p) {dlclose(*p); delete p; });
	ModuleSDK::ModuleInfo(*init)(void) = (ModuleSDK::ModuleInfo(*)(void))dlsym(*(this->dl), "Init");
	if (init == nullptr)
	{
		throw ModuleLoaderException("Can not find init function in module");
	}
	this->module_info = init();
	if (this->module_info.module_name.empty())
	{
		throw ModuleLoaderException("Module not set self name");
	}
}


Module::ModuleLoaderException::ModuleLoaderException(const std::string& reason) noexcept
	:reason(reason)
{
}

const char* Module::ModuleLoaderException::what(void) const noexcept
{
	return this->reason.c_str();
}
#include <iostream>
void Module::ModuleManager::LoadModules(const RbsLib::Storage::StorageFile& dir)
{
	//检查目标是不是目录
	if (dir.GetFileType() != RbsLib::Storage::FileType::FileType::Dir)
		throw ModuleLoaderException("指定的模块路径不是目录");
	for (const auto& it : dir)
	{
		if (it.GetFileType() == RbsLib::Storage::FileType::FileType::Regular &&
			it.GetExtension() == ".so")
		{
			try
			{
				ModuleObject obj(it);
				this->modules.insert(std::pair(obj.module_info.module_name, obj));
			}
			catch (ModuleLoaderException const& ex)
			{
				std::cout << ex.what() << std::endl;
			}
		}
	}
}

void Module::ModuleManager::UnloadModule(const std::string& module_name)
{
	this->modules.erase(module_name);
}

void Module::ModuleManager::UnloadAllModules(void)
{
	this->modules.clear();
}

ModuleSDK::ModuleAction_t Module::ModuleManager::GetAction(const std::string& module_name, const std::string& action_name)
{
	if (this->modules.find(module_name) == this->modules.end() ||
		this->modules.find(module_name)->second.module_info.Get(action_name) == nullptr)
		throw ModuleLoaderException("Can not find target module or action");
	return this->modules.find(module_name)->second.module_info.Get(action_name);
}

void Module::ModuleManager::ExecuteCommand(const RbsLib::Command::CommandLine& cmd)
{
	if (cmd.GetSize()<1) 
	{
		throw ModuleLoaderException("Command line is empty");
	}
	//查找所需命令名称所在的模块
	for (auto& it : this->modules)
	{
		auto p =it.second.module_info.commands.find(cmd[0]);
		if (p != it.second.module_info.commands.end())
		{
			int argc = cmd.GetSize();
			std::unique_ptr<const char*[]> argv(new const char*[argc]);
			try
			{
				for (int i = 0; i < argc; i++)
				{
					argv[i] = new char[cmd[i].size() + 1];
					std::strcpy((char*)argv[i], cmd[i].c_str());
				}
				p->second(argc, argv.get());
			}
			catch (...)
			{
				for (int i=0;i<argc;++i) delete[] argv[i];
				throw;
			}
			for (int i = 0; i < argc; ++i) delete[] argv[i];
			return;
		}
	}
	throw ModuleLoaderException("Can not find target command");
}

const std::map<std::string, Module::ModuleObject>& Module::ModuleManager::GetModules(void) const
{
	// TODO: 在此处插入 return 语句
	return this->modules;
}
