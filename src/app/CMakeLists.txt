cmake_minimum_required(VERSION 2.8)

if (POLICY CMP0015)
	cmake_policy(SET CMP0015 NEW)
endif()

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")


project(app)
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
#app
SET(LIBNAME app)

include_directories(./include)
aux_source_directory(./src SRCS)
aux_source_directory(./src ${LIBNAME}_SRCS)
file(GLOB ${LIBNAME}_HEADS ./include/*)
list(APPEND HEADS ${${LIBNAME}_HEADS})

#add_library(${LIBNAME} STATIC ${${LIBNAME}_SRCS} ${${LIBNAME}_HEADS})


SET(EXECUTABLE_OUTPUT_PATH ./bin)
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

link_directories(./lib/ ../algorithm/lib/ ../net/lib/ ../ui/lib/)

message("file:${${LIBNAME}_SRCS}")
message("file:${${LIBNAME}_HEADS}")
add_executable(${LIBNAME} ${${LIBNAME}_SRCS} ${${LIBNAME}_HEADS})
target_link_libraries(${LIBNAME} algorithm net ui)

IF (WIN32)
ELSE ()

target_link_libraries(${LIBNAME} boost_thread)
target_link_libraries(${LIBNAME} boost_filesystem)
target_link_libraries(${LIBNAME} boost_system)
target_link_libraries(${LIBNAME} boost_locale)
target_link_libraries(${LIBNAME} boost_date_time)
target_link_libraries(${LIBNAME} pthread)
target_link_libraries(${LIBNAME} boost_log)
target_link_libraries(${LIBNAME} boost_log_setup)

ENDIF()
