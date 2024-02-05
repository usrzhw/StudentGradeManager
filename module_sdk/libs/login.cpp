#include "login.h"
#include "user.h"
#include <map>
#include <exception>
#include "generators.h"
#include "../../log/logger.h"
#include <fmt/format.h>
#include "encryption.h"
#include <shared_mutex>
#include <mutex>

static std::map<std::uint64_t, Account::User> Online_Users;
static std::shared_mutex Mutex;

auto Account::LoginManager::Login(std::uint64_t ID, const std::string& password) -> User
{
	int user_permission_level = 4;
	User user;
	std::string user_pwd;
	if (password.empty()) throw LoginManagerException("未提交密码");
	try
	{
		if (ID >= Generator::STU_ID_START && ID <= Generator::STU_ID_END)
		{
			auto info = Account::AccountManager::GetStudentInfo(ID);
			user_pwd = info.password;
			user.ID = info.id;
			user.name = info.name;
			user.permission_level = info.permission_level;
			
		}
		else if (ID >= Generator::TEACHER_ID_START && ID <= Generator::TEACHER_ID_END)
		{
			auto info = Account::AccountManager::GetTeacherInfo(ID);
			user_pwd = info.password;
			user.ID = info.id;
			user.name = info.name;
			user.permission_level = info.permission_level;
		}
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
	user.enable_time = std::time(nullptr) + 3600;
	user.token = Generator::UUID::GenerateTimeSafe().ToString();
	user.token += Generator::UUID::GenerateRandom().ToString();
	user.token = Encryption::MD5::ResultToString(Encryption::MD5::Encode(RbsLib::Buffer(user.token)));
	std::unique_lock<std::shared_mutex> lock(Mutex);
	Online_Users[ID] = user;
	return user;
}

bool Account::LoginManager::CheckToken(std::uint64_t ID, const std::string& token)
{
	std::shared_lock<std::shared_mutex> lock(Mutex);
	auto p = Online_Users.find(ID);
	if (p != Online_Users.end())
		if (p->second.token == token && std::time(nullptr) <= p->second.enable_time)
			return true;
	return false;
}

auto Account::LoginManager::GetOnlineUserInfo(std::uint64_t ID) -> User
{
	std::shared_lock<std::shared_mutex> lock(Mutex);
	auto p = Online_Users.find(ID);
	if (p != Online_Users.end()) return p->second;
}

void Account::LoginManager::Logout(std::uint64_t ID)
{
	std::unique_lock<std::shared_mutex> lock(Mutex);
	Online_Users.erase(ID);
}

Account::LoginManagerException::LoginManagerException(const std::string& reason) noexcept
	:reason(reason)
{
}

const char* Account::LoginManagerException::what(void) const noexcept
{
	return this->reason.c_str();
}
