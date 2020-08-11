###################################################################
# Copyright (C) 2020 Matthias Rosenthal
# 
# This file is part of The Interactive 8051 Disassembler.
# 
# The Interactive 8051 Disassembler is licensed under Creative
# Commons-Attribution-Noncommercial-NoDerivative (CC BY-NC-ND).
# See https://creativecommons.org/licenses/by-nc-nd/4.0/legalcode
# 
# Additionally, The Interactive 8051 Disassembler including
# binaries generated from its source code is only allowed to be
# used for non-commercial purposes.
#
# The Interactive 8051 Disassembler is distributed in the hope 
# that it will be useful, but WITHOUT ANY WARRANTY; without 
# even the implied warranty of MERCHANTABILITY or FITNESS FOR 
# A PARTICULAR PURPOSE.
###################################################################

project(dis8051)

cmake_minimum_required(VERSION 3.5)

set(CMAKE_BUILD_TYPE Debug)

#set(wxWidgets_USE_LIBS)

set(wxWidgets_USE_DEBUG ON)

# If you do not want to use the default boost lib or if you do not have
# installed it, you can use your own with cmake option -DBOOST_ROOT=/your/boost/root
find_package(Boost COMPONENTS system signals thread filesystem)

if(WIN32)
    find_package(wxWidgets COMPONENTS stc core base richtext scintilla REQUIRED)
else(WIN32)
    find_package(wxWidgets COMPONENTS stc core base richtext REQUIRED)
endif(WIN32)

# if set, prints the command lines of the compilation steps
#set(CMAKE_VERBOSE_MAKEFILE ON)

include(
    "${wxWidgets_USE_FILE}" 
    "${Boost_INCLUDE_DIRS}"
    )

message(STATUS "Boost: ${Boost_LIBRARIES}")
message(STATUS "wxWidgets: ${wxWidgets_LIBRARIES}")
message(STATUS "wxWidgets include: ${wxWidgets_INCLUDE_DIRS}")
message(STATUS "wxWidgets file: ${wxWidgets_USE_FILE}")

set(USUAL_FILES annotation_canvas.cpp function_graph.cpp scroll_overview.cpp wxApp.cpp wxMain.cpp
    gui/gui_palette.cpp
    utils/utils.cpp utils/utils_wx.cpp utils/logger.cpp
    8051/disassembler.cpp 8051/disassembly.cpp 8051/function.cpp 8051/instruction.cpp 8051/switch_statement.cpp)
if(WIN32)
	add_executable(
		${PROJECT_NAME}
		${USUAL_FILES} resource.rc
		)
else(WIN32)
	add_executable(
		${PROJECT_NAME}
		${USUAL_FILES}
		)
endif(WIN32)

include_directories(${Boost_INCLUDE_DIRS} ${wxWidgets_INCLUDE_DIRS})
#include(${wxWidgets_USE_FILE})

if(WIN32)
    set (SCINTILLA_LIB "${CMAKE_CURRENT_SOURCE_DIR}/Lexers.a")
	# according to target_link_libraries docs, it's also ok to pass flags
	# (add_link_options did not work)
	# -mwindows: no command line window
	# -static-libgcc -static-libstdc++: the user should not need to provide those dlls
	set (ADDITIONAL_LINK_FLAGS -mwindows -static-libgcc -static-libstdc++)
else(WIN32)
    # I don't know why, but it's not needed under linux
    set (SCINTILLA_LIB "")
	set (ADDITIONAL_LINK_FLAGS "")
endif(WIN32)

target_link_libraries(
    ${PROJECT_NAME}
    ${Boost_LIBRARIES}
    ${wxWidgets_LIBRARIES}
	${SCINTILLA_LIB}
	${ADDITIONAL_LINK_FLAGS}
    )

# you can add -Wextra -pedantic -Werror if you want    
target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
