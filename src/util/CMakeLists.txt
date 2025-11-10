file(GLOB util_SRC *.cpp 3rdparty/*.cpp log/*.cpp)
add_library(pb_util ${util_SRC})
target_include_directories(pb_util PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
)

file(GLOB_RECURSE util_geoSRC geo/*.cpp)
add_library(pb_util_geo ${util_geoSRC})
target_include_directories(pb_util_geo PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
)

file(GLOB_RECURSE util_jsonSRC json/*.cpp)
add_library(pb_util_json ${util_jsonSRC})
target_include_directories(pb_util_json PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
)

file(GLOB_RECURSE util_xmlSRC xml/*.cpp)
add_library(pb_util_xml ${util_xmlSRC})
target_include_directories(pb_util_xml PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
)

file(GLOB_RECURSE util_graphSRC graph/*.cpp)
add_library(pb_util_graph ${util_graphSRC})
target_include_directories(pb_util_graph PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
)

file(GLOB_RECURSE util_httpSRC http/*.cpp)
add_library(pb_util_http ${util_httpSRC})
target_include_directories(pb_util_http PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
)

if (NOT BZIP2_FOUND)
    find_package(BZip2)
endif()

if (BZIP2_FOUND)
	target_compile_definitions(pb_util INTERFACE DPBUTIL_BZIP2_FOUND=${BZIP2_FOUND} )
	target_include_directories(pb_util INTERFACE ${BZIP2_INCLUDE_DIR} )
	target_link_libraries( pb_util ${BZIP2_LIBRARIES} )

	target_compile_definitions(pb_util_http INTERFACE DPBUTIL_BZIP2_FOUND=${BZIP2_FOUND} )
	target_include_directories(pb_util_http INTERFACE ${BZIP2_INCLUDE_DIR} )
	target_link_libraries(pb_util_http ${BZIP2_LIBRARIES})

	target_compile_definitions(pb_util_xml INTERFACE DPBUTIL_BZIP2_FOUND=${BZIP2_FOUND} )
	target_include_directories(pb_util_xml INTERFACE ${BZIP2_INCLUDE_DIR} )
	target_link_libraries(pb_util_xml ${BZIP2_LIBRARIES})
endif(BZIP2_FOUND)

if (NOT ZLIB_FOUND)
    find_package(ZLIB)
endif()

if (ZLIB_FOUND)
	target_compile_definitions(pb_util INTERFACE DPBUTIL_ZLIB_FOUND=${ZLIB_FOUND} )
	target_include_directories(pb_util INTERFACE ${ZLIB_INCLUDE_DIRS} )
	target_link_libraries(pb_util ${ZLIB_LIBRARIES})

	target_compile_definitions(pb_util_http INTERFACE DPBUTIL_ZLIB_FOUND=${ZLIB_FOUND} )
	target_include_directories(pb_util_http INTERFACE ${ZLIB_INCLUDE_DIRS} )
	target_link_libraries(pb_util_http ${ZLIB_LIBRARIES})

	target_compile_definitions(pb_util_http INTERFACE DPBUTIL_ZLIB_FOUND=${ZLIB_FOUND} )
	target_include_directories(pb_util_http INTERFACE ${ZLIB_INCLUDE_DIRS} )
	target_link_libraries(pb_util_http ${ZLIB_LIBRARIES})
endif(ZLIB_FOUND)

if(CMAKE_TESTING_ENABLED)
    add_subdirectory(tests)
endif()
