#include "login.h"
#include "user.h"
#include <map>
#include <exception>
#include "generators.h"
#include "../../log/logger.h"
#include <fmt/format.h>

auto Account::LoginManager::Login(std::uint64_t ID, const std::string& password) -> User
{
	int user_permission_level = 4;
	std::string user_pwd;
	if (password.empty()) throw LoginManagerException("未提交密码");
	try
	{
		if (ID >= Generator::STU_ID_START && ID <= Generator::STU_ID_END)
			user_pwd = Account::AccountManager::GetStudentPassword(ID), user_permission_level = 4;
		else if (ID >= Generator::TEACHER_ID_START && ID <= Generator::TEACHER_ID_END)
			user_pwd = Account::AccountManager::GetTeacherPassword(ID), user_permission_level = 2;
		else throw LoginManagerException("用户名或密码错误");
		if (user_pwd!=password) throw LoginManagerException("用户名或密码错误");
	}
	catch (const Account::AccountException& ex)
	{
		throw LoginManagerException("用户名或密码错误");
	}
	catch (const std::exception& ex)
	{
		Logger::LogWarn(fmt::format("用户{}登录时发生异常: {}", ID, ex.what()).c_str());
		throw LoginManagerException("服务器内部错误");
	}
	//密码正确，构造用户描述
	User user();
}

Account::LoginManagerException::LoginManagerException(const std::string& reason) noexcept
	:reason(reason)
{
}

const char* Account::LoginManagerException::what(void) const noexcept
{
	return this->reason.c_str();
}
