// StudentGradeManager.cpp: 定义应用程序的入口点。
//

#include <iostream>
#include <regex>
#include "rbslib/Network.h"
#include "rbslib/Storage.h"
#include "module_loader/loader.h"
#include "log/logger.h"
#include "configuration_manager/configuration_manager.h"
#include "rbslib/FileIO.h"
using namespace std;
#include <memory>
int main()
{
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
	Logger::LogInfo("端口设置为: %d", Config::Port());
	RbsLib::Network::HTTP::HTTPServer server(Config::Port());
	Module::ModuleManager module_manager;
	Logger::LogInfo("模块路径设置为: %s", Config::ModulePath().c_str());
	module_manager.LoadModules(RbsLib::Storage::StorageFile(Config::ModulePath()));
	server.SetGetHandle([&module_manager](const RbsLib::Network::TCP::TCPConnection& x, RbsLib::Network::HTTP::RequestHeader& h) {
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
			try
			{
				module_manager.GetAction(m[1], m[2])(RbsLib::Network::HTTP::Request(x, h, RbsLib::Buffer(1)));
			}
			catch (Module::ModuleLoaderException const& ex)
			{
				cout << ex.what() << endl;
				x.Send(header.ToBuffer());
				x.Send(not_found_content);
			}
		}
		else if (true == std::regex_match(h.path.c_str(), m, std::regex("^/html/(\\S+)$")))
		{
			
			try
			{
				auto buffer = RbsLib::Storage::StorageFile("html")[RbsLib::Storage::StorageFile(m[1]).GetName()].Open(RbsLib::Storage::FileIO::OpenMode::Read,
					RbsLib::Storage::FileIO::SeekBase::begin).Read(1024*1024);
				RbsLib::Network::HTTP::ResponseHeader header;
				header.status = 200;
				header.status_descraption = "ok";
				header.headers.AddHeader("Content-Length", std::to_string(buffer.GetLength()));
				header.headers.AddHeader("Content-Type", "text/html");
				x.Send(header.ToBuffer());
				x.Send(buffer);
			}
			catch (std::exception const& ex)
			{
				cout << ex.what() << endl;
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
	server.SetPostHandle([&module_manager](const RbsLib::Network::TCP::TCPConnection& x, RbsLib::Network::HTTP::RequestHeader& h,const RbsLib::Buffer&buffer) {
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
			try
			{
				module_manager.GetAction(m[1], m[2])(RbsLib::Network::HTTP::Request(x,h,buffer));
			}
			catch (Module::ModuleLoaderException const& ex)
			{
				cout << ex.what() << endl;
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
	try
	{
		server.LoopWait();
	}
	catch (const std::exception& ex)
	{
		Logger::LogError("HTTP服务核心错误: %s", ex.what());
	}
	return 0;
}
