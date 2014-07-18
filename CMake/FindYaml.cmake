set(YAML_FOUND false)
if(NOT YAML_DEFDIR)
	set(YAML_DEFDIR "YAML_DIR-NOTFOUND")
endif()
set(YAML_DIR ${YAML_DEFDIR} CACHE PATH "The path containing LibYAML 0.1.x source code.")

mark_as_advanced(YAML_HEADER)
find_file(YAML_HEADER yaml.h "${YAML_DIR}/include")

mark_as_advanced(YAML_SOURCE)
#find_file(YAML_SOURCE parser.c "${YAML_DIR}/src")
file (GLOB YAML_SOURCE "${YAML_DIR}/src/*.c")

if(YAML_DIR AND YAML_HEADER AND YAML_SOURCE)
	set(YAML_FOUND true)
	set (YAML_VERSION_MAJOR 0)
	set (YAML_VERSION_MINOR 1)
	set (YAML_VERSION_PATCH 5)
	set (YAML_VERSION_STRING "${YAML_VERSION_MAJOR}.${YAML_VERSION_MINOR}.${YAML_VERSION_PATCH}")
endif()

if(NOT YAML_FOUND)
	if(YAML_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find LibYAML")
	else()
		message(STATUS "Optional package LibYAML was not found" )
	endif()
endif()

#file (GLOB SRC src/*.c)

#include_directories (include win32)
#add_definitions (-DHAVE_CONFIG_H -DYAML_DECLARE_STATIC)
#add_library (yaml STATIC ${SRC})
