#ifndef STATIC2DB_H
#define STATIC2DB_H

#include <sqlite3.h>
#include <sql.h>

#define BUFLEN 256
#define NULLSTR {0}
#define SDD "SDD"

static const char *SQLDRV = "SQL Server";
static const char *DBNAME = "ebs_DATADUMP";
static const char *DBINST = "SQLExpress";
static const char *HOSTNAME = ".";
static const char *OUTDIR = "{datadir}/SDD";
static const char *CCPR = "©CCP hf. All rights reserved. Used with permission.";
static const char SZPATHSEP[] = { PATHSEP, '\0' };

struct Version {
	unsigned int version_id;
	unsigned int schema_id;
	const char version_desc[248];
};
static const struct Version VERS[] = {
		{ 95173, 95173, "Rubicon 1.3" },
		{ 98534, 95173, "Kronos 1.0" }
};
static const unsigned int VERS_N = sizeof(VERS) / sizeof(VERS[0]);

size_t strlcpy(char *dst, const char *src, size_t siz);
size_t strlcat(char *dst, const char *src, size_t siz);
void close_handles(SQLHENV, SQLHDBC, sqlite3*);
int dump_sql_error(SQLHENV, SQLHDBC, sqlite3*, SQLRETURN, SQLSMALLINT, SQLHANDLE);
int dump_db3_error(SQLHENV, SQLHDBC, sqlite3*);
unsigned short check_drivers(SQLHENV, unsigned short);
unsigned int dump_usage(void);
void clean_string(char*, int);

unsigned int post_file(const char*, const char*);

void db3_column_text(char*, sqlite3_stmt*, int, unsigned int);

SQLINTEGER sql_column_long(SQLHSTMT, SQLUSMALLINT);
SQLSMALLINT sql_column_bit(SQLHSTMT, SQLUSMALLINT);
SQLREAL sql_column_float(SQLHSTMT, SQLUSMALLINT);
void sql_column_text(char*, SQLHSTMT, SQLUSMALLINT, unsigned int);

#endif