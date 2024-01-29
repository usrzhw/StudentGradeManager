#include "loader.h"
#include <dlfcn.h>

Module::ModuleObject::ModuleObject(const RbsLib::Storage::StorageFile& path)
{
	void* temp = dlopen(path.Path().c_str(), RTLD_LAZY);
	if (!temp) throw ModuleLoaderException(std::string("Open shared lib failed: ") + dlerror());
	this->dl = std::shared_ptr<void*>(new void* (temp), [](void** p) {dlclose(*p); delete p; });
	ModuleSDK::ModuleInfo(*init)(void)=(ModuleSDK::ModuleInfo (*)(void))dlsym(*(this->dl),"Init");
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
		if (it.GetFileType() == RbsLib::Storage::FileType::FileType::Regular&&
			it.GetExtension()==".so")
		{
			try
			{
				ModuleObject obj(it);
				this->modules.insert(std::pair(obj.module_info.module_name,obj));
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
