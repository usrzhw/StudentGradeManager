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
		if (id == 0) id = 100000;
	}
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
		if (id <= 0) id = 10000;
	}
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
		if (id <= 0) id = 1000000;
	}
	std::uint64_t ret_id = id++;
	//写回
	id_fp.Open(FileIO::OpenMode::Write | FileIO::OpenMode::Replace, FileIO::SeekBase::begin).WriteLine(std::to_string(id));
	return ret_id;
}