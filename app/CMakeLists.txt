cmake_minimum_required(VERSION 3.4.1)

#aux_source_directory(src/main/cpp/UsageEnvironment DIR_LIB_SRCS_UE)
#aux_source_directory(src/main/cpp/BasicUsageEnvironment DIR_LIB_SRCS_BUE)
#aux_source_directory(src/main/cpp/groupsock DIR_LIB_SRCS_GS)
#aux_source_directory(src/main/cpp/liveMedia DIR_LIB_SRCS_LM)

add_definitions(-DSOCKLEN_T=socklen_t)
add_definitions(-DNO_SSTREAM=1)
add_definitions(-DNEWLOCALE_NOT_USED)
#add_definitions(-D_LARGEFILE_SOURCE=1)
#add_definitions(-D_FILE_OFFSET_BITS=64)
add_definitions(-DBSD=1)

include_directories(src/main/cpp/UsageEnvironment/include)
include_directories(src/main/cpp/BasicUsageEnvironment/include)
include_directories(src/main/cpp/groupsock/include)
include_directories(src/main/cpp/liveMedia/include)

file(GLOB BasicUsageEnvironmentFiles
	src/main/cpp/BasicUsageEnvironment/*.cpp)
add_library(BasicUsageEnvironment
			${BasicUsageEnvironmentFiles})

file(GLOB groupsockfiles
	src/main/cpp/groupsock/*.cpp src/main/cpp/groupsock/*.c)
add_library(groupsock ${groupsockfiles})

file(GLOB liveMediaFiles
		src/main/cpp/liveMedia/*.hh src/main/cpp/liveMedia/*.cpp src/main/cpp/liveMedia/*.c)
add_library(liveMedia ${liveMediaFiles})

file(GLOB UsageEnvironmentFiles
    src/main/cpp/UsageEnvironment/*.cpp)
add_library(UsageEnvironment ${UsageEnvironmentFiles})

add_library( native-lib
             SHARED
             src/main/cpp/native-lib.cpp )


find_library(log-lib
              log )

target_link_libraries( native-lib
                       ${log-lib}
                       liveMedia
                       groupsock
                       BasicUsageEnvironment
                       UsageEnvironment )