#include <iostream>
#include "generators.h"
#include "../../rbslib/FileIO.h"
#include "../../rbslib/Buffer.h"
#include "../../rbslib/Storage.h"
#include <sstream>

using namespace std;

std::uint64_t Generator::StudentsIDGenerator()
{
	using namespace RbsLib::Storage;
	static std::uint64_t id = 0;
	StorageFile id_fp("StudentsID.txt");
	if (id == 0)
	{
		//初次读取ID
		if (id_fp.IsExist())
		{
			std::stringstream(id_fp.Open(FileIO::OpenMode::Read, FileIO::SeekBase::begin).GetLine(128)) >> id;
		}
		if (id == 0) id = STU_ID_START;
	}
	if (id < STU_ID_START || id > STU_ID_END) throw GeneratorException("Student ID number great than max limit");
	std::uint64_t ret_id = id++;
	//写回
	id_fp.Open(FileIO::OpenMode::Write | FileIO::OpenMode::Replace, FileIO::SeekBase::begin).WriteLine(std::to_string(id));
	return ret_id;
}

std::uint64_t Generator::JobGenerator()
{
	using namespace RbsLib::Storage;
	static std::uint64_t id = 0;
	StorageFile id_fp("TeachersID.txt");
	if (id == 0)
	{
		//初次读取ID
		if (id_fp.IsExist())
		{
			std::stringstream(id_fp.Open(FileIO::OpenMode::Read, FileIO::SeekBase::begin).GetLine(128)) >> id;
		}
		if (id <= 0) id = TEACHER_ID_START;
	}
	if (id<TEACHER_ID_START || id>TEACHER_ID_END) throw GeneratorException("Teacher ID number great than max limit");
	std::uint64_t ret_id = id++;
	//写回
	id_fp.Open(FileIO::OpenMode::Write | FileIO::OpenMode::Replace, FileIO::SeekBase::begin).WriteLine(std::to_string(id));
	return ret_id;
}

std::uint64_t Generator::SubjectGenerator()
{
	using namespace RbsLib::Storage;
	static std::uint64_t id = 0;
	StorageFile id_fp("SubjectID.txt");
	if (id == 0)
	{
		//初次读取ID
		if (id_fp.IsExist())
		{
			std::stringstream(id_fp.Open(FileIO::OpenMode::Read, FileIO::SeekBase::begin).GetLine(128)) >> id;
		}
		if (id <= 0) id = SUBJECT_ID_START;
	}
	if (id<SUBJECT_ID_START||id>SUBJECT_ID_END) throw GeneratorException("Subject ID number great than max limit");
	std::uint64_t ret_id = id++;
	//写回
	id_fp.Open(FileIO::OpenMode::Write | FileIO::OpenMode::Replace, FileIO::SeekBase::begin).WriteLine(std::to_string(id));
	return ret_id;
}

Generator::GeneratorException::GeneratorException(const std::string& reason) noexcept
	:reason(reason)
{
}

const char* Generator::GeneratorException::what(void) const noexcept
{
	return this->reason.c_str();
}

Generator::UUID::UUID() noexcept
{
	uuid_clear(this->uuid);
}

Generator::UUID::UUID(const std::string& uuid_str)
{
	if (uuid_parse(uuid_str.c_str(), this->uuid)) throw GeneratorException("UUID string parse failed");
}

Generator::UUID Generator::UUID::Generate(void) noexcept
{

	Generator::UUID id;
	uuid_generate(id.uuid);
	return id;
}

Generator::UUID Generator::UUID::GenerateRandom(void) noexcept
{
	Generator::UUID id;
	uuid_generate_random(id.uuid);
	return id;
}

Generator::UUID Generator::UUID::GenerateTime(void) noexcept
{
	Generator::UUID id;
	uuid_generate_time(id.uuid);
	return id;
}

Generator::UUID Generator::UUID::GenerateTimeSafe(void) noexcept
{
	Generator::UUID id;
	uuid_generate_time_safe(id.uuid);
	return id;
}

auto Generator::UUID::ParseUUIDString(const std::string& uuid_string) -> UUID
{
	return UUID(uuid_string);
}

std::string Generator::UUID::ToString(void) const noexcept
{
	char str[512];
	uuid_unparse(this->uuid, str);
	std::string s;
	return std::string(str);
}

std::string Generator::UUID::ToUpperString(void) const noexcept
{
	char str[512];
	uuid_unparse_upper(this->uuid, str);
	return std::string(str);
}

std::string Generator::UUID::TpLowerString(void) const noexcept
{
	char str[512];
	uuid_unparse_lower(this->uuid, str);
	return std::string(str);
}

bool Generator::UUID::operator==(const UUID& uuid) const noexcept
{
	return !uuid_compare(this->uuid, uuid.uuid);
}

bool Generator::UUID::operator<(const UUID& uuid) const noexcept
{
	if (uuid_compare(this->uuid, uuid.uuid) < 0) return true;
	else return false;
}

bool Generator::UUID::operator>(const UUID& uuid) const noexcept
{
	if (uuid_compare(this->uuid, uuid.uuid) > 0) return true;
	else return false;
}
