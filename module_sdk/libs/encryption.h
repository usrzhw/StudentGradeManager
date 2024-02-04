#pragma once
#include <string>
#include <exception>
#include "../../rbslib/Buffer.h"
namespace Encryption
{
	class EncryptionException :std::exception
	{
	private:
		std::string reason;
	public:
		EncryptionException(const std::string& ex)noexcept;
		const char* what(void) const noexcept override;
	};
	class Base64
	{
	public:
		static RbsLib::Buffer Decode(const std::string& str);
		static std::string Encode(const RbsLib::IBuffer & buffer);
	};
	class MD5
	{
	public:
		static RbsLib::Buffer Encode(const RbsLib::IBuffer& buffer);
		static std::string ResultToString(const RbsLib::IBuffer& result);
	};
}