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
#定义各个库的标头
file(GLOB RBS_LIB_HEADERS "rbslib/*.h")
file(GLOB JSON_LIB_HEADERS "json/*.h")
file(GLOB MODULE_LOADER_LIB_HEADERS "module_loader")
set(MODULE_SDK_HEADERS "module_sdk/module_sdk.h")

#构建MODULE_SDK库
add_library(module_sdk STATIC ${MODULE_SDK_HEADERS} "module_sdk/module_sdk.cpp" ${RBS_LIB_HEADERS})
target_link_libraries(module_sdk rbs)

#构建RBSLIB库
file(GLOB RBS_LIB_SRC "rbslib/*.cpp")
add_library(rbs STATIC ${RBS_LIB_SRC} ${RBS_LIB_HEADERS})

#构建JSON处理库
file(GLOB JSON_LIB_SRC "json/*.cpp")
add_library(json STATIC ${JSON_LIB_SRC} ${JSON_LIB_HEADERS})

#构建模块加载器库
file(GLOB MODULE_LOADER_LIB_SRC "module_loader/*.cpp")
add_library(moduleloader STATIC ${MODULE_LOADER_LIB_SRC} ${MODULE_LOADER_LIB_HEADERS} ${MODULE_SDK_HEADERS} ${RBS_LIB_HEADERS})
target_link_libraries(moduleloader "dl" rbs module_sdk)

#生成主程序
add_executable (StudentGradeManager "StudentGradeManager.cpp" ${RBS_LIB_HEADERS} ${JSON_LIB_HEADERS} ${MODULE_LOADER_LIB_HEADERS})
target_link_libraries(StudentGradeManager rbs json moduleloader)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET StudentGradeManager PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
