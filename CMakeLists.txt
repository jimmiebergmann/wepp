# Function for grouping files. Used for filters in Visual Studio.
function(createSourceGroups files rootPath)
	message("================================================")
	foreach(_source IN ITEMS ${files} )
		get_filename_component(_source_path "${_source}" PATH)
		file(RELATIVE_PATH _source_path_rel "${rootPath}" "${_source_path}")
		message("${_source_path} ${_source_path_rel} ${rootPath}")
		string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
		source_group("${_group_path}" FILES "${_source}")
	endforeach()
	message("================================================")
endfunction(createSourceGroups)

project (wepp)
cmake_minimum_required(VERSION 2.8)

# Find files
file(GLOB_RECURSE test_files "${CMAKE_SOURCE_DIR}/test/*.cpp" "${CMAKE_SOURCE_DIR}/test/*.hpp")
file(GLOB_RECURSE wepp_src "${CMAKE_SOURCE_DIR}/source/wepp/*.cpp")
file(GLOB_RECURSE wepp_inc "${CMAKE_SOURCE_DIR}/include/wepp/*.hpp" "${CMAKE_SOURCE_DIR}/include/wepp/*.inl")
file(GLOB wepp_files ${wepp_src} ${wepp_inc})

# Wepp
include_directories ("${CMAKE_SOURCE_DIR}/include") # Include header files
add_library(wepp SHARED ${wepp_files}) # Create shared library

createSourceGroups("${wepp_src}" "${CMAKE_SOURCE_DIR}/source")
createSourceGroups("${wepp_inc}" "${CMAKE_SOURCE_DIR}/include")

target_compile_definitions(wepp PRIVATE WEPP_EXPORTS)
install(TARGETS wepp DESTINATION "${CMAKE_SOURCE_DIR}lib") # Install library
set_target_properties( wepp
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib"
  ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
)
install(FILES ${wepp_inc} DESTINATION "${CMAKE_SOURCE_DIR}/include") # Install library headers


# Wepp static
include_directories ("${CMAKE_SOURCE_DIR}/include")
add_library(wepp_static STATIC ${wepp_files}) # Create shared library

createSourceGroups("${wepp_src}" "${CMAKE_SOURCE_DIR}/source")
createSourceGroups("${wepp_inc}" "${CMAKE_SOURCE_DIR}/include")

target_compile_definitions(wepp_static PRIVATE WEPP_STATIC)
install(TARGETS wepp_static DESTINATION "${CMAKE_SOURCE_DIR}lib") # Install library
set_target_properties( wepp_static
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib"
  ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib"
)
install(FILES ${wepp_inc} DESTINATION "${CMAKE_SOURCE_DIR}/include") # Install library headers


# Test
SET(gtest_force_shared_crt on CACHE BOOL
  "Use shared (DLL) run-time lib even when Google Test is built as static lib.")
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/googletest/googletest
                 ${CMAKE_SOURCE_DIR}/vendor/googletest/googletest
                 EXCLUDE_FROM_ALL) # Add googletest directly to our build.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

include_directories ("${CMAKE_SOURCE_DIR}/include")
link_directories("${CMAKE_SOURCE_DIR}/lib")
add_executable(test ${test_files})
set_target_properties( test
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
)
target_link_libraries(test gtest_main wepp)


# Complete target, for building everything.
add_custom_target(complete)
add_dependencies(complete wepp)
add_dependencies(complete wepp_static)
add_dependencies(complete test)

