cmake_minimum_required(VERSION 3.12)
project(x_api)

set(CMAKE_CXX_STANDARD 20)

add_library(amxx_module_sdk INTERFACE)
target_sources(amxx_module_sdk INTERFACE
	${CMAKE_SOURCE_DIR}/sdk/amxxmodule.cpp
	${CMAKE_SOURCE_DIR}/sdk/amxxmodule.h
)
target_include_directories(amxx_module_sdk INTERFACE
	${CMAKE_SOURCE_DIR}/sdk/
)
target_compile_definitions(amxx_module_sdk INTERFACE
	-DHAVE_STDINT_H=1
)

add_library(x_api SHARED
	${CMAKE_SOURCE_DIR}/main.cpp
	${CMAKE_SOURCE_DIR}/semiclip.cpp
	${CMAKE_SOURCE_DIR}/semiclip.h
	${CMAKE_SOURCE_DIR}/immenu.hpp
	${CMAKE_SOURCE_DIR}/moduleconfig.h
)
target_include_directories(x_api PUBLIC
	${CMAKE_SOURCE_DIR}/
)

target_link_libraries(x_api PUBLIC amxx_module_sdk metahook)