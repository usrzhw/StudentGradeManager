#pragma once
#include "user.h"
#include <cstdint>
namespace Account
{
	class LoginManager
	{
	public:
		static auto Login(std::uint64_t ID, const std::string& password) -> User;

	};
}