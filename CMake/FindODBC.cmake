set(ODBC_FOUND false)

find_path(ODBC_INCLUDE_DIR sql.h
	/usr/include
	/usr/include/odbc
	/usr/local/include
	/usr/local/include/odbc
	/usr/local/odbc/include
	"C:/Program Files/ODBC/include"
	"C:/Program Files/Microsoft SDKs/Windows/v7.1A/include" 
	"C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/include" 
	"C:/Program Files/Microsoft SDKs/Windows/v6.0a/include" 
	"C:/ODBC/include"
	DOC "Specify the directory containing sql.h."
)

find_library(ODBC_LIBRARY odbc32
	#NAMES iodbc odbc odbcinst odbc32
	PATHS
	/usr/lib
	/usr/lib/odbc
	/usr/local/lib
	/usr/local/lib/odbc
	/usr/local/odbc/lib
	"C:/Program Files/ODBC/lib"
	"C:/ODBC/lib/debug"
	"C:/Program Files/Microsoft SDKs/Windows/v7.1A/Lib"
	"C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib"
	DOC "Specify the ODBC driver manager library here."
)

if(ODBC_LIBRARY AND ODBC_INCLUDE_DIR)
	set(ODBC_FOUND true)
endif()

set(ODBC_LIBRARIES ${ODBC_LIBRARY})

mark_as_advanced(ODBC_FOUND ODBC_LIBRARY ODBC_EXTRA_LIBRARIES ODBC_INCLUDE_DIR)
