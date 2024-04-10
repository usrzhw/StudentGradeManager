// StudentGradeManager.cpp: 定义应用程序的入口点。
//
#include <csignal>
#include <iostream>
#include <regex>
#include "rbslib/Network.h"
#include "rbslib/Storage.h"
#include "module_loader/loader.h"
#include "log/logger.h"
#include "configuration_manager/configuration_manager.h"
#include "rbslib/FileIO.h"
#include <memory>
#include <execinfo.h>
#include "rbslib/Commandline.h"
#include <sys/prctl.h>
#include <string>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <sys/sendfile.h>
#include <sys/file.h>
#include <memory>
#include "cache/cache.h"
using namespace std;

static std::shared_mutex Global_Module_List_Lock;
static bool IsExitProgram = false;

static void ExitProgram();
static void SignalHandle(int sig);

void PrintTrace(void)
{
	void* array[20];
	size_t size;
	char** strings;
	size_t i;
	size = backtrace(array, 20);
	strings = backtrace_symbols(array, size);
	Logger::LogError("Obtained %zd stack frames.", size);
	for (i = 0; i < size; i++)
	{
		Logger::LogError("%s", strings[i]);
	}
	free(strings);
}

static void SignalHandle(int sig)
{
	switch (sig)
	{
	case SIGSEGV:
	
		Logger::LogError("段错误，程序将在尝试捕获错误后返回错误点");
		PrintTrace();
		signal(sig, SIG_DFL);
		break;
	case SIGABRT:
		Logger::LogError("程序异常退出");
		PrintTrace();
		signal(sig, SIG_DFL);
		break;
	case SIGINT:
		ExitProgram();
		break;
	default:
		break;
	}
}

static void ExitProgram()
{
	Logger::LogInfo("正在退出服务器");
	IsExitProgram = true;
	fcloseall();
	exit(0);
}

void BackgroundCommandThread(Module::ModuleManager* modules)
{
	prctl(PR_SET_NAME, "BackgroundCommandThread");
	std::string cmd_str;
	RbsLib::Command::CommandLine cmdline;
	while (true)
	{
		std::getline(cin,cmd_str);
		cmdline.Parse(cmd_str);
		if (cmdline[0]=="") continue;
		if (cmdline[0] == "exit")
		{
			raise(SIGINT);
		}
		else if (cmdline[0] == "module")
		{
			if (cmdline[1] == "list")
			{
				std::shared_lock<std::shared_mutex> lock(Global_Module_List_Lock);
				for (const auto& it : modules->GetModules())
				{
					Logger::LogInfo("模块: %s,版本: %s,描述: %s", it.second.module_info.module_name.c_str(), it.second.module_info.module_version.c_str(),it.second.module_info.module_description.c_str());
				}
			}
			else if (cmdline[1] == "unload")
			{
				std::unique_lock<std::shared_mutex> lock(Global_Module_List_Lock);
				modules->UnloadModule(cmdline[2]);
			}
			else if (cmdline[1] == "reload")
			{
				try
				{
					std::unique_lock<std::shared_mutex> lock(Global_Module_List_Lock);
					modules->UnloadAllModules();
					modules->LoadModules(RbsLib::Storage::StorageFile(Config::ModulePath()));
				}
				catch (const Module::ModuleLoaderException& ex)
				{
					Logger::LogError("模块加载器错误: %s", ex.what());
				}
			}
			else if (cmdline[1] == "defaultpath")
			{
				Logger::LogInfo("%s",Config::ModulePath().c_str());
			}
			else if (cmdline[1]=="help")
			{
				Logger::LogInfo("module list: 列出所有已加载的模块");
				Logger::LogInfo("module unload <module_name>: 卸载指定模块");
				Logger::LogInfo("module reload: 重新加载模块");
				Logger::LogInfo("module defaultpath: 获取默认模块路径");
			}
			else
			{
				Logger::LogError("未知的模块命令,使用module help获取帮助");
			}
		}
		else if (cmdline[0] == "help")
		{
			Logger::LogInfo("exit: 退出程序");
			Logger::LogInfo("help: 显示帮助");
		}
		else
		{
			try
			{
				modules->ExecuteCommand(cmdline);
			}
			catch (const Module::ModuleLoaderException& ex)
			{
				Logger::LogError("模块加载器错误: %s", ex.what());
			}
			catch (const std::exception& ex)
			{
				Logger::LogError("未知错误: %s", ex.what());
			}
		}
	}
}

int main(int argc,const char**argv)
{
	std::signal(SIGPIPE, SignalHandle);
	std::signal(SIGINT, SignalHandle);
	std::signal(SIGSEGV, SignalHandle);
	std::signal(SIGABRT, SignalHandle);

	RbsLib::Command::CommandLine cmdline(argc, argv);
	


	//读取配置文件
	Logger::LogInfo("读取配置文件");
	try
	{
		Config::LoadConfig();
	}
	catch (std::exception const& ex)
	{
		Logger::LogError("读取配置文件失败: %s", ex.what());
		Logger::LogWarn("使用默认设置启动");
		Logger::LogInfo("保存默认的配置文件");
		Config::SaveConfig();
	}
	Logger::LogInfo("当前进程ID: %d", getpid());
	Logger::LogInfo("端口设置为: %d", Config::Port());
	RbsLib::Network::HTTP::HTTPServer server(Config::Port());
	Module::ModuleManager module_manager;
	Logger::LogInfo("模块路径设置为: %s", Config::ModulePath().c_str());
	Logger::LogInfo("开始加载模块");
	std::unique_lock<std::shared_mutex> lock(Global_Module_List_Lock);
	module_manager.LoadModules(RbsLib::Storage::StorageFile(Config::ModulePath()));
	lock.unlock();
	Logger::LogInfo("模块加载完成,已加载的模块列表:");
	std::shared_lock<std::shared_mutex> lock1(Global_Module_List_Lock);
	for (auto& x : module_manager.GetModules())
	{
		Logger::LogInfo("模块: %s,版本: %s",x.second.module_info.module_name.c_str(), x.second.module_info.module_version.c_str());
	}
	lock1.unlock();
	server.SetGetHandle([&module_manager](const RbsLib::Network::TCP::TCPConnection& x, RbsLib::Network::HTTP::RequestHeader& h) {
		//检查路径是否为根路径
		if (h.path == "/")
		{
			//构造302跳转
			RbsLib::Network::HTTP::ResponseHeader header;
			header.status = 302;
			header.headers.AddHeader("Location", "/html/login.html");
			x.Send(header.ToBuffer());
			return;
		}
		std::cmatch m;
		RbsLib::Network::HTTP::ResponseHeader header;
		header.status = 404;
		header.status_descraption = "not found";
		header.headers.AddHeader("Content-Length", "3");
		header.headers.AddHeader("Content-Type", "text/plain");
		RbsLib::Buffer not_found_content(4);
		not_found_content.SetData("404", 3);

		if (true == std::regex_match(h.path.c_str(), m, std::regex("^/app/(\\w+).(\\w+)$")))
		{
			std::shared_lock<std::shared_mutex> lock(Global_Module_List_Lock);
			try
			{
				module_manager.GetAction(m[1], m[2])(RbsLib::Network::HTTP::Request(x, h, RbsLib::Buffer(1)));
			}
			catch (Module::ModuleLoaderException const& ex)
			{
				Logger::LogWarn("模块%s.%s抛出异常:%s", m[1].str().c_str(), m[2].str().c_str(), ex.what());
				x.Send(header.ToBuffer());
				x.Send(not_found_content);
			}
		}
		else if (true == std::regex_match(h.path.c_str(), m, std::regex("^/html/(\\S+)$")))
		{
			try
			{
				auto file = RbsLib::Storage::StorageFile("html")[RbsLib::Storage::StorageFile(m[1]).GetName()];
				std::shared_ptr<int> fd(new int(open(file.Path().c_str(), O_RDONLY)), [](int* p) {if (*p != -1) close(*p); delete p; });
				if (*fd == -1) throw std::runtime_error("文件打开失败");
				std::size_t file_size = file.GetFileSize();
				RbsLib::Network::HTTP::ResponseHeader header;
				header.status = 200;
				header.status_descraption = "ok";
				header.headers.AddHeader("Content-Length", std::to_string(file_size));
				header.headers.AddHeader("Content-Type", "text/html");
				x.Send(header.ToBuffer());
				sendfile(x.GetSocket(),*fd, nullptr, file_size);
			}
			catch (const std::exception& ex)
			{
				Logger::LogWarn("加载网页%s时抛出异常:%s", m[1].str().c_str(), ex.what());
				x.Send(header.ToBuffer());
				x.Send(not_found_content);
			}
		}
		else
		{
			x.Send(header.ToBuffer());
			x.Send(not_found_content);
		}
		});
	server.SetPostHandle([&module_manager](const RbsLib::Network::TCP::TCPConnection& x, RbsLib::Network::HTTP::RequestHeader& h, const RbsLib::Buffer& buffer) {
		std::cmatch m;
		RbsLib::Network::HTTP::ResponseHeader header;
		header.status = 404;
		header.status_descraption = "not found";
		header.headers.AddHeader("Content-Length", "3");
		header.headers.AddHeader("Content-Type", "text/plain");
		RbsLib::Buffer not_found_content(4);
		not_found_content.SetData("404", 3);
		auto debug = buffer.ToString();

		if (true == std::regex_match(h.path.c_str(), m, std::regex("^/app/(\\w+).(\\w+)$")))
		{
			std::shared_lock<std::shared_mutex> lock(Global_Module_List_Lock);
			try
			{
				module_manager.GetAction(m[1], m[2])(RbsLib::Network::HTTP::Request(x, h, buffer));
			}
			catch (Module::ModuleLoaderException const& ex)
			{
				Logger::LogWarn("模块%s.%s抛出异常:%s", m[1].str().c_str(), m[2].str().c_str(), ex.what());
				x.Send(header.ToBuffer());
				x.Send(not_found_content);
			}
			catch (const std::exception& ex)
			{
				Logger::LogWarn("已引发异常:%s", ex.what());
			}
		}
		else
		{
			x.Send(header.ToBuffer());
			x.Send(not_found_content);
		}

		});
	std::thread background_command_thread(BackgroundCommandThread,&module_manager);
	try
	{
		server.LoopWait(true,20);
	}
	catch (const std::exception& ex)
	{
		Logger::LogError("HTTP服务核心错误: %s", ex.what());
	}
	return 0;
}
