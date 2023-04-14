#Cmake 最低版本号要求
cmake_minimum_required(VERSION 3.0.0)

#项目信息
project(${ifcb})

file(GLOB header_files 	*.h *.hpp)
file(GLOB source_files	*.cpp)

source_group("CMake Files" FILES CMakeLists.txt)
source_group("Header Files" FILES header_files)
source_group("Soruce Files" FILES source_files)


set(include_path 
    ${libMatSrc}
    ${libGnutSrc}
	${libIFCBSrc})
include_directories(${include_path})

add_executable(${PROJECT_NAME} ${header_files} ${source_files})

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(link_path 
		${ROOT}/build/Lib/Debug
		${ROOT}/build/Lib/Release
		${ROOT}/build/Lib/RelWithDebInfo
		${ROOT}/build/Lib/MinSizeRel)
	link_directories(${link_path} )				 
else()
	set(link_path
		${ROOT}/build/Lib)
	link_directories(${link_path} )				 
endif()

set(lib_list
	${libGnut}
	${libMat}
	${libIFCB})
target_link_libraries(${PROJECT_NAME} ${lib_list})

add_dependencies(${PROJECT_NAME} ${lib_list})
