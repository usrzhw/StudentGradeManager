#include "cache.h"

Cache::CacheException::CacheException(const std::string& reason) noexcept
{
		this->reason = reason;
}

const char* Cache::CacheException::what(void) const noexcept
{
	return this->reason.c_str();
}
