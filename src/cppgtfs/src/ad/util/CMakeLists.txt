file(GLOB_RECURSE ad_util_SOURCES *.cpp)

include_directories(
	SYSTEM ${LIBZIP_INCLUDE_DIR}
	SYSTEM ${LIBZIP_CONF_INCLUDE_DIR}
)

add_library(ad_csvparser ${ad_util_SOURCES})
