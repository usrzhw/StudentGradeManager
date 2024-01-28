// StudentGradeManager.cpp: 定义应用程序的入口点。
//

#include <iostream>
#include <regex>
#include "rbslib/Network.h"
using namespace std;

int main()
{
	RbsLib::Network::HTTP::HTTPServer server(8081);
	server.SetGetHandle([](const RbsLib::Network::TCP::TCPConnection& x, RbsLib::Network::HTTP::RequestHeader& h) {
		cout << h.ToString() << endl;
		});
	server.SetPostHandle([](const RbsLib::Network::TCP::TCPConnection& x, RbsLib::Network::HTTP::RequestHeader& h,const RbsLib::Buffer&buffer) {
		cout << h.ToString() << endl;
		cout << buffer.ToString() << endl;
		RbsLib::Network::HTTP::ResponseHeader res;
		res.headers.AddHeader("Content-Length", std::to_string(buffer.GetLength()));
		x.Send(res.ToBuffer());
		x.Send(buffer);
		});
	server.LoopWait();
	return 0;
}
