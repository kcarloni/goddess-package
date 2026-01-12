
#/*
# * author:      Erik Dietz-Laursonn
# * institution: Physics Institute 3A, RWTH Aachen University, Aachen, Germany
# * copyright:   Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
# */

########## Setup the project ##########
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

# Set project name:
project(GODDESS_SIM)

#------ Boost ------#
find_package(Boost REQUIRED COMPONENTS program_options regex)
include_directories(${Boost_INCLUDE_DIRS})

########## The project ##########

# Traverse source folder and find extra includes.
file(GLOB_RECURSE files ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR} *.h *.hh)

option(BUILD_SHARED_LIBS "Build package with shared libraries." OFF)

set(last_include_path "")

foreach(file ${files} )
	get_filename_component(path ${file} PATH)

	# Check if the path has not been processed before.
	string(COMPARE EQUAL "${path}" "${last_include_path}" already_included)
	if(NOT ${already_included})
		# Adapt last_include_path.
		set(last_include_path "${path}")
		# Add include path if include subdirectory exits.
		if(IS_DIRECTORY ${path})
			message(STATUS "Added include: ${path}")
			include_directories(${path})
		endif()
	endif()
endforeach()

# make documentation (after all subprojects (which add dependencies to the documentation) are build):
add_custom_target(documentation ALL
				COMMAND echo Creating documentation
				COMMAND rm -rf ../documentation
				COMMAND mkdir -p ../documentation
				COMMAND doxygen GODDeSS.dox > /dev/null
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)


# Add subdirectories which contain CMakeLists, too 
# (they will be executed after this project or, if this project depends on them, before this project):

message(STATUS "Added subproject: ${CMAKE_CURRENT_SOURCE_DIR}/G4BasicObjects")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/G4BasicObjects)

message(STATUS "Added subproject: ${CMAKE_CURRENT_SOURCE_DIR}/SimExample")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SimExample)

# message(STATUS "Added subproject: ${CMAKE_CURRENT_SOURCE_DIR}/SimA")
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SimA)

# message(STATUS "Added subproject: ${CMAKE_CURRENT_SOURCE_DIR}/SimB")
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SimB)
