#include "logger.h"
#include <time.h>
#include <cstdarg>
#include <cstdio>

static void WriteFrmtd(FILE* stream, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stream, format, args);
	va_end(args);
}
void Logger::print_log(int level, const char* log_format, va_list lst)
{
	switch (level)
	{
	case 3:
	case 2:
	case 1:
	case 0:
		vprintf(log_format, lst);
		FILE* fp;
		fp = fopen(log_format, "w");
		WriteFrmtd(fp, log_format);
		fclose(fp);
	default:
		break;
	}
}
void Logger::LogInfo(const char* format, ...)
{
	va_list lst;
	char tmp[80];
	sprintf(tmp, "[%s] [INFO] ", Time::GetFormattedTime().c_str());
	va_start(lst, format);
	Logger::print_log(0, (std::string(tmp) + format + "\n").c_str(), lst);
	va_end(lst);

}

void Logger::LogWarn(const char* format, ...)
{
	va_list lst;
	char tmp[80];
	sprintf(tmp, "[%s] [Warn] ", Time::GetFormattedTime().c_str());
	va_start(lst, format);
	Logger::print_log(1, (std::string(tmp) + format + "\n").c_str(), lst);
	va_end(lst);
}

void Logger::LogError(const char* format, ...)
{
	va_list lst;
	char tmp[80];
	sprintf(tmp, "[%s] [ERROR] ", Time::GetFormattedTime().c_str());
	va_start(lst, format);
	Logger::print_log(2, (std::string(tmp) + format + "\n").c_str(), lst);
	va_end(lst);
}

std::string Time::GetFormattedTime(void) noexcept
{
	return Time::ConvertTimeStampToFormattedTime(time(0));
}

std::string Time::ConvertTimeStampToFormattedTime(time_t time_stamp) noexcept
{
	char tmp[64];
	struct tm* timinfo;
	timinfo = localtime(&time_stamp);

	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H-%M-%S", timinfo);
	return tmp;
}
