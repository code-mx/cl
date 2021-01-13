cmake_minimum_required(VERSION 2.8)

if (POLICY CMP0015)
	cmake_policy(SET CMP0015 NEW)
endif()

project(cl)

# 搜索boost https://cmake.org/cmake/help/latest/module/FindBoost.html
# sudo apt-get install libboost-all-dev
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

#find_package( Boost COMPONENTS filesystem regex)
find_package( Boost )
if(Boost_FOUND)
	message("inc:${Boost_INCLUDE_DIRS}")
	message("lib:${Boost_LIBRARY_DIRS}")
	message("lib:${BOOST_LIBRARYDIR}")
	message("lib:${Boost_LIBRARIES}")
	message("lib:${Boost_LIBS}")

	#include_directories( ${Boost_INCLUDE_DIRS} )
	#link_directories(${Boost_LIBRARIES})
else(Boost_FOUND)
	#message(FATAL_ERROR "CURL library not found")
	#set(TEST ON CACHE PATH "This is a test")
endif(Boost_FOUND)

set(boost_include_path ${Boost_INCLUDE_DIRS} CACHE PATH "This is a test")
set(boost_lib_path ${Boost_LIBRARY_DIRS} CACHE PATH "This is a test")

include_directories( ${boost_include_path} )
link_directories(${boost_lib_path})

# 搜索OpenSSL https://cmake.org/cmake/help/v3.18/module/FindOpenSSL.html
find_package( OpenSSL )
if(OpenSSL_FOUND)
	message("inc:${OPENSSL_INCLUDE_DIR}")
	message("lib:${OPENSSL_LIBRARIES}")
else(OpenSSL_FOUND)
	#message(FATAL_ERROR "CURL library not found")
	#set(TEST ON CACHE PATH "This is a test")
endif(OpenSSL_FOUND)

set(OpenSSL_include_path ${OPENSSL_INCLUDE_DIR} CACHE PATH "This is a test")
set(OpenSSL_lib_path ${OPENSSL_LIBRARIES} CACHE PATH "This is a test")

include_directories( ${OpenSSL_include_path} )
link_directories(${OpenSSL_lib_path})

# 搜索curl https://cmake.org/cmake/help/v3.0/module/FindCURL.html
find_package( curl )
if(CURL_FOUND)
	message("inc:${CURL_INCLUDE_DIRS}")
	message("lib:${CURL_LIBRARIES}")
else(CURL_FOUND)
	#message(FATAL_ERROR "CURL library not found")
	#set(TEST ON CACHE PATH "This is a test")
endif(CURL_FOUND)

set(curl_include_path ${CURL_INCLUDE_DIRS} CACHE PATH "This is a test")
#set(curl_lib_path ${CURL_LIBRARIES} CACHE PATH "This is a test")
set(curl_libs ${CURL_LIBRARIES} CACHE PATH "This is a test")

include_directories( ${curl_include_path} )
link_directories(${curl_lib_path})


message("----------- trying to find Jsoncpp-------------")
find_package(Jsoncpp)

if(Jsoncpp_FOUND)
    message("INFO: we found LibJsoncpp on your pc.")
    message(Jsoncpp_FOUND = ${Jsoncpp_FOUND})
    message(Jsoncpp_INCLUDE_DIR = ${Jsoncpp_INCLUDE_DIR}) 
	message(Jsoncpp_LIBRARY = ${Jsoncpp_LIBRARY})
	
	get_cmake_property(_variableNames VARIABLES)
	foreach (_variableName ${_variableNames})
		message(STATUS "${_variableName}=${${_variableName}}")
	endforeach()

else(Jsoncpp_FOUND)
	message("WARNING: we couldn't find LibJsoncpp on your pc. DLC is disabled.")
	# JSONCPP 采用编译的方式
	#mkdir -p build/debug
	#cd build/debug
	#cmake -DCMAKE_BUILD_TYPE=debug -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DARCHIVE_INSTALL_DIR=. -G "Unix Makefiles" ../..
	#make
	set (BUILD_STATIC_LIBS ON)
	add_subdirectory(3rdparty/jsoncpp)

	message("inc:${PROJECT_BINARY_DIR}/3rdparty/jsoncpp/include")
	message("lib:${CMAKE_CURRRENT_BINARY_DIR}/3rdparty/jsoncpp/src/lib_json")

	include_directories("3rdparty/jsoncpp/include")
	link_directories("${PROJECT_BINARY_DIR}/3rdparty/jsoncpp/src/lib_json")
	# if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	# 	link_directories("${PROJECT_BINARY_DIR}/3rdparty/jsoncpp/src/lib_json/${CMAKE_BUILD_TYPE}")
	# else ()
	# 	link_directories("${PROJECT_BINARY_DIR}/3rdparty/jsoncpp/src/lib_json")
	# endif()

endif(Jsoncpp_FOUND)




add_subdirectory(algorithm)
add_subdirectory(net)
add_subdirectory(ui)
add_subdirectory(app)
#add_subdirectory(client)

#add_dependerncies(server common)