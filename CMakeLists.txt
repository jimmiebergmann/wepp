project (wepp)
cmake_minimum_required(VERSION 2.8)

# Find files
file(GLOB test_src "${CMAKE_SOURCE_DIR}/test/*.cpp")
file(GLOB wepp_src "${CMAKE_SOURCE_DIR}/source/*.cpp")
file(GLOB wepp_inc "${CMAKE_SOURCE_DIR}/include/wepp/*.hpp")

# Wepp

#add_definitions(-DWEPP_EXPORT) 

# Include header files
include_directories ("${CMAKE_SOURCE_DIR}/include/wepp")

# Create shared library
add_library(wepp SHARED ${wepp_src} ${wepp_inc})
target_compile_definitions(wepp PRIVATE WEPP_EXPORTS)
# Install library
install(TARGETS wepp DESTINATION "${CMAKE_SOURCE_DIR}lib")

set_target_properties( wepp
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib"
  ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
)

# Install library headers
install(FILES ${wepp_inc} DESTINATION "${CMAKE_SOURCE_DIR}/include")






include_directories ("${CMAKE_SOURCE_DIR}/include/wepp")

# Create shared library
add_library(wepp_static STATIC ${wepp_src} ${wepp_inc})
target_compile_definitions(wepp_static PRIVATE WEPP_STATIC)
# Install library
install(TARGETS wepp_static DESTINATION "${CMAKE_SOURCE_DIR}lib")

set_target_properties( wepp_static
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib"
  ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib"
)

# Install library headers
install(FILES ${wepp_inc} DESTINATION "${CMAKE_SOURCE_DIR}/include")




#message(${PROJECT_SOURCE_DIR})
#message(${CMAKE_SOURCE_DIR})
#message(${CMAKE_CURRENT_BINARY_DIR})

#cmake_minimum_required(VERSION 2.8)

# Generate and build Google Test
#execute_process(COMMAND ${CMAKE_COMMAND} . -G "${CMAKE_GENERATOR}" -Dgtest_force_shared_crt=ON
#  RESULT_VARIABLE result
#  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vendor/googletest/googletest )
#if(result)
#  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
#endif()
#execute_process(COMMAND ${CMAKE_COMMAND} --build .
#  RESULT_VARIABLE result
#  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vendor/googletest/googletest )
#if(result)
#  message(FATAL_ERROR "Build step for googletest failed: ${result}")
#endif()

# Add googletest directly to our build.
SET(gtest_force_shared_crt on CACHE BOOL
  "Use shared (DLL) run-time lib even when Google Test is built as static lib.") 
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/googletest/googletest
                 ${CMAKE_SOURCE_DIR}/vendor/googletest/googletest
                 EXCLUDE_FROM_ALL)
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

#SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
#SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
#SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")
#SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
#SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")
#SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib")
#SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib")



## Add test
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
include_directories ("${CMAKE_SOURCE_DIR}/include")
link_directories("${CMAKE_SOURCE_DIR}/lib")
add_executable(test ${test_src})

set_target_properties( test
  PROPERTIES
  #ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
  #LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
)
#set_target_properties(test PROPERTIES DEBUG_POSTFIX d)
target_link_libraries(test gtest_main wepp)


add_custom_target(complete)
add_dependencies(complete wepp)
add_dependencies(complete wepp_static)
add_dependencies(complete test)








