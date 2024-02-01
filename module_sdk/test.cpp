#include "module_sdk.h"
#include <iostream>

//两个函数
void func1(const RbsLib::Network::HTTP::Request& request)
{
	std::cout << "test" << std::endl;
	request.connection;//这是TCP连接，向客户端返回数据依赖该项
	request.header;//客户端发送的HTTP请求标头
	request.content;//客户端发送的HTTP请求内容(仅POST请求有效)
}
void Login(const RbsLib::Network::HTTP::Request& request)
{
	std::cout << "login" << std::endl;
	std::cout << request.content.ToString() << std::endl;
	RbsLib::Network::HTTP::ResponseHeader header;
	header.status = 200;
	header.status_descraption = "ok";
	header.headers.AddHeader("Content-Type", "application/json");
	std::string json = "{\"message\":\"ok\",\"token\":\"12345\"}";
	header.headers.AddHeader("Content-Length", std::to_string(json.length()));
	request.connection.Send(header.ToBuffer());
	request.connection.Send(RbsLib::Buffer(json));
}

//初始化函数，用于模块自身的初始化，主要是描述模块名称版本函数等信息
ModuleSDK::ModuleInfo Init(void)
{
	//创建模块信息结构体，并分别填入模块名称、版本、描述
	ModuleSDK::ModuleInfo info("test", "1.0.0", "测试");

	//为模块添加一个名称为func的方法，当请求func方法时会执行func1函数
	info.Add("func", func1);

	//再添加一个方法
	info.Add("login", Login);

	//将模块信息返回
	return info;
}