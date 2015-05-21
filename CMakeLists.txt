project(ocean)

cmake_minimum_required(VERSION 2.8.12)

if (UNIX)
    add_definitions(-Wall)
endif()

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()  

#--------------------------------------------------------------------
# Add subdirectories
#--------------------------------------------------------------------
add_subdirectory(glfw)
add_subdirectory(glm)

#--------------------------------------------------------------------
#	dir variables
#--------------------------------------------------------------------
SET(incl_dir "${CMAKE_CURRENT_SOURCE_DIR}/incl")

SET(src_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")

#--------------------------------------------------------------------
#	source files
#--------------------------------------------------------------------
FILE(GLOB_RECURSE PROJECT_INCL_FILES "${incl_dir}/*.h")
FILE(GLOB PROJECT_SRC_FILES "${src_dir}/*.cpp" )

add_executable(	${CMAKE_PROJECT_NAME} 
				${PROJECT_INCL_FILES} 
				${PROJECT_SRC_FILES} 
				"${CMAKE_CURRENT_SOURCE_DIR}/glfw/deps/glad.c")

#--------------------------------------------------------------------
#	the GLFW_LIBRARIES cache variable contains all link-time 
#	dependencies of GLFW as it is currently configured.
#--------------------------------------------------------------------
TARGET_LINK_LIBRARIES(	${CMAKE_PROJECT_NAME} glfw ${GLFW_LIBRARIES})

SET_TARGET_PROPERTIES(	${CMAKE_PROJECT_NAME} 
						PROPERTIES 
						INCLUDE_DIRECTORIES 
							"${incl_dir};${CMAKE_CURRENT_SOURCE_DIR}/glfw/include;${CMAKE_CURRENT_SOURCE_DIR}/glfw/deps;${CMAKE_CURRENT_SOURCE_DIR}/glm")