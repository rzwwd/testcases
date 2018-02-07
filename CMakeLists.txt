cmake_minimum_required (VERSION 3.2) 

PROJECT(cases)

option(BUILD_TYPE           "Set to ON to build apps"  "OCLINT")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -v -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -v -Wall ")

# ADD_EXECUTABLE( ${PROJECT_NAME} grayLevel.cpp)
# TARGET_LINK_LIBRARIES( ${PROJECT_NAME}  ${OpenCV_LIBS} )

file( GLOB ALL_CASES cases/case*.cpp cases/case*.c cases/case*.cxx )

foreach( testsourcefile ${ALL_CASES} )
    get_filename_component(fileName ${testsourcefile} NAME)
    get_filename_component(fileExt ${testsourcefile} EXT)

	if(${fileExt} STREQUAL ".cpp")
		string(REPLACE ".cpp" "" testname ${fileName} )
	elseif(${fileExt} STREQUAL ".cxx")
    	string(REPLACE ".cxx" "" testname ${fileName} )
    elseif(${fileExt} STREQUAL ".c")
    	string(REPLACE ".c" "" testname ${fileName} )
    endif(${fileExt} STREQUAL ".cpp")
	message(${BUILD_TYPE})
    ADD_EXECUTABLE( ${testname} ${testsourcefile} )
endforeach(testsourcefile ${ALL_CASES} )