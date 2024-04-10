#pragma once

#include <string>
#include <exception>
#include <map>
#include <shared_mutex>
#include <memory>
#include "../rbslib/Buffer.h"


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
		std::map<std::string,RbsLib::Buffer> cache;
		std::size_t max_cache_size;
	public:
		StaticResourceCache(std::size_t max_cache_size);
		void add(const std::string& key, const RbsLib::Buffer& value);
		RbsLib::Buffer get(const std::string& key);
		bool contains(const std::string& key);
		void remove(const std::string& key);
		std::size_t size(void);
		std::size_t max_size(void);
	};
}