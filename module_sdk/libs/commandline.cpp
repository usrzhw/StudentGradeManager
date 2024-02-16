#include "commandline.h"
#include "../../log/logger.h"
#include "user.h"
#include <exception>
#include <fmt/format.h>

static std::string cmd_list(const std::vector<std::string>& args,int i)
{
	++i;
	if (args.at(i) == "--teacher")
	{
		//列出所有老师的信息
		auto info_list = Account::AccountManager::GetAllTeacherInfo();
		std::string result="ID\tName\tSex\tPhone\tEmail\tCollege\tPermission\tNotes\tEnable\n";
		for (auto& it : info_list)
		{
			result += fmt::format("{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\n",
				it.id, it.name, it.sex, it.phone_number, it.email, it.college,
				it.permission_level, it.notes, it.is_enable);
		}
		return result;
	}
	else if (args.at(i) == "--student")
	{
		//列出所有学生的信息
		auto info_list = Account::AccountManager::GetAllStudentInfo();
		std::string result = "ID\tName\tSex\tPhone\tEmail\tCollege\tEnrollmentDate\tPermission\tNotes\tEnable\n";
		for (auto& it : info_list)
		{
			result += fmt::format("{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\n",
								it.id, it.name,it.sex,it.phone_number,it.email,it.college,
				it.enrollment_date,it.permission_level,it.notes,it.is_enable);
			return result;
		}
	}
	else if (args.at(i) == "--subject")
	{
		//列出所有课程的信息
		auto info_list = Account::SubjectManager::GetAllSubjectInfo();
		std::string result = "ID\tName\tDescription\n";
		for (auto& it : info_list)
		{
			result += fmt::format("{}\t{}\t{}\n", it.id, it.name, it.description);
		}
		return result;
	}
	else if (args.at(i) == "--class")
	{
		//列出所有班级的信息
		
	}
	else if (args.at(i) == "--user")
	{
		//列出所有用户信息
	}
	else if (args.at(i) == "-i")
	{
		//显示指定id的信息
	}
	else if (args.at(i) == "-h")
	{

	}
	else
	{
		return "Unknown option";
	}
}

std::string CommandLine::Execute(const std::vector<std::string>& args)
{
	if (args.size() == 0) return "No command";
	try
	{
		if (args.at(0) == "list") return cmd_list(args, 0);
		else return "Unknown command";
	}
	catch (const std::exception& e)
	{
		return "Execute failed";
	}
}

const std::vector<std::string> CommandLine::Parse(const std::string& command)
{
	std::vector<std::string> args;
	std::string arg;
	bool is_in = false;//标识是否在有效解析部分中
	bool q = false;//标识是否在双引号引起的部分中

	for (auto it : command)
	{
		if (q == true)
		{
			//当前在引号中
			if (it == '\"')
			{
				//离开引号区域
				q = false;
			}
			else arg.push_back(it);
		}
		else
		{
			//当前不在引号中
			if (it == ' ')
			{
				if (is_in == true)
				{
					//终止有效部分解析
					is_in = false;
					if (arg != "") args.push_back(arg);
					arg.clear();
				}
			}
			else
			{
				//进入有效解析区域
				is_in = true;
				if (it == '\"') q = true;//进入引号区域
				else arg.push_back(it);
			}
		}
	}
	if (!arg.empty()) args.push_back(arg);
	return args;
}
