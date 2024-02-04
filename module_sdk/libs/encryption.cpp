#include "encryption.h"
#include <memory>
#include <cstring>
#include "md5.h"
#include "base64.h"
RbsLib::Buffer Encryption::Base64::Decode(const std::string& str)
{
	char* decode_out = new char[BASE64_DECODE_OUT_SIZE(str.length())];
	if (0 == base64_decode(str.c_str(), str.length(), (unsigned char*)decode_out))
	{
		delete[] decode_out;
		throw EncryptionException("Base64 decode error");
	}
	RbsLib::Buffer result(decode_out, BASE64_DECODE_OUT_SIZE(str.length()));
	delete[] decode_out;
	return result;
}

std::string Encryption::Base64::Encode(const RbsLib::IBuffer & buffer)
{
	char* encode_out = new char[BASE64_ENCODE_OUT_SIZE(buffer.GetLength())];
	base64_encode((unsigned char*)buffer.Data(), buffer.GetLength(), encode_out);
	std::string result(encode_out);
	delete[] encode_out;
	return result;
}

Encryption::EncryptionException::EncryptionException(const std::string& ex) noexcept
	:reason(reason)
{
}

const char* Encryption::EncryptionException::what(void) const noexcept
{
	return reason.c_str();
}

RbsLib::Buffer Encryption::MD5::Encode(const RbsLib::IBuffer& buffer)
{
	MD5_CTX md5;
	MD5Init(&md5);
	MD5Update(&md5, (unsigned char*)buffer.Data(), buffer.GetLength());
	std::unique_ptr<unsigned char[]> ptr = std::make_unique<unsigned char[]>(16);
	MD5Final(&md5, ptr.get());
	return RbsLib::Buffer(ptr.get(), 16);
}

std::string Encryption::MD5::ResultToString(const RbsLib::IBuffer& result)
{
	if (result.GetLength() != 16) throw EncryptionException("Result to string failed: length != 16");
	const unsigned char* p = (const unsigned char*)result.Data();
	char str[128];
	std::sprintf(str, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
		p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11],
		p[12], p[13], p[14], p[15]);
	return str;
}
