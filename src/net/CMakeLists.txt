cmake_minimum_required(VERSION 2.8)

if (POLICY CMP0015)
	cmake_policy(SET CMP0015 NEW)
endif()

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")


project(net)
include_directories(./include)

IF (WIN32)
ELSE ()
add_definitions(
	-DBOOST_LOG_DYN_LINK
)
ENDIF()

add_compile_options(-std=c++11)

SET(LIBRARY_OUTPUT_PATH ./lib)
file(MAKE_DIRECTORY ${LIBRARY_OUTPUT_PATH})


################################################################################
#net
SET(LIBNAME net)

include_directories(./include)
aux_source_directory(./src SRCS)
aux_source_directory(./src ${LIBNAME}_SRCS)
file(GLOB ${LIBNAME}_HEADS ./include/*)
list(APPEND HEADS ${${LIBNAME}_HEADS})
add_library(${LIBNAME} STATIC ${${LIBNAME}_SRCS} ${${LIBNAME}_HEADS})

################################################################################
#test_bin

SET(EXECUTABLE_OUTPUT_PATH ./bin)
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

link_directories(./lib/ ../lib/)

add_executable(test_net test/test.cpp)
target_link_libraries(test_net net)
target_link_libraries(test_net ${curl_libs})
target_link_libraries(test_net jsoncpp)

IF (WIN32)
ELSE ()

target_link_libraries(test_net boost_thread)
target_link_libraries(test_net boost_filesystem)
target_link_libraries(test_net boost_system)
target_link_libraries(test_net boost_locale)
target_link_libraries(test_net boost_date_time)
target_link_libraries(test_net pthread)
target_link_libraries(test_net boost_log)
target_link_libraries(test_net boost_log_setup)

ENDIF()
