#include "user.h"
#include <shared_mutex>
#include <stdio.h>
#include <cJSON.h>
#include <iostream>
using namespace std;
std::shared_mutex Global_Student_Mutex;//读写学生信息文件时加锁
std::shared_mutex Global_Teacher_Mutex;//读写教师信息文件时加锁
std::shared_mutex Global_Classes_Mutex;//读写班级信息文件时加锁

void Account::AccountManager::CreateStudent(std::uint64_t ID, const std::string& name, const std::string& phone_number, const std::string& enrollment_date, const std::string& pass_word, const std::string& class_name, int permission_level)
{
	//将学生信息存放在 Students 目录下，以学号命名，如100000.json
	//该函数只允许直接访问或修改Students 目录下的内容，禁止直接修改其他文件,其他函数同理


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
