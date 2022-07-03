########################################################################
# Copyright (C) 2020-2021 Matthias Rosenthal
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>
########################################################################

project(dis8051)

cmake_minimum_required(VERSION 3.5)

#set(CMAKE_BUILD_TYPE Debug)

#set(wxWidgets_USE_LIBS)

message(STATUS "build type: ${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE EQUAL "Debug")
	set(wxWidgets_USE_DEBUG ON)
else(CMAKE_BUILD_TYPE EQUAL "Debug")
	set(wxWidgets_USE_DEBUG OFF)
endif(CMAKE_BUILD_TYPE EQUAL "Debug")

# If you do not want to use the default boost lib or if you do not have
# installed it, you can use your own with cmake option -DBOOST_ROOT=/your/boost/root
find_package(Boost COMPONENTS system thread filesystem)

if(WIN32)
	set(wxWidgets_USE_STATIC ON)
endif(WIN32)
find_package(wxWidgets COMPONENTS stc core base richtext REQUIRED)

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
	# according to target_link_libraries docs, it's also ok to pass flags
	# (add_link_options did not work)
	# -mwindows: no command line window
	# -static-libgcc -static-libstdc++: the user should not need to provide those dlls
	# -static: the user should not need to provide libwinpthread-1.dll
	set (ADDITIONAL_LINK_FLAGS -mwindows -static-libgcc -static-libstdc++ -static)
else(WIN32)
	set (ADDITIONAL_LINK_FLAGS "")
endif(WIN32)

target_link_libraries(
    ${PROJECT_NAME}
    ${Boost_LIBRARIES}
    ${wxWidgets_LIBRARIES}
	${ADDITIONAL_LINK_FLAGS}
    )

# you can add -Wextra -pedantic -Werror if you want
target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
