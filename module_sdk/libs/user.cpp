#include "user.h"
#include <shared_mutex>
#include <mutex>
#include <cstdlib>
#include <cstring>
#include <fmt/format.h>
#include "database.h"
#include "../../rbslib/String.h"
#include "../../rbslib/Storage.h"
#include "../../rbslib/FileIO.h"
#include "../../json/CJsonObject.h"
#include "../../log/logger.h"
#include "../libs/sqlite_cpp.h"
constexpr auto STUDENT_DIR = "Students";
constexpr auto CLASSES_FILE = "Classes.json";
constexpr auto TEACHER_DIR = "Teachers";
constexpr auto SUBJECT_DIR = "Subjects";
constexpr auto DATABASE_FILE_PATH = "user.db";
//以下锁需要同时加锁多个时按照从上到下的顺序加锁防止死锁
std::shared_mutex Global_Student_Mutex;//读写学生信息文件时加锁
std::shared_mutex Global_Teacher_Mutex;//读写教师信息文件时加锁 
std::shared_mutex Global_Classes_Mutex;//读写班级信息文件时加锁
std::shared_mutex Global_Subjects_Mutex;

static inline void CreateDefaultClassesFile(void)
{
	RbsLib::Storage::FileIO::File fp(CLASSES_FILE, RbsLib::Storage::FileIO::OpenMode::Write |
		RbsLib::Storage::FileIO::OpenMode::Replace, RbsLib::Storage::FileIO::SeekBase::begin);
	fp.Write(RbsLib::Buffer("{}"));
}

static inline void AddClassToTeacher(std::uint64_t id, const std::string& class_name)
{
	using namespace RbsLib::Storage;
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", TEACHER_DIR, id));
	if (file.IsExist() == false) throw Account::AccountException("Teacher not exist");
	auto fp = file.Open(FileIO::OpenMode::Read, FileIO::SeekBase::begin);
	neb::CJsonObject obj;
	if (false == obj.Parse(fp.Read(file.GetFileSize()).ToString()))
		throw Account::AccountException(fmt::format("Parse file {} failed : ", file.Path(), obj.GetErrMsg()));
	if (false == obj["Classes"].Add(class_name))
		throw Account::AccountException(fmt::format("Add class to teacher failed"));
	fp.Close();
	fp = file.Open(FileIO::OpenMode::Write | FileIO::OpenMode::Replace, FileIO::SeekBase::begin);
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
}

static inline void RemoveClassFromTeacher(std::uint64_t id, const std::string& class_name)
{
	using namespace RbsLib::Storage;
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", TEACHER_DIR, id));
	if (file.IsExist() == false) throw Account::AccountException("Teacher not exist");
	auto fp = file.Open(FileIO::OpenMode::Read, FileIO::SeekBase::begin);
	neb::CJsonObject obj;
	if (false == obj.Parse(fp.Read(file.GetFileSize()).ToString()))
		throw Account::AccountException(fmt::format("Parse file {} failed : ", file.Path(), obj.GetErrMsg()));
	std::string str;
	for (int i = 0; i < obj["Classes"].GetArraySize(); ++i)
	{
		if (obj["Classes"](i) == class_name)
		{
			obj["Classes"].Delete(i);
			break;
		}
	}
	fp.Close();
	fp = file.Open(FileIO::OpenMode::Write | FileIO::OpenMode::Replace, FileIO::SeekBase::begin);
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
}

static inline void AddSubjectToStudent(std::uint64_t subject_id, std::uint64_t stu_id)
{
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(".");
	auto fp = file[STUDENT_DIR][std::to_string(stu_id) + ".json"].Open(OpenMode::Read, SeekBase::begin);
	neb::CJsonObject obj(fp.Read(file[STUDENT_DIR][std::to_string(stu_id) + ".json"].GetFileSize()).ToString());
	if (false == obj["Subjects"].Add(subject_id)) throw Account::AccountException("Can not add subject id to student");
	fp.Close();
	fp = file[STUDENT_DIR][std::to_string(stu_id) + ".json"].Open(OpenMode::Write | OpenMode::Replace, SeekBase::begin);
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
}
static inline void RemoveSubjectFromStudent(std::uint64_t subject_id, std::uint64_t stu_id)
{
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(".");
	auto fp = file[STUDENT_DIR][std::to_string(stu_id) + ".json"].Open(OpenMode::Read | OpenMode::Write, SeekBase::begin);
	neb::CJsonObject obj(fp.Read(file[STUDENT_DIR][std::to_string(stu_id) + ".json"].GetFileSize()).ToString());
	for (int i = 0; i < obj["Subjects"].GetArraySize(); ++i)
	{
		std::uint64_t temp = 0;
		if (obj["Subjects"].Get(i, temp) == false)
			throw Account::AccountException("Get number from subjects failed");
		if (temp == subject_id)
		{
			obj["Subjects"].Delete(i);
			break;
		}
	}
	fp.Close();
	fp = file[STUDENT_DIR][std::to_string(stu_id) + ".json"].Open(OpenMode::Write | OpenMode::Replace, SeekBase::begin);
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
}

static inline void AddSubjectToTeacher(std::uint64_t subject_id, std::uint64_t teacher_id)
{
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(".");
	auto fp = file[TEACHER_DIR][std::to_string(teacher_id) + ".json"].Open(OpenMode::Read, SeekBase::begin);
	neb::CJsonObject obj(fp.Read(file[TEACHER_DIR][std::to_string(teacher_id) + ".json"].GetFileSize()).ToString());
	if (false == obj["Subjects"].Add(subject_id)) throw Account::AccountException("Can not add subject id to teacher");
	fp.Close();
	fp = file[TEACHER_DIR][std::to_string(teacher_id) + ".json"].Open(OpenMode::Write | OpenMode::Replace, SeekBase::begin);
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
}

static inline void RemoveSubjectFromTeacher(std::uint64_t subject_id, std::uint64_t teacher_id)
{
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(".");
	auto fp = file[TEACHER_DIR][std::to_string(teacher_id) + ".json"].Open(OpenMode::Read | OpenMode::Write, SeekBase::begin);
	neb::CJsonObject obj(fp.Read(file[TEACHER_DIR][std::to_string(teacher_id) + ".json"].GetFileSize()).ToString());
	for (int i = 0; i < obj["Subjects"].GetArraySize(); ++i)
	{
		std::uint64_t temp = 0;
		if (obj["Subjects"].Get(i, temp) == false)
			throw Account::AccountException("Get number from subjects failed");
		if (temp == subject_id)
		{
			obj["Subjects"].Delete(i);
			break;
		}
	}
	fp.Close();
	fp = file[TEACHER_DIR][std::to_string(teacher_id) + ".json"].Open(OpenMode::Write | OpenMode::Replace, SeekBase::begin);
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
}

void Account::AccountManager::CreateStudent(
	std::uint64_t ID,
	const std::string& name,
	const std::string& phone_number, const std::string& email,
	const std::string& student_sex,
	const std::string& enrollment_date,
	const std::string& pass_word,
	const std::string& college,
	const std::string& class_name,
	const std::string& notes, int permission_level)
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	db.Exec(fmt::format("CREATE TABLE IF NOT EXISTS students {},{},{},{},{},{},{},{},{},{},{};", ID, name, phone_number, student_sex, enrollment_date, pass_word, college, class_name, notes, permission_level));
}

void Account::AccountManager::CreateTeacher(
	std::uint64_t ID,
	const std::string& name,
	const std::string& phone_number, const std::string& email,
	const std::string& teacher_sex,
	const std::string& college,
	const std::string& pass_word,
	const std::string& notes, int permission_level)
{
	
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	db.Exec(fmt::format("CREATE TABLE IF NOT EXISTS teachers {},{},{},{},{},{},{},{},{}", ID, name, phone_number, email, teacher_sex, college, pass_word, notes, permission_level));
}

bool Account::AccountManager::IsStudentExist(std::uint64_t id)
{
	
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec(fmt::format("SELECT ID FROM students WHERE ID = {};",id));
	if (temp["ID"].size() == 0)
	{
		return false;
	}
	else return true;
}

bool Account::AccountManager::IsTeacherExist(std::uint64_t id)
{
	
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec(fmt::format("SELECT ID FROM teachers WHERE ID = {};", id));
	if (temp["ID"].size() == 0)
	{
		return false;
	}
	else return true;
}

void Account::AccountManager::DeleteStudent(std::uint64_t student_id)
{
	
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec(fmt::format("SELECT ID FROM students WHERE ID = {};", student_id));
	if (temp["ID"].size() == 0)
	{
		throw AccountException("This student not in the student's tables");
	}
	else
	{
		db.Exec(fmt::format("DELETE FROM students WHERE ID = {}",student_id));
		db.Exec(fmt::format("DELETE FROM students_subjects_relation WHERE StudentID = {}", student_id));
	}

}

void Account::AccountManager::DeleteTeacher(std::uint64_t teacher_id)
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec(fmt::format("SELECT ID FROM students WHERE ID = {};", teacher_id));
	if (temp["ID"].size() == 0)
	{
		throw AccountException("This teacher not in the teacher's tables");
	}
	else
	{
		db.Exec(fmt::format("DELETE FROM students WHERE ID = {}", teacher_id));
		db.Exec(fmt::format("DELETE FROM teachers_subjects_relation WHERE TeacherID = {}", teacher_id));
	}
}

auto Account::AccountManager::GetStudentInfo(std::uint64_t id)->StudentBasicInfo
{
	std::shared_lock<std::shared_mutex> lock(Global_Student_Mutex);
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", STUDENT_DIR, id));
	RbsLib::Storage::FileIO::File fp = file.Open
	(RbsLib::Storage::FileIO::OpenMode::Read,
		RbsLib::Storage::FileIO::SeekBase::begin);
	auto buffer = fp.Read(file.GetFileSize());
	neb::CJsonObject obj;
	if (false == obj.Parse(buffer.ToString()))
		throw AccountException("Parse student json text failed");
	StudentBasicInfo info;
	info.id = id;
	info.name = obj("Name");
	info.sex = obj("Sex");
	info.email = obj("Email");
	info.phone_number = obj("PhoneNumber");
	info.enrollment_date = obj("EnrollmentDate");
	info.class_name = obj("Class");
	info.college = obj("College");
	info.password = obj("Password");
	obj.Get("PermissionLevel", info.permission_level);
	info.notes = obj("Notes");
	obj.Get("IsEnable", info.is_enable);
	for (int i = 0; i < obj["Subjects"].GetArraySize(); ++i)
	{
		std::uint64_t temp;
		obj["Subjects"].Get(i, temp);
		info.subjects.push_back(temp);
	}
	return info;
}

auto Account::AccountManager::GetTeacherInfo(std::uint64_t id) -> TeacherBasicInfo
{
	std::shared_lock<std::shared_mutex> lock(Global_Teacher_Mutex);
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", TEACHER_DIR, id));
	RbsLib::Storage::FileIO::File fp = file.Open
	(RbsLib::Storage::FileIO::OpenMode::Read,
		RbsLib::Storage::FileIO::SeekBase::begin);
	auto buffer = fp.Read(file.GetFileSize());
	neb::CJsonObject obj;
	if (false == obj.Parse(buffer.ToString()))
		throw AccountException("Parse teacher json text failed");
	TeacherBasicInfo info;
	info.id = id;
	info.name = obj("Name");
	info.sex = obj("Sex");
	info.email = obj("Email");
	info.phone_number = obj("PhoneNumber");
	info.college = obj("College");
	info.password = obj("Password");
	obj.Get("PermissionLevel", info.permission_level);
	info.notes = obj("Notes");
	obj.Get("IsEnable", info.is_enable);
	for (int i = 0; i < obj["Subjects"].GetArraySize(); ++i)
	{
		std::uint64_t temp;
		obj["Subjects"].Get(i, temp);
		info.subjects.push_back(temp);
	}
	for (int i = 0; i < obj["Classes"].GetArraySize(); ++i)
	{
		info.classes.push_back(obj["Classes"](i));
	}
	return info;
}

void Account::AccountManager::SetStudentProperty(const StudentBasicInfo& info)
{
	std::shared_lock<std::shared_mutex> lock(Global_Student_Mutex);
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", STUDENT_DIR, info.id));
	neb::CJsonObject obj(file.Open(RbsLib::Storage::FileIO::OpenMode::Read, RbsLib::Storage::FileIO::SeekBase::begin).Read(file.GetFileSize()).ToString());
	obj.ReplaceAdd("Name", info.name);
	obj.ReplaceAdd("PhoneNumber", info.phone_number);
	obj.ReplaceAdd("Email", info.email);
	obj.ReplaceAdd("Sex", info.sex);
	obj.ReplaceAdd("College", info.college);
	obj.ReplaceAdd("EnrollmentDate", info.enrollment_date);
	obj.ReplaceAdd("Password", info.password);
	obj.ReplaceAdd("PermissionLevel", info.permission_level);
	obj.ReplaceAdd("Notes", info.notes);
	obj.ReplaceAdd("IsEnable", info.is_enable);
	file.Open(RbsLib::Storage::FileIO::OpenMode::Write | RbsLib::Storage::FileIO::OpenMode::Replace, RbsLib::Storage::FileIO::SeekBase::begin)
		.Write(RbsLib::Buffer(obj.ToFormattedString()));
}

void Account::AccountManager::SetTeacherProperty(const TeacherBasicInfo& info)
{
	std::shared_lock<std::shared_mutex> lock(Global_Teacher_Mutex);
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", TEACHER_DIR, info.id));
	neb::CJsonObject obj(file.Open(RbsLib::Storage::FileIO::OpenMode::Read, RbsLib::Storage::FileIO::SeekBase::begin).Read(file.GetFileSize()).ToString());
	obj.ReplaceAdd("Name", info.name);
	obj.ReplaceAdd("PhoneNumber", info.phone_number);
	obj.ReplaceAdd("Email", info.email);
	obj.ReplaceAdd("Sex", info.sex);
	obj.ReplaceAdd("College", info.college);
	obj.ReplaceAdd("Password", info.password);
	obj.ReplaceAdd("PermissionLevel", info.permission_level);
	obj.ReplaceAdd("Notes", info.notes);
	obj.ReplaceAdd("IsEnable", info.is_enable);
	file.Open(RbsLib::Storage::FileIO::OpenMode::Write | RbsLib::Storage::FileIO::OpenMode::Replace, RbsLib::Storage::FileIO::SeekBase::begin)
		.Write(RbsLib::Buffer(obj.ToFormattedString()));
}

auto Account::AccountManager::GetAllStudentInfo(void) -> std::vector<StudentBasicInfo>
{
	std::shared_lock<std::shared_mutex> lock(Global_Student_Mutex);
	std::vector<StudentBasicInfo> info_list;
	RbsLib::Storage::StorageFile dir(STUDENT_DIR);
	if (dir.IsExist() && dir.GetFileType() == RbsLib::Storage::FileType::FileType::Dir)
	{
		for (const auto& it : dir)
		{
			if (it.GetExtension() == ".json" && it.GetFileType() == RbsLib::Storage::FileType::FileType::Regular)
			{
				info_list.push_back(GetStudentInfo(RbsLib::String::Convert::StringToNumber<std::uint64_t>(it.GetStem())));
			}
		}
	}
	return info_list;
}

auto Account::AccountManager::GetAllTeacherInfo(void) -> std::vector<TeacherBasicInfo>
{
	std::shared_lock<std::shared_mutex> lock(Global_Teacher_Mutex);
	std::vector<TeacherBasicInfo> info_list;
	RbsLib::Storage::StorageFile dir(TEACHER_DIR);
	if (dir.IsExist() && dir.GetFileType() == RbsLib::Storage::FileType::FileType::Dir)
	{
		for (const auto& it : dir)
		{
			if (it.GetExtension() == ".json" && it.GetFileType() == RbsLib::Storage::FileType::FileType::Regular)
			{
				info_list.push_back(GetTeacherInfo(RbsLib::String::Convert::StringToNumber<std::uint64_t>(it.GetStem())));
			}
		}
	}
	return info_list;
}

void Account::AccountManager::ChangeStudentClass(std::uint64_t id, const std::string& new_class_name)
{
	std::unique_lock<std::shared_mutex> lock1(Global_Student_Mutex);
	std::unique_lock<std::shared_mutex> lock2(Global_Classes_Mutex);
	RbsLib::Storage::StorageFile file = CLASSES_FILE;
	if (file.IsExist() == false)
	{
		throw AccountException("class not exist");
	}
	auto fp=file.Open(RbsLib::Storage::FileIO::OpenMode::Read, RbsLib::Storage::FileIO::SeekBase::begin);
	neb::CJsonObject obj;
	obj.Parse(fp.Read(file.GetFileSize()).ToString());
	if (obj.KeyExist(new_class_name) == false)
	{
		throw AccountException("class not exist");
	}
	obj[new_class_name]["Students"].Add(id);
	RbsLib::Storage::StorageFile student_file = fmt::format("{}/{}.json", STUDENT_DIR, id);
	if (student_file.IsExist() == false)
	{
		throw AccountException("student not exist");
	}
	fp = student_file.Open(RbsLib::Storage::FileIO::OpenMode::Read, RbsLib::Storage::FileIO::SeekBase::begin);
	neb::CJsonObject stu_obj;
	stu_obj.Parse(fp.Read(student_file.GetFileSize()).ToString());
	obj[stu_obj("Class")]["Students"].GetArraySize();
	for (int i = 0; i < obj[stu_obj("Class")]["Students"].GetArraySize(); i++)
	{
		std::uint64_t tmp;
		obj[stu_obj("Class")]["Students"].Get(i, tmp);
		if (tmp == id)
			obj[stu_obj("Class")]["Students"].Delete(i);
	}
	stu_obj.Replace("Class", new_class_name);
	fp = file.Open(RbsLib::Storage::FileIO::OpenMode::Write | RbsLib::Storage::FileIO::OpenMode::Replace,
		RbsLib::Storage::FileIO::SeekBase::begin);
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
	fp = student_file.Open(RbsLib::Storage::FileIO::OpenMode::Write | RbsLib::Storage::FileIO::OpenMode::Replace, RbsLib::Storage::FileIO::SeekBase::begin);
	fp.Write(RbsLib::Buffer(stu_obj.ToFormattedString()));
}
void Account::ClassesManager::CreateClass(const std::string& class_name, std::uint64_t teacherID)
{
	//将班级信息存储在Classes.json文件中
	std::unique_lock<std::shared_mutex> teacher_lock(Global_Teacher_Mutex);
	std::unique_lock<std::shared_mutex> lock(Global_Classes_Mutex);
	RbsLib::Storage::StorageFile classes_file(CLASSES_FILE);
	if (classes_file.IsExist() == false) CreateDefaultClassesFile();
	auto class_fp = classes_file.Open(RbsLib::Storage::FileIO::OpenMode::Read,
		RbsLib::Storage::FileIO::SeekBase::begin);
	auto buffer = class_fp.Read(classes_file.GetFileSize());
	neb::CJsonObject class_json;
	if (class_json.Parse(buffer.ToString()) == false)
	{
		Logger::LogError("配置文件%s解析失败，原因: %s", CLASSES_FILE, class_json.GetErrMsg());
		std::abort();
	}
	//检查课程是否已经存在
	if (class_json.KeyExist(class_name))
		throw AccountException("Class is already exist");
	//向老师文件添加
	AddClassToTeacher(teacherID, class_name);
	//保存课程信息
	class_fp.Close();
	neb::CJsonObject subjson;
	subjson.Add("TeacherID", teacherID);
	subjson.Add("CreateTime", Time::GetFormattedTime());
	subjson.AddEmptySubArray("Students");
	class_json.Add(class_name, subjson);
	class_fp = classes_file.Open(RbsLib::Storage::FileIO::OpenMode::Write | RbsLib::Storage::FileIO::OpenMode::Replace,
		RbsLib::Storage::FileIO::SeekBase::begin);
	class_fp.Write(RbsLib::Buffer(class_json.ToFormattedString()));
}

void Account::ClassesManager::DeleteClass(const std::string& class_name)
{
	//若班级中不存在学生，则删除班级及其相关信息，若存在学生则抛出异常
	std::unique_lock<std::shared_mutex> teacher_lock(Global_Teacher_Mutex);
	std::unique_lock<std::shared_mutex> lock(Global_Classes_Mutex);
	using namespace RbsLib::Storage;
	StorageFile file(CLASSES_FILE);
	if (file.IsExist() == false) throw AccountException("Class not found");
	neb::CJsonObject tobj(file.Open(FileIO::OpenMode::Read, FileIO::SeekBase::begin).Read(file.GetFileSize()).ToString());
	if (!tobj.KeyExist(class_name))
		throw AccountException("Class not found");
	if (tobj[class_name]["Students"].GetArraySize() > 0) throw AccountException("Can not delete class with students");
	std::uint64_t teacher_id = RbsLib::String::Convert::StringToNumber<std::uint64_t>(tobj[class_name]("TeacherID"));
	tobj.Delete(class_name);
	file.Open(FileIO::OpenMode::Write | FileIO::OpenMode::Replace, FileIO::SeekBase::begin).Write(RbsLib::Buffer(tobj.ToFormattedString()));
	//从教师文件中删除
	RemoveClassFromTeacher(teacher_id, class_name);
}

bool Account::ClassesManager::IsClassExist(const std::string& class_name)
{
	std::shared_lock<std::shared_mutex> lock(Global_Classes_Mutex);
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(CLASSES_FILE);
	if (file.IsExist())
	{
		auto fp = file.Open(OpenMode::Read, SeekBase::begin);
		if (neb::CJsonObject(fp.Read(file.GetFileSize()).ToString()).KeyExist(class_name))
			return true;
	}
	return false;
}

auto Account::ClassesManager::GetClassInfo(const std::string& class_name) -> ClassInfo
{
	std::shared_lock<std::shared_mutex> lock(Global_Classes_Mutex);
	ClassInfo ret;
	using namespace RbsLib::Storage;
	StorageFile file(CLASSES_FILE);
	if (file.IsExist() == false) throw AccountException("Class not found");
	neb::CJsonObject tobj(file.Open(FileIO::OpenMode::Read, FileIO::SeekBase::begin).Read(file.GetFileSize()).ToString());
	if (!tobj.KeyExist(class_name))
		throw AccountException("Class not found");
	ret.name = class_name;
	neb::CJsonObject obj;
	tobj.Get(class_name, obj);
	obj.Get("TeacherID", ret.teacher_id);
	obj.Get("CreateTime", ret.create_time);
	for (int i = 0; i < obj["Students"].GetArraySize(); ++i)
	{
		std::uint64_t temp;
		obj["Students"].Get(i, temp);
		ret.students.push_back(temp);
	}
	return ret;
}

auto Account::ClassesManager::GetAllClassInfo(void) -> std::vector<ClassInfo>
{
	std::shared_lock<std::shared_mutex> lock(Global_Classes_Mutex);
	std::vector<ClassInfo> ret;
	using namespace RbsLib::Storage;
	StorageFile file(CLASSES_FILE);
	if (file.IsExist() == false) throw AccountException("Class not found");
	neb::CJsonObject obj(file.Open(FileIO::OpenMode::Read, FileIO::SeekBase::begin).Read(file.GetFileSize()).ToString());
	std::string class_name;
	while (obj.GetKey(class_name))
	{
		ret.push_back(GetClassInfo(class_name));
	}
	return ret;
}


void Account::SubjectManager::CreateSubject(std::uint64_t subject_id, const std::string& subject_name, int begin_year, int end_year, int semester, const std::string& classroom, const std::string& description)
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp_semester = fmt::format("{}-{}:{}", begin_year, end_year, semester);
	try
	{
		db.Exec(fmt::format("INSERT INTO subjects (Subject_ID,Subject_Name,Semeseter,Classroom,Desctiption) VALUES ({},'{}','{}','{}','{}')", subject_id, subject_name, temp_semester, classroom, description));
	}
	catch (const DataBase::DataBaseException& ex)
	{
		throw AccountException(std::string("Create subject failed: ") + ex.what());
	}
}

void Account::SubjectManager::AddStudent(std::uint64_t student_id, std::uint64_t subject_id, const std::string& notes)
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	db.Exec(fmt::format(" INSERT INTO students_subjects_relation(StudentID,SubjectID,Grade,Notes) VALUES ({},{},{},'{}')", student_id, subject_id, '-1', notes));
}

void Account::SubjectManager::AddTeacher(std::uint64_t teacher_id, std::uint64_t subject_id)
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	db.Exec(fmt::format("INSERT INTO teachers_subjects_relation(TeacherID,SubjectID,Notes) VALUES ({},{},'{}')",teacher_id,subject_id));
}

void Account::SubjectManager::RemoveStudent(std::uint64_t student_id, std::uint64_t subject_id)
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	db.Exec(fmt::format(" DELETE FROM students_subjects_relation WHERE StudentID = {} and SubjectID = {};", student_id, subject_id));
}

void Account::SubjectManager::RemoveTeacher(std::uint64_t teacher_id, std::uint64_t subject_id)
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	db.Exec(fmt::format("DELETE FROM teachers_subjects_relation WHERE TeacherID = {} and SubjectID = {};", teacher_id, subject_id));
}

void Account::SubjectManager::DeleteSubject(std::uint64_t subject_id)
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	db.Exec(fmt::format("DELETE FROM students_subjects_relation WHERE SubjectID = {}",subject_id));
	db.Exec(fmt::format("DELETE FROM teachers_subjects_relation WHERE SubjectID = {}", subject_id));
	db.Exec(fmt::format("DELETE FROM subjects WHERE ID = {}", subject_id));
}

bool Account::SubjectManager::IsSubjectExist(std::uint64_t subject_id)
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec(fmt::format("SELECT ID FROM subjects WHERE ID = {}", subject_id));
	if (temp["ID"].size())
	{
		return true;
	}
	else return false;
}

Account::SubjectInfo Account::SubjectManager::GetSubjectInfo(std::uint64_t subject_id)
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec(fmt::format(" SELECT * FROM subjects WHERE ID = {}", subject_id));
	auto temp2 = db.Exec(fmt::format(" SELECT * FROM teachers_subjects_relation WHERE SubjectID = {}", subject_id));
	auto temp3 = db.Exec(fmt::format(" SELECT * FROM classes WHERE TeacherID = {}", temp2["TeacherID"]));
	auto temp4 = db.Exec(fmt::format(" SELECT * FROM students_subjects_relation WHERE StudentID = {}", subject_id));
	if (temp["ID"].size())
	{
		throw AccountException("This subject isn't exict");
	}
	else
	{
		SubjectInfo a;
		a.classroom = temp3["ClassName"][0];
		a.description = temp["Description"][0];
		a.id = RbsLib::String::Convert::StringToNumber<std::uint64_t>(temp["ID"][0]);
		a.name = temp["Name"][0];
		a.semester = RbsLib::String::Convert::StringToNumber<std::uint64_t>(temp["semester"][0]);
		for (const auto& it:temp4["StudentID"])
		{
			a.students.push_back(RbsLib::String::Convert::StringToNumber<std::uint64_t>(it));
		}
		for (const auto& it : temp2["TeacherID"])
		{
			a.teachers.push_back(RbsLib::String::Convert::StringToNumber<std::uint64_t>(it));
		}
	}
}

auto Account::SubjectManager::GetAllSubjectInfo(void) -> std::vector<SubjectInfo>
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec("SELECT * FROM subjects");
	auto temp2 = db.Exec("SELECT ID FROM students");
	auto temp3 = db.Exec("SELIECT * FROM teachers_subjects_relation");
	auto temp4 = db.Exec("SELECT * FROM classes");
	auto temp5 = db.Exec("SELIECT * FROM students_subjects_relation");
	if (temp["ID"].size() == 0)
	{
		throw AccountException("This subjects' tables is null");

	}
	else
	{
		std::vector<SubjectInfo> ret;
		std::uint64_t n = temp["Name"].size();
		for (std::uint64_t i = 0; i < n; i++)
		{
			SubjectInfo a;
			a.classroom = temp4["ClassName"][0];
			a.description = temp["Description"][0];
			a.id = RbsLib::String::Convert::StringToNumber<std::uint64_t>(temp["ID"][0]);
			a.name = temp["Name"][0];
			a.semester = RbsLib::String::Convert::StringToNumber<std::uint64_t>(temp["Semester"][0]);
			for (const auto& it : temp5["StudentID"])
			{
				a.students.push_back(RbsLib::String::Convert::StringToNumber<std::uint64_t>(it));
			}
			for (const auto& it : temp3["TeachersID"])
			{
				a.teachers.push_back(RbsLib::String::Convert::StringToNumber<std::uint64_t>(it));
			}
		}
	}
}

void Account::SubjectManager::SetStudentProperty(std::uint64_t subject_id, const SubjectInfo::Student& info)
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec(fmt::format("SELECT * From students_subjects_relation WHERE SubjectID = {} and StudentID = {}",subject_id,info.id));
	if (temp["SubjectID"].size() == 0)
	{
		throw AccountException("These students not in students' tables");
	}
	else
	{
		db.Exec(fmt::format("UPDATE students SET Grade = {} WHERE ID = {}",info.grade,info.id));
		db.Exec(fmt::format("UPDATE students SET Notes = {} WHERE ID = {}", info.notes, info.id));
	}
}

bool Account::SubjectManager::IsStudentInSubject(std::uint64_t student_id, std::uint64_t subject_id)
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec(fmt::format("SELECT * from students_subjects_relation WHERE StudentID = {} and SubjectID = {};", student_id, subject_id));
	if (temp["ID"].size())
	{
		return true;
	}
	else return false;
}

Account::AccountException::AccountException(const std::string& reason) noexcept
	:reason(reason)
{
}

const char* Account::AccountException::what(void) const noexcept
{
	return this->reason.c_str();
}
