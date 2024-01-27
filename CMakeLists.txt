# CMakeList.txt: StudentGradeManager 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("StudentGradeManager")

add_compile_options(-std=c++20)

#构建RBSLIB库
file(GLOB RBSLIB_SRC "rbslib/*.cpp" "rbslib/*.h")
add_library(rbslib ${RBSLIB_SRC})

#构建JSON处理库
file(GLOB JSONLIB_SRC "json/*.cpp" "json/*.h")
add_library(json_lib ${JSONLIB_SRC})

#生成主程序
add_executable (StudentGradeManager "StudentGradeManager.cpp")
target_link_libraries(StudentGradeManager rbslib json_lib)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET StudentGradeManager PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
