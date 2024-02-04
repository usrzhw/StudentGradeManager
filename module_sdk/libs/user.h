#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <exception>
#include <vector>
namespace Account
{
	struct StudentBasicInfo
	{
		std::uint64_t id;
		std::string name;
		std::string sex;
		std::string phone_number;
		int enrollment_date;
		std::string class_name;
		std::string college;
		std::string password;
		int permission_level;
		std::string notes;
		bool is_enable;
	};
	class AccountException : public std::exception
	{
	private:
		std::string reason;
	public:
		AccountException(const std::string& reason) noexcept;
		const char* what(void) const noexcept override;
	};
	class AccountManager
	{
	public:
		static void CreateStudent(
			std::uint64_t ID,
			const std::string& name,
			const std::string& student_sex,
			const std::string& phone_number,
			const std::string& enrollment_date,
			const std::string& pass_word,
			const std::string& college,
			const std::string& class_name,
			const std::string& notes,
			int permission_level
		);
		static void CreateTeacher(
			std::uint64_t ID,
			const std::string& name, 
			const std::string& teacher_sex,
			const std::string& phone_number,
			const std::string & college,
			const std::string& pass_word,
			const std::string & notes, 
			int permission_level
		);
		static bool IsStudentExist(std::uint64_t id);
		static bool IsTeacherExist(std::uint64_t id);
		static void DeleteStudent(std::uint64_t student_id);
		static void DeleteTeacher(std::uint64_t teacher_id);
		static std::string GetStudentPassword(std::uint64_t id);
		static std::string GetTeacherPassword(std::uint64_t id);
	};
	class ClassesManager
	{
	public:
		static void CreateClass(const std::string& class_name, std::uint64_t teacherID);
		static void DeleteClass(const std::string& class_name);
		static bool IsClassExist(const std::string& class_name);
	};
	class SubjectManager
	{
	public:
		static void CreateSubject(
			std::uint64_t subject_id,
			const std::string& subject_name,
			int begin_year,
			int end_year,
			const std::string& description
			);
		static void AddStudent(std::uint64_t student_id, std::uint64_t subject_id, const std::string & notes);
		static void AddTeacher(std::uint64_t teacher, std::uint64_t subject_id);
		static void RemoveStudent(std::uint64_t student_id, std::uint64_t subject_id);
		static void RemoveTeacher(std::uint64_t teacher_id, std::uint64_t subject_id);
		static void DeleteSubject(std::uint64_t subject_id);
		static bool IsSubjectExist(std::uint64_t subject_id);
	};
	class User
	{
	private:
		std::string name;
		std::uint64_t ID;
		std::string token;
	public:
		User(const std::string& name, std::uint64_t ID, const std::string& token);
		User() = default;

	};
	
}