#include "database.h"

DataBase::DataBaseException::DataBaseException(const std::string& reason) noexcept
	:m_msg(reason) {}

const char* DataBase::DataBaseException::what() const noexcept
{
	return this->m_msg.c_str();
}
