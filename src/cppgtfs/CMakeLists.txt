project (cppgtfs)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

if (!LIBZIP_FOUND)
    find_package(LibZip)
endif()

if (LIBZIP_FOUND)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLIBZIP_FOUND=1")
endif()

add_subdirectory(src)
