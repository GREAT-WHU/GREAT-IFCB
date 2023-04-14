#Minimum requirement of CMake version : 3.0.0
cmake_minimum_required(VERSION 3.0.0)

#Project name and version number
project(${libMat} VERSION 0.1.0)

# set include path
include_directories(${libMatSrc})

# for all the header files and source_files
file(GLOB_RECURSE header_files *.h *.hpp)
file(GLOB_RECURSE source_files *.cpp)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	# find files and put them into different groups
	file(GLOB gutils_header_files 	gutils/*.h)
	file(GLOB newmat_header_files	newmat/*.h)

	file(GLOB gutils_source_files 	gutils/*.cpp)
	file(GLOB newmat_source_files	newmat/*.cpp)

	# add files to project
	source_group("CMake Files"  FILES CMakeLists.txt)
	source_group("Header Files" FILES header_files)
	source_group("Soruce Files" FILES source_files)
	
	source_group("Header Files\\gutils"		FILES ${gutils_header_files})
	source_group("Header Files\\newmat"		FILES ${newmat_header_files})
	
	source_group("Source Files\\gutils"		FILES ${gutils_source_files})
	source_group("Source Files\\newmat"		FILES ${newmat_source_files})
endif()

add_definitions(-D GREAT_MAT_LIBRARY)
add_library(${PROJECT_NAME} SHARED ${header_files} ${source_files})




	
