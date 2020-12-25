# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

cmake_minimum_required(VERSION 3.5)
if(WIN32)
	
else()
	set(CMAKE_C_COMPILER "gcc")
	set(CMAKE_CXX_COMPILER "g++")
endif()

project(breakout)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake/")
    
endif()

if(WIN32)    
    set(CMAKE_TOOLCHAIN_FILE "$ENV{HOME}/source/repos/vcpkg/scripts/buildsystems/vcpkg.cmake")
    #set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}
    #"${CMAKE_SOURCE_DIR}/lib/glfw-3.2.2")
    add_subdirectory("${CMAKE_SOURCE_DIR}/lib/glfw-3.3.2")
    add_subdirectory("${CMAKE_SOURCE_DIR}/lib/glm")

    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    #find_package(glm CONFIG REQUIRED)
    #find_package(glfw3 3.3.2 REQUIRED)
else()
  find_package(glfw3 3.3 REQUIRED)
  
  #if(NOT GLM_FOUND)
  #       message(Error "GLM not found")
  #endif(NOT GLM_FOUND)
endif()

set (SRCS
    main.cc
    glad.c
    wavefrontreader.cc)

add_executable(${CMAKE_PROJECT_NAME} ${SRCS})

if(WIN32)
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glfw glm)
else()
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glfw dl m)
endif()

