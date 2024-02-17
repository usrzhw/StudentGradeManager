#include <iostream>
#include <cstdint>
using namespace std;

class Generator
{
public:
	std::uint64_t StudentsIDGenerator();
	std::uint64_t JobGenerator();
	std::uint64_t SubjectGenerator();
};
