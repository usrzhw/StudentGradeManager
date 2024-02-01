#include "module_sdk.h"
#include <iostream>
#include "../rbslib/Buffer.h"
#include "../rbslib/Storage.h"
#include "../rbslib/FileIO.h"

//两个函数
void func1(const RbsLib::Network::HTTP::Request& request)
{
	std::cout << "登录" << std::endl;	
	RbsLib::Network::HTTP::ResponseHeader header;
	header.status = 200;
	header.status_descraption = "ok";
	header.headers.AddHeader("Content-Length", "5");
	request.connection.Send(header.ToBuffer());
	request.connection.Send("hello", 5);
}
void func2(const RbsLib::Network::HTTP::Request& request)
{
	std::cout << "test2" << std::endl;
}

//初始化函数，用于模块自身的初始化，主要是描述模块名称版本函数等信息
ModuleSDK::ModuleInfo Init(void)
{
	int a = 10086;
	RbsLib::Storage::StorageFile b("ID.txt");
	auto fp = b.Open(RbsLib::Storage::FileIO::OpenMode::Write|RbsLib::Storage::FileIO::OpenMode::Replace,
		RbsLib::Storage::FileIO::SeekBase::begin);
	fp.WriteLine(std::to_string(a));
	return ModuleSDK::ModuleInfo("123", "", "");
}