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

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-fPIC)

#定义各个库的标头
file(GLOB RBS_LIB_HEADERS "rbslib/*.h")
file(GLOB JSON_LIB_HEADERS "json/*.h")
file(GLOB MODULE_LOADER_LIB_HEADERS "module_loader/*.h")
file(GLOB CACHE_LIB_HEADERS "cache/*.h")
file(GLOB LOG_LIB_HEADERS "log/*.h")
set(MODULE_SDK_HEADERS "module_sdk/module_sdk.h")
file (GLOB CONFIG_LIB_HEADERS "configuration_manager/*.h")

#构建日志库
add_library(logger STATIC ${LOG_LIB_HEADERS} "log/logger.cpp")

#构建配置文件管理器
add_library(config STATIC ${CONFIG_LIB_HEADERS} "configuration_manager/configuration_manager.cpp" ${RBS_LIB_HEADERS} ${JSON_LIB_HEADERS})
target_link_libraries(config rbs json)

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

#构建缓存库
file(GLOB CACHE_LIB_SRC "cache/*.cpp")
add_library(cachelib STATIC ${CACHE_LIB_SRC} ${CACHE_LIB_HEADERS} ${RBS_LIB_HEADERS})
target_link_libraries(cachelib rbs)

#生成测试模块
add_library(test_module SHARED ${MODULE_SDK_HEADERS} ${RBS_LIB_HEADERS} "module_sdk/test.cpp")
target_link_libraries(test_module moduleloader)
set_target_properties(test_module PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/app)

#学生成绩管理系统模块
file(GLOB STU_MODULE_SRC "module_sdk/libs/*")
add_library(stu_module SHARED ${MODULE_SDK_HEADERS} ${JSON_LIB_HEADERS} ${RBS_LIB_HEADERS} ${LOG_LIB_HEADERS} "module_sdk/stu_module.cpp" ${STU_MODULE_SRC})
target_link_libraries(stu_module moduleloader rbs json logger "fmt" "uuid" "sqlite3")
set_target_properties(stu_module PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/app)

#复制网页等文件
file(GLOB htmlCopyFiles "html/*")
file(COPY ${htmlCopyFiles} DESTINATION ${CMAKE_BINARY_DIR}/html)
file(GLOB HTML_FILES "${CMAKE_BINARY_DIR}/html/*")

#生成主程序
add_executable(StudentGradeManager  "StudentGradeManager.cpp" ${LOG_LIB_HEADERS} ${RBS_LIB_HEADERS} ${MODULE_LOADER_LIB_HEADERS})
target_sources(StudentGradeManager PUBLIC ${HTML_FILES})
target_link_libraries(StudentGradeManager rbs moduleloader logger config)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET StudentGradeManager PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
