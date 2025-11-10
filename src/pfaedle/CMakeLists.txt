file(GLOB_RECURSE pfaedle_SRC *.cpp)

set(pfaedle_main PfaedleMain.cpp)

list(REMOVE_ITEM pfaedle_SRC ${pfaedle_main})

include_directories(
	${PFAEDLE_INCLUDE_DIR}
)

if (OSMIUM_INCLUDE_DIR)
	include_directories(${OSMIUM_INCLUDE_DIR})
endif()

configure_file (
  "_config.h.in"
  "_config.h"
)

add_executable(pfaedle ${pfaedle_main})
add_library(pfaedle_dep ${pfaedle_SRC})

include_directories(pfaedle_dep PUBLIC ${PROJECT_SOURCE_DIR}/src/xml/include/ ${PROJECT_SOURCE_DIR}/src/cppgtfs/src)

target_link_libraries(pfaedle pfaedle_dep util configparser ad_cppgtfs -lpthread)

# Link osmium dependencies
if (OSMIUM_INCLUDE_DIR)
	find_package(EXPAT REQUIRED)
	target_link_libraries(pfaedle ${EXPAT_LIBRARIES})
	find_package(Threads REQUIRED)
	target_link_libraries(pfaedle Threads::Threads)
endif()

if (LIBZIP_FOUND)
	target_link_libraries(pfaedle ${LIBZIP_LIBRARY})
endif( LIBZIP_FOUND )

if (BZIP2_FOUND)
	target_link_libraries(pfaedle ${BZIP2_LIBRARY})
endif( BZIP2_FOUND )

add_subdirectory(tests)
