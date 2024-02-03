#include "user.h"
#include <shared_mutex>
#include "../../rbslib/Storage.h"
#include "../../rbslib/FileIO.h"
#include "../../json/CJsonObject.h"

std::shared_mutex Global_Student_Mutex;//读写学生信息文件时加锁
std::shared_mutex Global_Teacher_Mutex;//读写教师信息文件时加锁
std::shared_mutex Global_Classes_Mutex;//读写班级信息文件时加锁

void Account::AccountManager::CreateStudent(std::uint64_t ID, const std::string& name, const std::string& phone_number, const std::string& enrollment_date, const std::string& pass_word, const std::string & college, const std::string& class_name, int permission_level)
{
	//将学生信息存放在 Students 目录下，以学号命名，如100000.json
	RbsLib::Storage::StorageFile dir("Students");
	RbsLib::Storage::StorageFile now_dir(".");
	if (dir.IsExist() == false) now_dir.CreateDir("Students");
	if (dir[std::to_string(ID) + ".json"].IsExist()) throw AccountException("Student already exist");
	auto fp = dir[std::to_string(ID) + ".json"].Open(RbsLib::Storage::FileIO::OpenMode::Write |
		RbsLib::Storage::FileIO::OpenMode::Replace, RbsLib::Storage::FileIO::SeekBase::begin);
	neb::CJsonObject obj;
	obj.Add("Name", name);
	obj.Add("PhoneNumber", phone_number);
	obj.Add("EnrollmentDate", enrollment_date);
	obj.AddEmptySubArray("Subjects");
	obj.Add("College", college);
	obj.Add("Class", class_name);
	obj.Add("Password", pass_word);
	obj.Add("PermissionLevel", permission_level);
	obj.Add("IsEnable", true);
	std::string str = obj.ToFormattedString();
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
}

void Account::AccountManager::CreateTeacher(std::uint64_t ID, const std::string& name, const std::string& phone_number, const std::string& pass_word, const std::string& class_name, int permission_level)
{
	//将教师信息存放在 Teachers 目录下，以工号命名，如10000.json
}

void Account::AccountManager::AddSubjectToStudent(std::uint64_t student_id, std::uint64_t subject_id)
{
	//为某个学生添加课程
}

void Account::AccountManager::AddSubjectToTeacher(std::uint64_t teacher_id, std::uint64_t subject_id)
{
	//为教师添加课程
}

void Account::AccountManager::AddClassToTeacher(std::uint64_t teacher_id, std::uint64_t class_name)
{
	//为教师添加班级
}

void Account::AccountManager::RemoveSubjectFromStudent(std::uint64_t student_id, std::uint64_t subject_id)
{
	//为学生删除课程
}

void Account::AccountManager::RemoveSubjectFromTeacher(std::uint64_t teacher_id, std::uint64_t subject_id)
{
	//为教师删除课程
}

void Account::AccountManager::RemoveClassFromTeacher(std::uint64_t teacher_id, std::uint64_t class_name)
{
	//为教师删除班级
}

void Account::AccountManager::DeleteStudent(std::uint64_t student_id)
{
	//标志其为不可用账户
}

void Account::AccountManager::DeleteTeacher(std::uint64_t teacher_id)
{
	//标记其为不可用账户
}

void Account::ClassesManager::CreateClass(const std::string& class_name, std::uint64_t teacherID, int create_year)
{
	//将班级信息存储在Classes.json文件中
}

void Account::ClassesManager::AddStudentToClass(std::uint64_t student_id, const std::string class_name)
{
	//添加学生到班级中
}

void Account::ClassesManager::RemoveStudentFromClass(const std::string& class_name, std::uint64_t student_id)
{
	//从班级中移除学生
}

void Account::ClassesManager::DeleteClass(const std::string& class_name)
{
	//若班级中不存在学生，则删除班级及其相关信息，若存在学生则抛出异常
}

void Account::SubjectManager::CreateSubject(std::uint64_t subject_id, const std::string& subject_name, int begin_year, int end_year, const std::string& description)
{
	//创建课程，放在Subjects目录中，以课程编号命名，如1000000.json
}

void Account::SubjectManager::AddStudent(std::uint64_t id)
{
	//向课程添加学生
}

void Account::SubjectManager::AddTeacher(std::uint64_t id)
{
	//向课程添加老师
}

void Account::SubjectManager::RemoveStudent(std::uint64_t id)
{
	//从课程移除学生
}

void Account::SubjectManager::RemoveTeacher(std::uint64_t id)
{
	//从课程移除老师
}

void Account::SubjectManager::DeleteSubject(std::uint64_t subject_id)
{
	//删除课程，只允许删除没有老师没有学生的课程，否则抛出异常
}

Account::AccountException::AccountException(const std::string& reason) noexcept
	:reason(reason)
{
}

const char* Account::AccountException::what(void) const noexcept
{
	return this->reason.c_str();
}
