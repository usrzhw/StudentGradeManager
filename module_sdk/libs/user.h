#pragma once
#include <string>
#include <cstdint>
#include <vector>
namespace Account
{
	class AccountManager
	{
	public:
		static void CreateStudent(
			std::uint64_t ID,
			const std::string& name,
			const std::string& phone_number,
			const std::string& enrollment_date,
			const std::string& pass_word,
			const std::string& class_name,
			int permission_level
		);
		static void CreateTeacher(
			std::uint64_t ID,
			const std::string& name,
			const std::string& phone_number,
			const std::string& pass_word,
			const std::string& class_name,
			int permission_level
		);
		static void AddSubjectToStudent(std::uint64_t student_id, std::uint64_t subject_id);
		static void AddSubjectToTeacher(std::uint64_t teacher_id, std::uint64_t subject_id);
		static void AddClassToTeacher(std::uint64_t teacher_id, std::uint64_t class_name);
		static void RemoveSubjectFromStudent(std::uint64_t student_id, std::uint64_t subject_id);
		static void RemoveSubjectFromTeacher(std::uint64_t teacher_id, std::uint64_t subject_id);
		static void RemoveClassFromTeacher(std::uint64_t teacher_id, std::uint64_t class_name);
		static void DeleteStudent(std::uint64_t student_id);
		static void DeleteTeacher(std::uint64_t teacher_id);
	};
	class ClassesManager
	{
	public:
		static void CreateClass(const std::string& class_name, std::uint64_t teacherID, int create_year);
		static void AddStudentToClass(std::uint64_t student_id, const std::string class_name);
		static void RemoveStudentFromClass(const std::string& class_name, std::uint64_t student_id);
		static void DeleteClass(const std::string& class_name);
	};
	class Student
	{
	private:
		std::string name;
		std::uint64_t ID;
		std::string token;
	public:
		Student(const std::string& name, std::uint64_t ID, const std::string& token);
		Student() = default;

	};
}