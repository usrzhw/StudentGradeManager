// StudentGradeManager.cpp: 定义应用程序的入口点。
//

#include <iostream>
#include <regex>
#include "rbslib/Network.h"
#include "rbslib/Storage.h"
#include "module_loader/loader.h"
using namespace std;
#include <memory>
int main()
{
	RbsLib::Network::HTTP::HTTPServer server(8081);
	Module::ModuleManager module_manager;
	module_manager.LoadModules(RbsLib::Storage::StorageFile("/home/allesugo/.vs/StudentGradeManager/module_sdk"));
	server.SetGetHandle([](const RbsLib::Network::TCP::TCPConnection& x, RbsLib::Network::HTTP::RequestHeader& h) {
		cout << h.ToString() << endl;
		});
	server.SetPostHandle([&module_manager](const RbsLib::Network::TCP::TCPConnection& x, RbsLib::Network::HTTP::RequestHeader& h,const RbsLib::Buffer&buffer) {
		std::regex path_reg("^/run/(\\w+).(\\w+)$");
		std::cmatch m;
		RbsLib::Network::HTTP::ResponseHeader header;
		header.status = 404;
		header.status_descraption = "not found";
		header.headers.AddHeader("Content-Length", "3");
		header.headers.AddHeader("Content-Type", "text/plain");
		RbsLib::Buffer not_found_content(4);
		not_found_content.SetData("404", 3);

		if (true == std::regex_match(h.path.c_str(), m, path_reg))
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
	server.LoopWait();
	return 0;
}
