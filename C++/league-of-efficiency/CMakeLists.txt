cmake_minimum_required( VERSION 2.8.4 )
project(league-efficiency)

# ycm support
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# build to bin/
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# build with C++11
set(CMAKE_CXX_FLAGS "-std=c++11")

# grab all the source files
set(SOURCES
	"${PROJECT_SOURCE_DIR}/src/main.cpp"
	"${PROJECT_SOURCE_DIR}/src/spell.cpp"
	"${PROJECT_SOURCE_DIR}/src/utils.cpp"
	"${PROJECT_SOURCE_DIR}/src/jsonutils.cpp"
	"${PROJECT_SOURCE_DIR}/src/jsoncpp.cpp"
	)

# find libcurl
find_package(CURL REQUIRED)

# create executables!
add_executable(league-efficiency ${SOURCES})
target_link_libraries(league-efficiency ${CURL_LIBRARIES})
