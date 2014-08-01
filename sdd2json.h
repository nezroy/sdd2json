#ifndef STATIC2DB_H
#define STATIC2DB_H

#include <sqlite3.h>
#include <sql.h>
#include <yaml.h>

#define SDD2JSON_V_MAJOR 0
#define SDD2JSON_V_MINOR 1
#define SDD2JSON_V_PATCH 1

#define BUFLEN 256
#define NULLSTR {0}
#define SDD "SDD"
#define FORMAT_ID 1

#define TBLSEP(F, MF) fprintf(MF, ",\n"); fprintf(F, ",\n");

static const char *SQLDRV = "SQL Server";
static const char *DBNAME = "ebs_DATADUMP";
static const char *DBINST = "SQLExpress";
static const char *HOSTNAME = ".";
static const char *OUTDIR = "{datadir}/SDD";
static const char *CCPR = "©CCP hf. All rights reserved. Used with permission.";
static const char SZPATHSEP[] = { PATHSEP, '\0' };

// globals are awesome and I'm too lazy to pass handles all over the damn place for cleanup/util/error funcs :)
unsigned short GZIP_FLAG; // whether post_file should gzip or not
unsigned short JSONP_FLAG; // whether post_file should create jsonp or not
unsigned int SCHEMA; // global schema ID for the output data
char *JSON_DIR; // global output path for all json files being created
SQLHENV H_ENV; // global SQL server env handle
SQLHDBC H_DBC; // global SQL server primary dbc handle
SQLHDBC H_DBC2; // global SQL server secondary dbc handle
sqlite3 *DB3_UD; // global sqlite3 handle for mapUniverseData

struct Version {
	unsigned int version_id;
	unsigned int schema_id;
	const char version_desc[248];
};
static const struct Version VERS[] = {
		{ 100370, 100038, "Crius 1.6" },
		{ 100038, 100038, "Crius 1.0" }
};
static const unsigned int VERS_N = sizeof(VERS) / sizeof(VERS[0]);

size_t strlcpy(char *dst, const char *src, size_t siz);
size_t strlcat(char *dst, const char *src, size_t siz);
void close_handles();
int dump_sql_error(SQLRETURN, SQLSMALLINT, SQLHANDLE, unsigned int);
int dump_db3_error(sqlite3*, unsigned int);
unsigned short check_drivers(unsigned short);
unsigned int dump_usage(void);
void clean_string(char*, int);

unsigned int post_file(const char*);

void db3_column_text(char*, sqlite3_stmt*, int, unsigned int);

SQLINTEGER sql_column_long(SQLHSTMT, SQLUSMALLINT);
SQLSMALLINT sql_column_bit(SQLHSTMT, SQLUSMALLINT);
SQLREAL sql_column_float(SQLHSTMT, SQLUSMALLINT);
void sql_column_text(char*, SQLHSTMT, SQLUSMALLINT, unsigned int);

int yaml_json_mapping(FILE*, yaml_document_t*, yaml_node_t*);
int yaml_json_sequence(FILE*, yaml_document_t*, yaml_node_t*);

FILE* open_datafile(const char*);
unsigned int close_datafile(const char*, FILE*);
FILE* open_segment(const char*, unsigned int);
unsigned int close_segment(const char*, unsigned int, FILE*, int);

// all the prefix creation entries
unsigned int create_crp(FILE*);
unsigned int create_dgm(FILE*, const char*);
unsigned int create_inv(FILE*, const char*, const char*);
unsigned int create_map(FILE*);
unsigned int create_ram(FILE*, const char*);
unsigned int create_sta(FILE*);

#endif