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
	db.Exec(fmt::format("INSERT INTO students ('ID','Name','PhoneNumber','Email','Sex','EnrollmentDate','Password','CollegeName','ClassName','Notes','PermissionLevel') VALUES ({},'{}','{}','{}','{}','{}','{}','{}','{}','{}',{});", ID, name, phone_number, email, student_sex, enrollment_date, pass_word, college, class_name, notes, permission_level));
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
	db.Exec(fmt::format("INSERT INTO teachers('ID','Name','PhoneNumber','Email','Sex','CollegeName','Password','Notes','PermissionLevel') VALUES ({},'{}','{}','{}','{}','{}','{}','{}',{});", ID, name, phone_number, email, teacher_sex, college, pass_word, notes, permission_level));
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
		db.Exec(fmt::format("DELETE FROM students_subjects_relation WHERE StudentID = {}", student_id));
		db.Exec(fmt::format("DELETE FROM students WHERE ID = {}",student_id));
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
		db.Exec(fmt::format("DELETE FROM teachers_subjects_relation WHERE TeacherID = {}", teacher_id));
		db.Exec(fmt::format("DELETE FROM students WHERE ID = {}", teacher_id));
	}
}

auto Account::AccountManager::GetStudentInfo(std::uint64_t id)->StudentBasicInfo
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec(fmt::format("SELECT * FROM students WHERE ID = {};",id));
	auto temp2 = db.Exec(fmt::format("SELECT SubjectID FROM students_subjects_relation WHERE StudentID = {};", id));
	if (temp["ID"].size() == 0)
	{
		throw AccountException("These students not in the students' tables");
	}
	if (temp2["SubjectID"].size() == 0)
	{
		throw AccountException("These subject not is not exist");
	}
		StudentBasicInfo a;
		a.id = RbsLib::String::Convert::StringToNumber<std::uint64_t>(temp["ID"][0]);
		a.name = temp["Name"][0];
		a.phone_number = temp["PhoneNumber"][0];
		a.sex = temp["Sex"][0];
		a.enrollment_date = temp["PermissionLevel"][0];
		a.notes = temp["Notes"][0];
		a.college = temp["CollegeName"][0];
		a.class_name = temp["ClassName"][0];
		a.password = temp["Password"][0];
		a.permission_level = RbsLib::String::Convert::StringToNumber<std::uint64_t>(temp["PermissionLevel"][0]);
		a.is_enable = RbsLib::String::Convert::StringToNumber<std::uint64_t>(temp["IsEnable"][0]);
		a.email = temp["Email"][0];
		for (const auto& it : temp2["SubjectID"])
			{
				a.subjects.push_back(RbsLib::String::Convert::StringToNumber<std::uint64_t>(it));
			}
		return a;
}

auto Account::AccountManager::GetTeacherInfo(std::uint64_t id) -> TeacherBasicInfo
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec(fmt::format("SELECT * FROM teachers WHERE ID = {};", id));
	auto temp2 = db.Exec(fmt::format("SELECT ClassName FROM classes WHERE TeacherID = {};", id));
	auto temp3 = db.Exec(fmt::format("SELECT SubjectID FROM teachers_subjects_relation WHERE TeacherID = {};", id));
	if (temp["ID"].size() == 0)
	{
		throw AccountException("These teachers not in the teachers' tables");
	}
		TeacherBasicInfo a;
		a.id = RbsLib::String::Convert::StringToNumber<std::uint64_t>(temp["ID"][0]);
		a.name = temp["Name"][0];
		a.phone_number = temp["PhoneNumber"][0];
		a.sex = temp["Sex"][0];
		a.notes = temp["Notes"][0];
		a.college = temp["CollegeName"][0];
		a.password = temp["Password"][0];
		a.permission_level = RbsLib::String::Convert::StringToNumber<std::uint64_t>(temp["PermissionLevel"][0]);
		a.is_enable = RbsLib::String::Convert::StringToNumber<std::uint64_t>(temp["IsEnable"][0]);
		a.email = temp["Email"][0];
		for (const auto& it : temp2["ClassName"])
		{
			a.classes.push_back(it);
		}
		for (const auto& it : temp3["SubjectID"])
		{
			a.subjects.push_back(RbsLib::String::Convert::StringToNumber<std::uint64_t>(it));
		}
		return a;
}

void Account::AccountManager::SetStudentProperty(const StudentBasicInfo& info)
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec(fmt::format("SELECT * FROM students WHERE ID = {};", info.id));
	if (temp["ID"].size() == 0)
	{
		throw AccountException("These students not in the students' tables");
	}
	else
	{
		db.Exec(fmt::format("UPDATE students SET Name = '{}' WHERE ID = {};", info.name, info.id));
		db.Exec(fmt::format("UPDATE students SET PhoneNumber = '{}' WHERE ID = {};",info.phone_number , info.id));
		db.Exec(fmt::format("UPDATE students SET Sex = '{}' WHERE ID = {};",info.sex , info.id));
		db.Exec(fmt::format("UPDATE students SET EnrollmentDate = '{}' WHERE ID = {};", info.enrollment_date, info.id));
		db.Exec(fmt::format("UPDATE students SET CollegeName = '{}' WHERE ID = {};", info.college, info.id));
		db.Exec(fmt::format("UPDATE students SET Password = '{}' WHERE ID = {};", info.password, info.id));
		db.Exec(fmt::format("UPDATE students SET PermissionLevel = {} WHERE ID = {};", info.permission_level, info.id));
		db.Exec(fmt::format("UPDATE students SET Notes = '{}' WHERE ID = {};", info.notes, info.id));
		db.Exec(fmt::format("UPDATE students SET IsEnable = {} WHERE ID = {};", info.is_enable, info.id));
		db.Exec(fmt::format("UPDATE students SET Email = '{}' WHERE ID = {};", info.email, info.id));
	}
}

void Account::AccountManager::SetTeacherProperty(const TeacherBasicInfo& info)
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec(fmt::format("SELECT * FROM teachers WHERE ID = {};", info.id));
	if (temp["ID"].size() == 0)
	{
		throw AccountException("These teacher not in the teachers' tables");
	}
	else
	{
		db.Exec(fmt::format("UPDATE teachers SET Name = '{}' WHERE ID = {};", info.name, info.id));
		db.Exec(fmt::format("UPDATE teachers SET PhoneNumber = '{}' WHERE ID = {};", info.phone_number, info.id));
		db.Exec(fmt::format("UPDATE teachers SET Sex = '{}' WHERE ID = {};", info.sex, info.id));
		db.Exec(fmt::format("UPDATE teachers SET CollegeName = '{}' WHERE ID = {};", info.college, info.id));
		db.Exec(fmt::format("UPDATE teachers SET Password = '{}' WHERE ID = {};", info.password, info.id));
		db.Exec(fmt::format("UPDATE teachers SET PermissionLevel = {} WHERE ID = {};", info.permission_level, info.id));
		db.Exec(fmt::format("UPDATE teachers SET Notes = '{}' WHERE ID = {};", info.notes, info.id));
		db.Exec(fmt::format("UPDATE teachers SET IsEnable = {} WHERE ID = {};", info.is_enable, info.id));
		db.Exec(fmt::format("UPDATE teachers SET Email = '{}' WHERE ID = {};", info.email, info.id));
	}
}

auto Account::AccountManager::GetAllStudentInfo(void) -> std::vector<StudentBasicInfo>
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec("SELECT * FROM students ");
	if (temp["ID"].size() == 0)
	{
		throw AccountException("This students' tables is null");
	}
	else
	{

		std::vector<StudentBasicInfo> ret;
		std::size_t n = temp["ID"].size();
		for (std::size_t i = 0; i < n; i++)
		{
			StudentBasicInfo a;
			a.id = RbsLib::String::Convert::StringToNumber<std::uint64_t>(temp["ID"][i]);
			a.name = temp["Name"][i];
			a.phone_number = temp["PhoneNumber"][i];
			a.sex = temp["Sex"][i];
			a.enrollment_date = temp["PermissionLevel"][i];
			a.notes = temp["Notes"][i];
			a.college = temp["CollegeName"][i];
			a.class_name = temp["ClassName"][i];
			a.password = temp["Password"][i];
			a.permission_level = RbsLib::String::Convert::StringToNumber<std::uint64_t>(temp["PermissionLevel"][i]);
			a.is_enable = RbsLib::String::Convert::StringToNumber<std::uint64_t>(temp["IsEnable"][i]);
			a.email = temp["Email"][i];
			ret.push_back(a);
		}
		return ret;
	}
}

auto Account::AccountManager::GetAllTeacherInfo(void) -> std::vector<TeacherBasicInfo>
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec("SELECT * FROM teachers;");
	
		std::vector<TeacherBasicInfo> ret;
		std::size_t n = temp["ID"].size();
		for (std::size_t i = 0; i < n; i++)
		{
			TeacherBasicInfo a;
			a.id = RbsLib::String::Convert::StringToNumber<std::uint64_t>(temp["ID"][i]);
			a.name = temp["Name"][i];
			a.phone_number = temp["PhoneNumber"][i];
			a.sex = temp["Sex"][i];
			a.notes = temp["Notes"][i];
			a.college = temp["CollegeName"][i];
			a.password = temp["Password"][i];
			a.permission_level = RbsLib::String::Convert::StringToNumber<std::uint64_t>(temp["PermissionLevel"][i]);
			a.is_enable = RbsLib::String::Convert::StringToNumber<std::uint64_t>(temp["IsEnable"][i]);
			a.email = temp["Email"][i];
			ret.push_back(a);
		}
		return ret;
	
}

void Account::AccountManager::ChangeStudentClass(std::uint64_t id, const std::string& new_class_name)
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec(fmt::format("SELECT * FROM students WHERE ID = {};", id));
	if (temp["ID"].size() == 0)
	{
		throw AccountException("These students not in the students' tables");
	}
	else
	{
		db.Exec(fmt::format("UPDATE students SET ClassName = '{}' WHERE ID = {};", new_class_name, id));
	}
}
void Account::ClassesManager::CreateClass(const std::string& class_name, std::uint64_t teacherID)
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	db.Exec(fmt::format("INSERT INTO classes ('ClassName','TeacherID') VALUES('{}',{});", class_name, teacherID));
}

void Account::ClassesManager::DeleteClass(const std::string& class_name)
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec(fmt::format("SELECT ID FROM students WHERE ClassName = '{}';", class_name));
	if (temp["ClassName"].size() == 0)
	{
		try
		{
			db.Exec(fmt::format("DELETE FROM classes WHERE ClassName = {}", class_name));
		}
		catch (const DataBase::DataBaseException& ex)
		{
			throw AccountException(std::string("Delete class failed : ") + ex.what());
		}
	}
	else
	{
		throw AccountException("This class  in the students' tables");
	}
}

bool Account::ClassesManager::IsClassExist(const std::string& class_name)
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec(fmt::format("SELECT ClassName FROM classes WHERE ClassName = {};", class_name));
	if (temp["ClassName"].size() == 0)
	{
		return false;
	}
	else return true;
}

auto Account::ClassesManager::GetClassInfo(const std::string& class_name) -> ClassInfo
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec(fmt::format("SELECT * FROM classes WHERE ClassName = '{}';", class_name));
	auto temp2 = db.Exec(fmt::format("SELECT ID FROM students WHERE ClassName = '{}';", class_name));
	if (temp["ClassName"].size() == 0)
	{
		throw AccountException("These class not in the classes' tables");
	}
		ClassInfo a;
		a.create_time = temp["CreateTime"][0];
		a.name = temp["Name"][0];
		a.teacher_id = RbsLib::String::Convert::StringToNumber<std::uint64_t>(temp["TeacherID"][0]);
		for (const auto&it:temp2["ID"])
		{
			a.students.push_back(RbsLib::String::Convert::StringToNumber<std::uint64_t>(it));
		}
	return a;
}

auto Account::ClassesManager::GetAllClassInfo(void) -> std::vector<ClassInfo>
{
	auto db = DataBase::SQLite::Open(DATABASE_FILE_PATH);
	auto temp = db.Exec("SELECT * FROM classes ");
	auto temp2 = db.Exec("SELECT ID FROM students ");
	if (temp["ClassName"].size() == 0)
	{
		throw AccountException("This class' tables is null");
	}
	else
	{
		std::vector<ClassInfo> ret;
		std::size_t n = temp["ClassName"].size();
		for (std::size_t i = 0; i < n; i++)
		{
			ClassInfo a;
			a.create_time = temp["CreateTime"][0];
			a.name = temp["Name"][0];
			a.teacher_id = RbsLib::String::Convert::StringToNumber<std::uint64_t>(temp["TeacherID"][0]);
			for (const auto& it : temp2["ID"])
			{
				a.students.push_back(RbsLib::String::Convert::StringToNumber<std::uint64_t>(it));
			}
			ret.push_back(a);
		}
		return ret;
	}
}


void Account::SubjectManager::CreateSubject(std::uint64_t subject_id, const std::string& subject_name, int begin_year, int end_year, int semester, const std::string& classroom, const std::string& description)
{
	using namespace RbsLib::Storage;
	//创建课程，放在Subjects目录中，以课程编号命名，如1000000.json
	std::unique_lock<std::shared_mutex> lock(Global_Subjects_Mutex);
	//检查目录是否存在
	if (StorageFile(SUBJECT_DIR).IsExist() == false)
		StorageFile(".").CreateDir(SUBJECT_DIR);
	//检查课程是否已经存在
	StorageFile file(fmt::format("{}/{}.json", SUBJECT_DIR, subject_id));
	if (file.IsExist()) throw AccountException("Subject is already exist");
	neb::CJsonObject object;
	object.Add("SubjectName", subject_name);
	object.Add("Semester", fmt::format("{}-{}:{}", begin_year, end_year, semester));
	object.Add("Description", description);
	object.Add("Classroom", classroom);
	object.AddEmptySubArray("TeacherID");
	object.AddEmptySubArray("StudentsID");
	auto fp = file.Open(FileIO::OpenMode::Write | FileIO::OpenMode::Replace, FileIO::SeekBase::begin);
	fp.Write(RbsLib::Buffer(object.ToFormattedString()));
}

void Account::SubjectManager::AddStudent(std::uint64_t student_id, std::uint64_t subject_id, const std::string& notes)
{
	//向课程添加学生
	std::unique_lock<std::shared_mutex> lock(Global_Student_Mutex);
	std::unique_lock<std::shared_mutex> lock2(Global_Subjects_Mutex);
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", SUBJECT_DIR, subject_id));
	auto fp = file.Open(OpenMode::Read, SeekBase::begin);
	neb::CJsonObject json(fp.Read(file.GetFileSize()).ToString());
	for (int i = 0; i < json["StudentsID"].GetArraySize(); ++i)
	{
		std::uint64_t temp = 0;
		json["StudentsID"][i].Get("ID", temp);
		if (temp == student_id) throw AccountException("Student is already in the subject");
	}
	neb::CJsonObject sub;
	sub.Add("ID", student_id);
	sub.Add("Grade", -1);
	sub.Add("Notes", notes);
	if (json["StudentsID"].Add(sub) == false)
		throw AccountException("Add student to subject failed");
	AddSubjectToStudent(subject_id, student_id);
	fp.Close();
	fp = file.Open(OpenMode::Write | OpenMode::Replace, SeekBase::begin);
	fp.Write(RbsLib::Buffer(json.ToFormattedString()));
}

void Account::SubjectManager::AddTeacher(std::uint64_t teacher_id, std::uint64_t subject_id)
{
	//向课程添加老师
	std::unique_lock<std::shared_mutex> lock(Global_Teacher_Mutex);
	std::unique_lock<std::shared_mutex> lock2(Global_Subjects_Mutex);
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", SUBJECT_DIR, subject_id));
	auto fp = file.Open(OpenMode::Read, SeekBase::begin);
	neb::CJsonObject json(fp.Read(file.GetFileSize()).ToString());
	for (int i = 0; i < json["TeacherID"].GetArraySize(); ++i)
	{
		std::uint64_t temp = 0;
		json["TeacherID"].Get(i, temp);
		if (temp == teacher_id) throw AccountException("Teacher is already in the subject");
	}
	if (json["TeacherID"].Add(teacher_id) == false)
		throw AccountException("Add teacher to subject failed");
	AddSubjectToTeacher(subject_id, teacher_id);
	fp.Close();
	fp = file.Open(OpenMode::Write | OpenMode::Replace, SeekBase::begin);
	fp.Write(RbsLib::Buffer(json.ToFormattedString()));
}

void Account::SubjectManager::RemoveStudent(std::uint64_t student_id, std::uint64_t subject_id)
{
	//从课程移除学生
	std::unique_lock<std::shared_mutex> lock(Global_Student_Mutex);
	std::unique_lock<std::shared_mutex> lock2(Global_Subjects_Mutex);
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", SUBJECT_DIR, subject_id));
	auto fp = file.Open(OpenMode::Read, SeekBase::begin);
	neb::CJsonObject json(fp.Read(file.GetFileSize()).ToString());
	for (int i = 0; i < json["StudentsID"].GetArraySize(); ++i)
	{
		std::uint64_t temp = 0;
		if (json["StudentsID"][i].Get("ID", temp) == false)
			throw AccountException("Read students list from subject failed");
		if (temp == student_id)
		{
			json["StudentsID"].Delete(i);
			break;
		}
	}
	RemoveSubjectFromStudent(subject_id, student_id);
	fp.Close();
	fp = file.Open(OpenMode::Write | OpenMode::Replace, SeekBase::begin);
	fp.Write(RbsLib::Buffer(json.ToFormattedString()));
}

void Account::SubjectManager::RemoveTeacher(std::uint64_t teacher_id, std::uint64_t subject_id)
{
	//从课程移除老师
	std::unique_lock<std::shared_mutex> lock(Global_Teacher_Mutex);
	std::unique_lock<std::shared_mutex> lock2(Global_Subjects_Mutex);
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", SUBJECT_DIR, subject_id));
	auto fp = file.Open(OpenMode::Read, SeekBase::begin);
	neb::CJsonObject json(fp.Read(file.GetFileSize()).ToString());
	for (int i = 0; i < json["TeacherID"].GetArraySize(); ++i)
	{
		std::uint64_t temp = 0;
		if (json["TeacherID"].Get(i, temp) == false)
			throw AccountException("Read teacher list from subject failed");
		if (temp == teacher_id) json["TeacherID"].Delete(i);
	}
	RemoveSubjectFromTeacher(subject_id, teacher_id);
	fp.Close();
	fp = file.Open(OpenMode::Write | OpenMode::Replace, SeekBase::begin);
	fp.Write(RbsLib::Buffer(json.ToFormattedString()));
}

void Account::SubjectManager::DeleteSubject(std::uint64_t subject_id)
{
	//删除课程，只允许删除没有老师没有学生的课程，否则抛出异常
	std::unique_lock<std::shared_mutex> lock2(Global_Subjects_Mutex);
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", SUBJECT_DIR, subject_id));
	auto fp = file.Open(OpenMode::Read, SeekBase::begin);
	neb::CJsonObject json(fp.Read(file.GetFileSize()).ToString());

	if (json["TeacherID"].GetArraySize() > 0 || json["StudentsID"].GetArraySize() > 0)
		throw AccountException("Can not delete subject with teacher or student");
	fp.Close();
	file.Remove();
}

bool Account::SubjectManager::IsSubjectExist(std::uint64_t subject_id)
{
	std::shared_lock<std::shared_mutex> lock(Global_Subjects_Mutex);
	if (RbsLib::Storage::StorageFile(SUBJECT_DIR).IsExist())
		if (RbsLib::Storage::StorageFile(SUBJECT_DIR)[std::to_string(subject_id) + ".json"].IsExist())
			return true;
	return false;
}

Account::SubjectInfo Account::SubjectManager::GetSubjectInfo(std::uint64_t subject_id)
{
	std::shared_lock<std::shared_mutex> lock(Global_Subjects_Mutex);
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", SUBJECT_DIR, subject_id));
	auto fp = file.Open(RbsLib::Storage::FileIO::OpenMode::Read,
		RbsLib::Storage::FileIO::SeekBase::begin);
	neb::CJsonObject obj;
	if (false == obj.Parse(fp.Read(file.GetFileSize()).ToString()))
		throw AccountException("Can not parse subject info");
	SubjectInfo info;
	info.classroom = obj("Classroom");
	info.description = obj("Description");
	info.id = subject_id;
	info.name = obj("SubjectName");
	std::sscanf(obj("Semester").c_str(), "%d-%d:%d", &info.semester_start, &info.semester_end, &info.semester);
	for (int i = 0; i < obj["TeacherID"].GetArraySize(); ++i)
	{
		std::uint64_t teacher_id = 0;
		obj["TeacherID"].Get(i, teacher_id);
		info.teachers.push_back(teacher_id);
	}
	for (int i = 0; i < obj["StudentsID"].GetArraySize(); ++i)
	{
		SubjectInfo::Student grade;
		obj["StudentsID"][i].Get("ID", grade.id);
		obj["StudentsID"][i].Get("Grade", grade.grade);
		obj["StudentsID"][i].Get("Notes", grade.notes);
		info.students.push_back(grade);
	}
	return info;
}

auto Account::SubjectManager::GetAllSubjectInfo(void) -> std::vector<SubjectInfo>
{
	std::shared_lock<std::shared_mutex> lock(Global_Subjects_Mutex);
	std::vector<SubjectInfo> info_list;
	RbsLib::Storage::StorageFile dir(SUBJECT_DIR);
	if (dir.IsExist() && dir.GetFileType() == RbsLib::Storage::FileType::FileType::Dir)
	{
		for (const auto& it : dir)
		{
			if (it.GetExtension() == ".json" && it.GetFileType() == RbsLib::Storage::FileType::FileType::Regular)
			{
				info_list.push_back(GetSubjectInfo(RbsLib::String::Convert::StringToNumber<std::uint64_t>(it.GetStem())));
			}
		}
	}
	return info_list;
}

void Account::SubjectManager::SetStudentProperty(std::uint64_t subject_id, const SubjectInfo::Student& info)
{
	std::unique_lock<std::shared_mutex> lock(Global_Subjects_Mutex);
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", SUBJECT_DIR, subject_id));
	auto fp = file.Open(RbsLib::Storage::FileIO::OpenMode::Read, RbsLib::Storage::FileIO::SeekBase::begin);
	neb::CJsonObject obj(fp.Read(file.GetFileSize()).ToString());
	for (int i = 0; i < obj["StudentsID"].GetArraySize(); ++i)
	{
		std::uint64_t temp = 0;
		if (obj["StudentsID"][i].Get("ID", temp) == false)
			throw AccountException("Read students list from subject failed");
		if (temp == info.id)
		{
			obj["StudentsID"][i].ReplaceAdd("Grade", info.grade);
			obj["StudentsID"][i].ReplaceAdd("Notes", info.notes);
			break;
		}
	}
	fp.Close();
	fp = file.Open(RbsLib::Storage::FileIO::OpenMode::Write | RbsLib::Storage::FileIO::OpenMode::Replace, RbsLib::Storage::FileIO::SeekBase::begin);
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
}

bool Account::SubjectManager::IsStudentInSubject(std::uint64_t student_id, std::uint64_t subject_id)
{
	std::shared_lock<std::shared_mutex> lock(Global_Subjects_Mutex);
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", SUBJECT_DIR, subject_id));
	auto fp = file.Open(RbsLib::Storage::FileIO::OpenMode::Read, RbsLib::Storage::FileIO::SeekBase::begin);
	neb::CJsonObject obj(fp.Read(file.GetFileSize()).ToString());
	for (int i = 0; i < obj["StudentsID"].GetArraySize(); ++i)
	{
		std::uint64_t temp = 0;
		if (obj["StudentsID"][i].Get("ID", temp) == false)
			throw AccountException("Read students list from subject failed");
		if (temp == student_id) return true;
	}
	return false;
}

Account::AccountException::AccountException(const std::string& reason) noexcept
	:reason(reason)
{
}

const char* Account::AccountException::what(void) const noexcept
{
	return this->reason.c_str();
}
