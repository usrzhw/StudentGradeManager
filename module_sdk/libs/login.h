#pragma once
#include "user.h"
#include <exception>
#include <cstdint>
namespace Account
{
	class LoginManagerException : std::exception
	{
	private:
		std::string reason;
	public:
		LoginManagerException(const std::string& reason)noexcept;
		const char* what(void) const noexcept override;
	};
	class LoginManager
	{
	public:
		static auto Login(std::uint64_t ID, const std::string& password) -> User;
		static bool CheckToken(std::uint64_t ID, const std::string& token);
		static auto GetOnlineUserInfo(std::uint64_t ID) -> User;
		static void Logout(std::uint64_t ID);
	};
}