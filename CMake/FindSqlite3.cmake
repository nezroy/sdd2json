set(SQLITE3_FOUND false)
if(NOT SQLITE3_DEFDIR)
	set(SQLITE3_DEFDIR "SQLITE3_DIR-NOTFOUND")
endif()
set(SQLITE3_DIR ${SQLITE3_DEFDIR} CACHE PATH "The path containing SQLite 3.x amalgamated source code.")

mark_as_advanced(SQLITE3_HEADER)
find_file(SQLITE3_HEADER sqlite3.h ${SQLITE3_DIR})

mark_as_advanced(SQLITE3_SOURCE)
find_file(SQLITE3_SOURCE sqlite3.c ${SQLITE3_DIR})

if(SQLITE3_DIR AND SQLITE3_HEADER AND SQLITE3_SOURCE)
	set(SQLITE3_FOUND true)
endif()

if(NOT SQLITE3_FOUND)
	if(SQLITE3_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find SQLite3")
	else()
		message(STATUS "Optional package SQLite3 was not found" )
	endif()
endif()
