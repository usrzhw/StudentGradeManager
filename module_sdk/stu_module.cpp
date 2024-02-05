#include "module_sdk.h"
#include <iostream>
#include <sstream>
#include <string>
#include "../json/CJsonObject.h"
#include "libs/user.h"
#include "libs/login.h"
#include "../log/logger.h"
#include "libs/generators.h"

static void SendError(const RbsLib::Network::TCP::TCPConnection& connection,
	const std::string& message, int status)
{
	try
	{
		neb::CJsonObject robj;
		robj.Add("message", message);
		RbsLib::Network::HTTP::ResponseHeader header;
		header.status = status;
		header.status_descraption = "";
		header.headers.AddHeader("Content-Type", "application/json");
		std::string json = robj.ToFormattedString();
		header.headers.AddHeader("Content-Length", std::to_string(json.length()));
		connection.Send(header.ToBuffer());
		connection.Send(RbsLib::Buffer(json));
	}
	catch (const std::exception& ex)
	{
	}
}
static void SendSuccessResponse(const RbsLib::Network::TCP::TCPConnection& connection, const neb::CJsonObject & obj)
{
	try
	{
		RbsLib::Network::HTTP::ResponseHeader header;
		header.status = 200;
		header.status_descraption = "ok";
		header.headers.AddHeader("Content-Type", "application/json");
		std::string json = obj.ToFormattedString();
		header.headers.AddHeader("Content-Length", std::to_string(json.length()));
		connection.Send(header.ToBuffer());
		connection.Send(RbsLib::Buffer(json));
	}
	catch (const std::exception& ex)
	{
	}
}

void Login(const RbsLib::Network::HTTP::Request& request)
{
	neb::CJsonObject obj(request.content.ToString());
	try
	{
		uint64_t id;
		std::stringstream(obj("ID")) >> id;
		auto user = Account::LoginManager::Login(id, obj("Password"));
		Logger::LogInfo("用户[%d:%s]登录成功", user.ID,user.name.c_str());
		neb::CJsonObject robj;
		robj.Add("id", user.ID);
		robj.Add("token", user.token);
		robj.Add("username", user.name);
		robj.Add("permission", user.permission_level);
		robj.Add("message", "ok");
		RbsLib::Network::HTTP::ResponseHeader header;
		header.status = 200;
		header.status_descraption = "ok";
		header.headers.AddHeader("Content-Type", "application/json");
		std::string json = robj.ToFormattedString();
		header.headers.AddHeader("Content-Length", std::to_string(json.length()));
		request.connection.Send(header.ToBuffer());
		request.connection.Send(RbsLib::Buffer(json));
	}
	catch (const Account::LoginManagerException& ex)
	{
		neb::CJsonObject robj;
		robj.Add("message", ex.what());
		RbsLib::Network::HTTP::ResponseHeader header;
		header.status = 403;
		header.status_descraption = "ok";
		header.headers.AddHeader("Content-Type", "application/json");
		std::string json = robj.ToFormattedString();
		header.headers.AddHeader("Content-Length", std::to_string(json.length()));
		request.connection.Send(header.ToBuffer());
		request.connection.Send(RbsLib::Buffer(json));
	}
	catch (const std::exception& ex)
	{
		neb::CJsonObject robj;
		robj.Add("message", "服务器内部错误");
		RbsLib::Network::HTTP::ResponseHeader header;
		header.status = 503;
		header.status_descraption = "ok";
		header.headers.AddHeader("Content-Type", "application/json");
		std::string json = robj.ToFormattedString();
		header.headers.AddHeader("Content-Length", std::to_string(json.length()));
		request.connection.Send(header.ToBuffer());
		request.connection.Send(RbsLib::Buffer(json));
	}
	
}

void Logout(const RbsLib::Network::HTTP::Request& request)
{
	neb::CJsonObject obj(request.content.ToString());
	RbsLib::Network::HTTP::ResponseHeader header;
	//检查权限
	std::uint64_t ID, target_id = 0;
	std::stringstream(obj("ID")) >> ID;
	if (false == Account::LoginManager::CheckToken(ID, obj("token")))
	{
		SendError(request.connection, "invailed token", 403);
		return;
	}
	auto basic_info = Account::LoginManager::GetOnlineUserInfo(ID);
	std::stringstream(obj("target_id")) >> target_id;
	if (target_id == 0) return SendError(request.connection, "参数错误", 422);
	obj.Clear();
	//获取目标信息
	if (Generator::IsStudentID(target_id))
	{
		auto target_info = Account::AccountManager::GetStudentInfo(target_id);
		if ((target_info.is_enable == false && basic_info.permission_level == 0) ||
			(target_info.is_enable == true && target_info.permission_level < basic_info.permission_level ||
				target_id == ID))
		{
			Account::LoginManager::Logout(target_id);
			Logger::LogInfo("用户[%d:%s]退出了[%d:%s]账号", ID, basic_info.name.c_str(),
				target_info.id, target_info.name.c_str());
		}
		else return SendError(request.connection, "permission denied", 403);
	}
	else if (Generator::IsTecherID(target_id))
	{
		auto target_info = Account::AccountManager::GetTeacherInfo(target_id);
		if ((target_info.is_enable == false && basic_info.permission_level == 0) ||
			(target_info.is_enable == true && target_info.permission_level < basic_info.permission_level ||
				target_id == ID))
		{
			Account::LoginManager::Logout(target_id);
			Logger::LogInfo("用户[%d:%s]退出了[%d:%s]账号", ID, basic_info.name.c_str(),
				target_info.id, target_info.name.c_str());
		}
		else return SendError(request.connection, "permission denied", 403);
	}
	else return SendError(request.connection, "permission denied", 403);
	obj.Add("message", "ok");
	return SendSuccessResponse(request.connection, obj);
}


void GetUserInfo(const RbsLib::Network::HTTP::Request& request)
{
	neb::CJsonObject obj(request.content.ToString());
	RbsLib::Network::HTTP::ResponseHeader header;
	//检查权限
	std::uint64_t ID,target_id=0;
	std::stringstream(obj("ID")) >> ID;
	if (false == Account::LoginManager::CheckToken(ID, obj("token")))
	{
		SendError(request.connection, "invailed token", 403);
		return;
	}
	auto basic_info = Account::LoginManager::GetOnlineUserInfo(ID);
	std::stringstream(obj("target_id")) >> target_id;
	if (target_id == 0) return SendError(request.connection,"参数错误",422);
	obj.Clear();
	//获取目标信息
	if (Generator::IsStudentID(target_id))
	{
		auto target_info = Account::AccountManager::GetStudentInfo(target_id);
		if ((target_info.is_enable==false && basic_info.permission_level == 0)||
			(target_info.is_enable == true && target_info.permission_level < basic_info.permission_level ||
			target_id == ID))
		{
			obj.Add("type", "student");
			obj.Add("name", target_info.name);
			obj.Add("sex", target_info.sex);
			obj.Add("phone_number", target_info.phone_number);
			obj.Add("email", target_info.email);
			obj.Add("enrollment_date", target_info.enrollment_date);
			obj.Add("class_name", target_info.class_name);
			obj.Add("college", target_info.college);
			obj.Add("notes", target_info.notes);
			obj.Add("permission_level", target_info.permission_level);
			Logger::LogInfo("用户[%d:%s]获取了[%d:%s]的个人信息", ID, basic_info.name.c_str(),
				target_info.id, target_info.name.c_str());
		}
		else return SendError(request.connection, "permission denied", 403);
	}
	else if (Generator::IsTecherID(target_id))
	{
		auto target_info = Account::AccountManager::GetTeacherInfo(target_id);
		if ((target_info.is_enable == false && basic_info.permission_level == 0) ||
			(target_info.is_enable == true && target_info.permission_level < basic_info.permission_level ||
				target_id == ID))
		{
			obj.Add("type", "student");
			obj.Add("name", target_info.name);
			obj.Add("sex", target_info.sex);
			obj.Add("phone_number", target_info.phone_number);
			obj.Add("email", target_info.email);
			obj.Add("college", target_info.college);
			obj.Add("notes", target_info.notes);
			obj.Add("permission_level", target_info.permission_level);
			Logger::LogInfo("用户[%d:%s]获取了[%d:%s]的个人信息", ID, basic_info.name.c_str(),
				target_info.id, target_info.name.c_str());
		}
		else return SendError(request.connection, "permission denied", 403);
	}
	else return SendError(request.connection, "permission denied", 403);
	obj.Add("message", "ok");
	return SendSuccessResponse(request.connection, obj);
}

void GetStudentSubjects(const RbsLib::Network::HTTP::Request& request)
{
	neb::CJsonObject obj(request.content.ToString());
	RbsLib::Network::HTTP::ResponseHeader header;
	//检查权限
	std::uint64_t ID, target_id = 0;
	std::stringstream(obj("ID")) >> ID;
	if (false == Account::LoginManager::CheckToken(ID, obj("token")))
	{
		SendError(request.connection, "invailed token", 403);
		return;
	}
	auto basic_info = Account::LoginManager::GetOnlineUserInfo(ID);
	std::stringstream(obj("target_id")) >> target_id;
	if (target_id == 0) return SendError(request.connection, "参数错误", 422);
	obj.Clear();
	//获取目标信息
	if (Generator::IsStudentID(target_id)&&Account::AccountManager::IsStudentExist(target_id))
	{
		auto target_info = Account::AccountManager::GetStudentInfo(target_id);
		if ((target_info.is_enable == false && basic_info.permission_level == 0) ||
			(target_info.is_enable == true && target_info.permission_level < basic_info.permission_level ||
				target_id == ID))
		{
			obj.AddEmptySubArray("subjects");
			for (auto it : target_info.subjects)
			{
				neb::CJsonObject subobj;
				if (Account::SubjectManager::IsSubjectExist(it))
				{
					auto info = Account::SubjectManager::GetSubjectInfo(it);
					subobj.Add("name", info.name);
					subobj.Add("ID", info.id);
					subobj.Add("classroom", info.classroom);
					subobj.Add("start", info.semester_start);
					subobj.Add("end", info.semester_end);
					subobj.Add("description", info.description);
					//查找课程中的该学生
					for (auto& it : info.students)
					{
						if (it.id == target_id)
						{
							subobj.Add("student_note", it.notes);
							subobj.Add("grade", it.grade);
						}
					}
					//获取该课程的老师信息
					subobj.AddEmptySubArray("teachers");
					for (auto it : info.teachers)
					{
						if (Account::AccountManager::IsTeacherExist(it))
						{
							auto tinfo = Account::AccountManager::GetTeacherInfo(it);
							subobj["teachers"].Add(tinfo.name);
						}
					}
				}
				obj["subjects"].Add(subobj);
			}
			Logger::LogInfo("用户[%d:%s]获取了[%d:%s]的课程信息", ID, basic_info.name.c_str(),
				target_info.id, target_info.name.c_str());
		}
		else return SendError(request.connection, "permission denied", 403);
	} else return SendError(request.connection, "permission denied", 403);
	obj.Add("message", "ok");
	return SendSuccessResponse(request.connection, obj);
}


//初始化函数，用于模块自身的初始化，主要是描述模块名称版本函数等信息
ModuleSDK::ModuleInfo Init(void)
{
	Logger::LogInfo("正在加载学生成绩管理系统模块");

	//创建模块信息结构体，并分别填入模块名称、版本、描述
	ModuleSDK::ModuleInfo info("stu", "1.0.0", "学生成绩管理模块");

	//为模块添加一个名称为func的方法，当请求func方法时会执行func1函数

	//再添加一个方法
	info.Add("login", Login);
	info.Add("get_user_info", GetUserInfo);
	info.Add("logout", Logout);
	info.Add("get_student_subjects", GetStudentSubjects);
	//将模块信息返回
	return info;
}