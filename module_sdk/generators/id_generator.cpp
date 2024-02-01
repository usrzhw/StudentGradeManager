#include <iostream>
#include "Generator.h"
#include "../../rbslib/FileIO.h"
#include "../rbslib/Buffer.h"
#include "../rbslib/Storage.h"
using namespace std;

void Generator::Id_generator()
{
	int id;
	RbsLib::Storage::StorageFile b("ID.txt");
	auto fp = b.Open(RbsLib::Storage::FileIO::OpenMode::Read | RbsLib::Storage::FileIO::OpenMode::Write | RbsLib::Storage::FileIO::OpenMode::Replace,
		RbsLib::Storage::FileIO::SeekBase::begin);
	id = fp.GetLine(12);
	id = id + 1;
	fp.WriteLine(std::to_string(id));
}

void Generator::Job_Generator()
{
	int job;
	RbsLib::Storage::StorageFile b("job.txt");
	auto fp = b.Open(RbsLib::Storage::FileIO::OpenMode::Read | RbsLib::Storage::FileIO::OpenMode::Write | RbsLib::Storage::FileIO::OpenMode::Replace,
		RbsLib::Storage::FileIO::SeekBase::begin);
	job = fp.GetLine(12);
	job += 1;
	fp.WriteLine(std::to_string(job));
}

void Generator::Cclass_Generator()
{
	int cclass;
	RbsLib::Storage::StorageFile b("cclass.txt");
	auto fp = b.Open(RbsLib::Storage::FileIO::OpenMode::Read | RbsLib::Storage::FileIO::OpenMode::Write | RbsLib::Storage::FileIO::OpenMode::Replace,
		RbsLib::Storage::FileIO::SeekBase::begin);
	cclass = fp.GetLine(12);
	cclass += 1;
	fp.WriteLine(std::to_string(cclass));
}