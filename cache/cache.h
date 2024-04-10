#pragma once

#include <string>
#include <exception>
#include <map>
#include <shared_mutex>
#include <memory>


namespace Cache
{
	class CacheException : public std::exception
	{
	private:
		std::string reason;
	public:
		CacheException(const std::string& reason) noexcept;
		const char* what(void) const noexcept override;
	};
	class StaticResourceCache
	{
	private:

	};
}