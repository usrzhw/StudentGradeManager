#include <iostream>
#include <cstdint>
#include <exception>
#include <uuid/uuid.h>
using namespace std;

namespace Generator
{
	class GeneratorException :std::exception
	{
	private:
		std::string reason;
	public:
		GeneratorException(const std::string& reason) noexcept;
		const char* what(void) const noexcept override;
	};
	static std::uint64_t StudentsIDGenerator();
	static std::uint64_t JobGenerator();
	static std::uint64_t SubjectGenerator();
	class UUID
	{
	private:
		uuid_t uuid;
	public:
		UUID() noexcept;
		UUID(const std::string& uuid_str);
		static UUID Generate(void) noexcept;
		static UUID GenerateRandom(void) noexcept;
		static UUID GenerateTime(void) noexcept;
		static UUID GenerateTimeSafe(void) noexcept;
		static auto ParseUUIDString(const std::string& uuid_string) -> UUID;
		std::string ToString(void)const noexcept;
		std::string ToUpperString(void)const noexcept;
		std::string TpLowerString(void)const noexcept;
		bool operator==(const UUID& uuid) const noexcept;
		bool operator<(const UUID& uuid) const noexcept;
		bool operator>(const UUID& uuid) const noexcept;
	};
};
