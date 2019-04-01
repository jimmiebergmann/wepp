# Function for grouping files. Used for filters in Visual Studio.
function(createSourceGroups files rootPath)
	foreach(_source IN ITEMS ${files} )
		get_filename_component(_source_path "${_source}" PATH)
		file(RELATIVE_PATH _source_path_rel "${rootPath}" "${_source_path}")
		#message("${_source_path} ${_source_path_rel} ${rootPath}")
		string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
		source_group("${_group_path}" FILES "${_source}")
	endforeach()
endfunction(createSourceGroups)

project (wepp)
cmake_minimum_required(VERSION 2.8)

option(CODE_COVERAGE "Enable coverage reporting" OFF)

# C++11 enabled.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warnings as errors
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()


if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")


# Find files
file(GLOB_RECURSE test_files "${CMAKE_SOURCE_DIR}/test/*.cpp" "${CMAKE_SOURCE_DIR}/test/*.hpp")
file(GLOB_RECURSE petStoreExampleFile "${CMAKE_SOURCE_DIR}/example/petstoreExample.cpp")
file(GLOB_RECURSE routingExampleFile "${CMAKE_SOURCE_DIR}/example/routingExample.cpp")
file(GLOB_RECURSE wepp_src "${CMAKE_SOURCE_DIR}/source/wepp/*.cpp")
file(GLOB_RECURSE wepp_inc "${CMAKE_SOURCE_DIR}/include/wepp/*.hpp" "${CMAKE_SOURCE_DIR}/include/wepp/*.inl")
file(GLOB wepp_files ${wepp_src} ${wepp_inc})


# Wepp
include_directories ("${CMAKE_SOURCE_DIR}/include") # Include header files
add_library(wepp SHARED ${wepp_files}) # Create shared library
IF (WIN32)
	target_link_libraries(wepp "ws2_32.lib")
ENDIF()

createSourceGroups("${wepp_src}" "${CMAKE_SOURCE_DIR}/source")
createSourceGroups("${wepp_inc}" "${CMAKE_SOURCE_DIR}/include")

target_compile_definitions(wepp PRIVATE WEPP_EXPORTS)
install(TARGETS wepp DESTINATION "${CMAKE_SOURCE_DIR}lib") # Install library
set_target_properties( wepp
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib"
  ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
)
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang") ## Code coverage
  set_target_properties(wepp PROPERTIES COMPILE_FLAGS "--coverage")
endif(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

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
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib"
  ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
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
add_executable(wepp_test ${test_files})
set_target_properties( wepp_test
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
)
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang") ## Code coverage
  set_target_properties(wepp_test PROPERTIES COMPILE_FLAGS "--coverage")
endif(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

target_link_libraries(wepp_test gtest_main wepp)


# Petstore example
include_directories ("${CMAKE_SOURCE_DIR}/include")
link_directories("${CMAKE_SOURCE_DIR}/lib")
add_executable(petstoreExample ${petStoreExampleFile})
set_target_properties( petstoreExample
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
)
target_link_libraries(petstoreExample wepp)


# Routing example
include_directories ("${CMAKE_SOURCE_DIR}/include")
link_directories("${CMAKE_SOURCE_DIR}/lib")
add_executable(routingExample ${routingExampleFile})
set_target_properties( routingExample
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
)
target_link_libraries(routingExample wepp)



# Complete target, for building everything.
add_custom_target(complete)
add_dependencies(complete wepp)
add_dependencies(complete wepp_static)
add_dependencies(complete wepp_test)

