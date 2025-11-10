file(GLOB_RECURSE utilTest_SRC *.cpp)

add_library(pb_utilTest ${utilTest_SRC})

target_include_directories(pb_utilTest PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../>
)

add_executable(utilTest TestMain.cpp)
target_link_libraries(utilTest pb_utilTest pb_util pb_util_geo pb_util_graph pb_util_json)
