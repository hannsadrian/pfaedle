file(GLOB_RECURSE ad_cppgtfs_SOURCES *.cpp)

include_directories(
	${CPPGTFS_INCLUDE_DIR}
	SYSTEM ${LIBZIP_INCLUDE_DIR}
	SYSTEM ${LIBZIP_CONF_INCLUDE_DIR}
)

add_library(ad_cppgtfs ${ad_cppgtfs_SOURCES})
target_link_libraries(ad_cppgtfs ad_csvparser ${LIBZIP_LIBRARY})
