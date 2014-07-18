set(ZLIB_FOUND false)
if(NOT ZLIB_DEFDIR)
	set(ZLIB_DEFDIR "ZLIB_DIR-NOTFOUND")
endif()
set(ZLIB_DIR ${ZLIB_DEFDIR} CACHE PATH "The path containing zlib 1.2.8 source code.")

mark_as_advanced(ZLIB_HEADER)
find_file(ZLIB_HEADER zlib.h ${ZLIB_DIR})

mark_as_advanced(ZLIB_SOURCE)
#find_file(ZLIB_SOURCE gzlib.c ${ZLIB_DIR})
file (GLOB ZLIB_SOURCE "${ZLIB_DIR}/*.c")

if(ZLIB_DIR AND ZLIB_HEADER AND ZLIB_SOURCE)
	set(ZLIB_FOUND true)
endif()

if(NOT ZLIB_FOUND)
	if(ZLIB_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find zlib")
	else()
		message(STATUS "Optional package zlib was not found" )
	endif()
endif()
