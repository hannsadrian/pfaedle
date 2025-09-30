cmake_minimum_required (VERSION 3.6)
# libosmium requires modern C++; use C++17 to satisfy dependencies
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project (pfaedle)

if (CMAKE_BUILD_TYPE)
	string(SUBSTRING ${CMAKE_BUILD_TYPE} 0 1 FIRST_CHAR)
	string(TOUPPER ${FIRST_CHAR} FIRST_CHAR)
	string(REGEX REPLACE "^.(.*)" "${FIRST_CHAR}\\1" CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}")
endif()

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

# set compiler flags, see http://stackoverflow.com/questions/7724569/debug-vs-release-in-cmake
set(CMAKE_CXX_FLAGS                "-Ofast -fno-signed-zeros -fno-trapping-math -Wall -Wno-format-extra-args -Wextra -Wformat-nonliteral -Wformat-security -Wformat=2 -Wextra -Wno-implicit-fallthrough -pedantic -Wno-keyword-macro")
set(CMAKE_CXX_FLAGS_SANITIZE       "-Og -g -fsanitize=address -fsanitize=leak -fsanitize=undefined -DLOGLEVEL=3 -DPFAEDLE_DBG=1")
set(CMAKE_CXX_FLAGS_PROFILE        "-g -pg -DLOGLEVEL=3 -DPFAEDLE_DBG=1")
set(CMAKE_CXX_FLAGS_DEBUG          "-Og -g -DLOGLEVEL=3 -DPFAEDLE_DBG=1")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS} -DLOGLEVEL=2")
set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS} -DLOGLEVEL=2")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} -g -DLOGLEVEL=3")

# export compile commands to tools like clang
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPFAEDLE_PRECISION=${PFAEDLE_PRECISION}")

# Option to silence all warnings (useful for clean error-only logs in CI/debug)
option(PFAEDLE_SILENCE_WARNINGS "Silence compiler warnings (-w)" OFF)
if (PFAEDLE_SILENCE_WARNINGS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
endif()

find_package(LibZip)
find_package(ZLIB)
find_package(BZip2)

# Try to locate libosmium and protozero (header-only)
find_path(LIBOSMIUM_INCLUDE_DIR NAMES osmium/osm.hpp PATH_SUFFIXES include)
find_path(PROTOZERO_INCLUDE_DIR NAMES protozero/pbf_reader.hpp PATH_SUFFIXES include)

if (LIBOSMIUM_INCLUDE_DIR AND PROTOZERO_INCLUDE_DIR)
	message(STATUS "Found libosmium at ${LIBOSMIUM_INCLUDE_DIR} and protozero at ${PROTOZERO_INCLUDE_DIR}")
	include_directories(${LIBOSMIUM_INCLUDE_DIR} ${PROTOZERO_INCLUDE_DIR})
	add_definitions(-DOSMIUM_ENABLED=1)
	# libosmium's XML reader uses Expat; required because we include any_input and convert XML->PBF
	find_package(EXPAT REQUIRED)
	if (EXPAT_FOUND)
		message(STATUS "Found Expat: ${EXPAT_LIBRARIES}")
		include_directories(${EXPAT_INCLUDE_DIRS})
	endif()
else()
	message(STATUS "libosmium/protozero not found; PBF support will be disabled")
endif()

if (LIBZIP_FOUND)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLIBZIP_FOUND=1")
endif()

if (ZLIB_FOUND)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DZLIB_FOUND=1")
else ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPFXML_NO_ZLIB=1")
endif()

if (BZIP2_FOUND)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBZLIB_FOUND=1")
else ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPFXML_NO_BZLIB=1")
endif()

# http://brianmilco.blogspot.de/2012/11/cmake-automatically-use-git-tags-as.html
include(GetGitRevisionDescription)
git_get_tag(VERSION_GIT)
get_git_is_dirty(VERSION_GIT_IS_DIRTY)
if ("${VERSION_GIT_IS_DIRTY}" STREQUAL "")
	set(VERSION_GIT_FULL "${VERSION_GIT}")
else()
	set(VERSION_GIT_FULL "${VERSION_GIT}-${VERSION_GIT_IS_DIRTY}")
endif()

# Download submodules if needed

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/src/configparser/.git OR NOT EXISTS ${CMAKE_SOURCE_DIR}/src/cppgtfs/.git OR NOT EXISTS ${CMAKE_SOURCE_DIR}/src/xml/.git)
    execute_process(
        COMMAND git submodule update --init --recursive
    )
endif()

add_subdirectory(src)

# tests

add_test("utilTest" utilTest)
add_test("pfaedleTest" pfaedleTest)

# handles install target
install(
	FILES pfaedle.cfg DESTINATION etc/${PROJECT_NAME} COMPONENT config PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
)

install(
	FILES ${CMAKE_BINARY_DIR}/pfaedle DESTINATION bin
	PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE COMPONENT binaries
)

install(
	FILES ${CMAKE_BINARY_DIR}/shapevl DESTINATION bin
	PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE COMPONENT binaries
)
