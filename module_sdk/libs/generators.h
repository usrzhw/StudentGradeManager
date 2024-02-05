#include <iostream>
#include <cstdint>
#include <exception>
#include <uuid/uuid.h>


namespace Generator
{
	constexpr auto STU_ID_START = (1000000);
	constexpr auto STU_ID_END = (9999999);
	constexpr auto TEACHER_ID_START = (100000);
	constexpr auto TEACHER_ID_END = (999999);
	constexpr auto SUBJECT_ID_START = (10000000);
	constexpr auto SUBJECT_ID_END = (99999999);
	class GeneratorException :std::exception
	{
	private:
		std::string reason;
	public:
		GeneratorException(const std::string& reason) noexcept;
		const char* what(void) const noexcept override;
	};
	std::uint64_t StudentsIDGenerator();
	std::uint64_t TeacherIDGenerator();
	std::uint64_t SubjectGenerator();
	bool IsStudentID(std::uint64_t id);
	bool IsTecherID(std::uint64_t id);
	bool IsSubjectID(std::uint64_t id);
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
