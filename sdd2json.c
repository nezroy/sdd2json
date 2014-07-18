#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compat.h"

#include <sqlite3.h>
#include <sql.h>
#include <sqlext.h>
#include <yaml.h>
#include <zlib.h>

#include "getopt.h"

#include "sdd2json.h"
#include "map.h"
#include "inv.h"

unsigned short GZIP_FLAG = 1;
unsigned short JSONP_FLAG = 1;

int main(int argc, char* argv[]) {
	int c;
	char *dbname = (char *)DBNAME;
	char *dbinst = (char *)DBINST;
	char *hostname = (char *)HOSTNAME;
	char *datadir = NULL;
	char *verdesc = NULL;
	char *verstr = NULL;
	unsigned int version = 0;
	char *schemastr = NULL;
	unsigned int schema = 0;
	char *outdir = NULL;
	unsigned short usage = 0;
	unsigned short dblist = 0;
	unsigned short schemalist = 0;
	unsigned short trusted = 1;
	unsigned char term = '\0';

	// source files
	char udd_sql3[BUFLEN] = NULLSTR;
	char cert_yaml[BUFLEN] = NULLSTR;
	char gfxids_yaml[BUFLEN] = NULLSTR;
	char iconids_yaml[BUFLEN] = NULLSTR;
	char typeids_yaml[BUFLEN] = NULLSTR;

	// SQLlite things
	sqlite3 *udd_db3 = NULL;
	int db3_rc;

	// SQL server things
	SQLHENV henv = SQL_NULL_HENV;
	SQLHDBC hdbc = SQL_NULL_HDBC;
	SQLRETURN ret;
	SQLCHAR connStrOut[BUFLEN] = NULLSTR;
	SQLCHAR dsn[BUFLEN] = NULLSTR;
	SQLCHAR auth[BUFLEN] = NULLSTR;
	SQLCHAR usr[BUFLEN] = NULLSTR;
	SQLCHAR pwd[BUFLEN] = NULLSTR;
	SQLSMALLINT connStrLen;

	while ((c = getopt(argc, argv, "i:o:d:u:p:n:N:s:hvDSZP")) != -1) {
		switch (c) {
		case 'i':
			datadir = (char *)malloc(BUFLEN);
			strlcpy(datadir, optarg, BUFLEN);
			term = datadir[strlen(datadir) - 1];
			if (term != PATHSEP) {
				strlcat(datadir, SZPATHSEP, BUFLEN);
			}
			break;
		case 'o':
			outdir = (char *)malloc(BUFLEN);
			strlcpy(outdir, optarg, BUFLEN);
			term = outdir[strlen(outdir) - 1];
			if (term != PATHSEP) {
				strlcat(outdir, SZPATHSEP, BUFLEN);
			}
			break;
		case 'd':
			dbname = (char *)malloc(BUFLEN);
			strlcpy(dbname, optarg, BUFLEN);
			break;
		case 'n':
			verstr = (char *)malloc(BUFLEN);
			strlcpy(verstr, optarg, BUFLEN);
			break;
		case 'N':
			verdesc = (char *)malloc(BUFLEN);
			strlcpy(verdesc, optarg, BUFLEN);
			break;
		case 's':
			schemastr = (char *)malloc(BUFLEN);
			strlcpy(schemastr, optarg, BUFLEN);
			break;
		case 'S':
			schemalist = 1;
			break;
		case 'v':
			printf("EVE static2db version 0.0.1");
			return 0;
		case 'D':
			dblist = 1;
			break;
		case 'u':
			trusted = 0;
			strlcpy(usr, optarg, BUFLEN);
			break;
		case 'p':
			trusted = 0;
			strlcpy(pwd, optarg, BUFLEN);
			break;
		case 'P':
			JSONP_FLAG = 0;
			break;
		case 'Z':
			GZIP_FLAG = 0;
			break;
		case '?':
			printf("\n");
			usage = 1;
			break;
		}
	}

	if (1 == schemalist) {
		for (unsigned int i = 0; i < VERS_N; i++) {
			printf("%s:\tversion %d;\tschema %d\n", VERS[i].version_desc, VERS[i].version_id, VERS[i].schema_id);
		}
		return 0;
	}

	if (usage < 1 && datadir == NULL) {
		fprintf(stderr, "EVE static data dir path is required\n");
		usage = 1;
	}

	if (usage < 1 && verstr == NULL) {
		fprintf(stderr, "static data version ID is required\n");
		usage = 1;
	}

	if (usage > 0) return dump_usage();

	// parse version/schema from params and known values
	if (verstr != NULL) {
		version = atoi(verstr);
	}
	if (version == 0) {
		fprintf(stderr, "the static data version ID provided is invalid: %s\n", verstr);
		return 1;
	}
	unsigned int found_version = 0;
	for (unsigned int i = 0; i < VERS_N; i++) {
		if (VERS[i].version_id == version) {
			found_version = 1;
			schema = VERS[i].schema_id;
			if (NULL == verdesc) {
				verdesc = (char *)malloc(BUFLEN);
				strlcpy(verdesc, VERS[i].version_desc, BUFLEN);
			}
			break;
		}
	}
	if (1 != found_version) {
		fprintf(stderr, "warning: using unknown static data version ID: %d\n", version);
	}
	if (schemastr != NULL) {
		if (0 != schema) {
			fprintf(stderr, "warning: overriding known schema ID\n");
		}
		schema = atoi(schemastr);
	}
	if (0 == schema) {
		fprintf(stderr, "schema ID is required\n");
		return 1;
	}
	printf("static data: '%s', version %d, schema %d\n", verdesc, version, schema);

	// validate input/schema
	printf("checking input: %s - ", datadir);
	if (ACCESS(datadir, 0) != 0) {
		printf("err\n");
		fprintf(stderr, "could not access output path\n");
		return 1;
	}
		
	if (schema >= 95173) {
		strlcpy(cert_yaml, datadir, BUFLEN);
		strlcat(cert_yaml, "certificates.yaml", BUFLEN);
		if (ACCESS(cert_yaml, 0) != 0) {
			printf("err\n");
			fprintf(stderr, "could not access %s\n", cert_yaml);
			return 1;
		}
	}

	if (schema >= 95173) {
		strlcpy(gfxids_yaml, datadir, BUFLEN);
		strlcat(gfxids_yaml, "graphicIDs.yaml", BUFLEN);
		if (ACCESS(gfxids_yaml, 0) != 0) {
			printf("err\n");
			fprintf(stderr, "could not access %s\n", gfxids_yaml);
			return 1;
		}
	}

	if (schema >= 95173) {
		strlcpy(iconids_yaml, datadir, BUFLEN);
		strlcat(iconids_yaml, "iconIDs.yaml", BUFLEN);
		if (ACCESS(iconids_yaml, 0) != 0) {
			printf("err\n");
			fprintf(stderr, "could not access %s\n", iconids_yaml);
			return 1;
		}
	}

	if (schema >= 95173) {
		strlcpy(typeids_yaml, datadir, BUFLEN);
		strlcat(typeids_yaml, "typeIDs.yaml", BUFLEN);
		if (ACCESS(typeids_yaml, 0) != 0) {
			printf("err\n");
			fprintf(stderr, "could not access %s\n", typeids_yaml);
			return 1;
		}
	}

	if (schema >= 95173) {
		strlcpy(udd_sql3, datadir, BUFLEN);
		strlcat(udd_sql3, "universeDataDx.db", BUFLEN);
		if (ACCESS(udd_sql3, 0) != 0) {
			printf("err\n");
			fprintf(stderr, "could not access %s\n", udd_sql3);
			return 1;
		}
	}

	printf("OK\n");

	// validate output
	if (outdir == NULL) {
		outdir = (char *)malloc(2 * BUFLEN);
		strlcpy(outdir, datadir, 2 * BUFLEN);
		strlcat(outdir, SDD, 2* BUFLEN);
		strlcat(outdir, SZPATHSEP, 2 * BUFLEN);
	}
	printf("checking output: %s - ", outdir);
	if (ACCESS(outdir, 0) != 0) {
		if (MKDIR(outdir) != 0) {
			printf("err\n");
			fprintf(stderr, "could not create output path\n");
			return 1;
		}
		if (ACCESS(outdir, 0) != 0) {
			printf("err\n");
			fprintf(stderr, "could not access output path\n");
			return 1;
		}
	}
	printf("OK\n");

	// connect to SQLLITE dbs
	printf("connecting to [%s] - ", udd_sql3);

	db3_rc = sqlite3_open(udd_sql3, &udd_db3);
	if (SQLITE_OK != db3_rc) return dump_db3_error(henv, hdbc, udd_db3);
	printf("OK\n");

	// connect to SQL server
	if (0 != trusted) {
		SNPRINTF(auth, BUFLEN, "Trusted_Connection=yes");
	}
	else {
		SNPRINTF(auth, BUFLEN, "User ID=%s;Password=%s", usr, pwd);
	}
	printf("connecting to [%s\\%s] using [%s] - ", hostname, dbinst, auth);

	ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HENV, &henv);
	if (!SQL_SUCCEEDED(ret)) return dump_sql_error(henv, hdbc, udd_db3, ret, SQL_HANDLE_ENV, henv);
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);
	if (!SQL_SUCCEEDED(ret)) return dump_sql_error(henv, hdbc, udd_db3, ret, SQL_HANDLE_ENV, henv);

	if (dblist > 0) {
		printf("list drivers\n");
		unsigned short ret = check_drivers(henv, 1);
		close_handles(henv, hdbc, udd_db3);
		return ret;
	}

	if (0 != check_drivers(henv, 0)) {
		printf("err\n");
		fprintf(stderr, "SQL server driver not found\n");
		close_handles(henv, hdbc, udd_db3);
		return 1;
	}

	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	if (!SQL_SUCCEEDED(ret)) return dump_sql_error(henv, hdbc, udd_db3, ret, SQL_HANDLE_ENV, henv);

	SNPRINTF(dsn, BUFLEN, "Driver={%s};Server=%s\\%s;Database=%s;%s;", SQLDRV, hostname, dbinst, dbname, auth);
	ret = SQLDriverConnect(hdbc, NULL, dsn, strnlen(dsn, BUFLEN), connStrOut, BUFLEN, &connStrLen, SQL_DRIVER_NOPROMPT);
	if (!SQL_SUCCEEDED(ret)) return dump_sql_error(henv, hdbc, udd_db3, ret, SQL_HANDLE_DBC, hdbc);

	printf("OK\n");

	// create metainfo file
	char metafile[BUFLEN] = NULLSTR;
	strlcpy(metafile, outdir, BUFLEN);
	strlcat(metafile, "metainf.json", BUFLEN);
	printf("creating %s - ", metafile);
	FILE *f = fopen(metafile, "w");
	if (f == NULL) {
		printf("err\n");
		fprintf(stderr, "error opening file");
		close_handles(henv, hdbc, udd_db3);
		return 1;
	}
	printf("OK\n");
	fprintf(f, "{\n");
	fprintf(f, "\"formatID\":1,\n");
	fprintf(f, "\"schema\":%d,\n", schema);
	fprintf(f, "\"copy\":\"%s\",\n", CCPR);
	fprintf(f, "\"version\":%d,\n", version);
	fprintf(f, "\"verdesc\":\"%s\",\n", verdesc);
	fprintf(f, "\"tables\":{\n", verdesc);

	// do stuff
	int rc = 0;

	rc = create_map(outdir, schema, udd_db3, f);
	if (0 != rc) {
		if (rc > 1) dump_db3_error(henv, hdbc, udd_db3);
		else close_handles(henv, hdbc, udd_db3);
		fclose(f);
		return 1;
	}

	fprintf(f, ",\n");
	
	rc = create_inv(outdir, schema, hdbc, f);
	if (0 != rc) {
		if (rc > 1) dump_db3_error(henv, hdbc, udd_db3);
		else close_handles(henv, hdbc, udd_db3);
		fclose(f);
		return 1;
	}

	fprintf(f, "\n}\n"); // end of sources

	// clean up connections
	fprintf(f, "}\n");
	fclose(f);
	post_file(outdir, "metainf");

	close_handles(henv, hdbc, udd_db3);

	return 0;
}


void clean_string(char *buf, int siz) {
	char *tmp = (char *)malloc(siz + 1);
	int moves = 0;
	int max = strlen(buf) + 1;
	if (max > siz) max = siz;
	for (int i = 0; i < max; i++) {
		if (i + moves > siz - 1) {
			break;
		}
		unsigned char chr = buf[i];
		if (chr == '\n') {
			tmp[i + moves] = '\\';
			tmp[i + moves + 1] = 'n';
			moves++;
		}
		else if (chr == '\r') {
			moves--;
		}
		else if (chr == '"') {
			tmp[i + moves] = '\\';
			tmp[i + moves + 1] = '"';
			moves++;
		}
		else if (chr == '\t') {
			tmp[i + moves] = '\\';
			tmp[i + moves + 1] = 't';
			moves++;
		}
		else {
			tmp[i + moves] = chr;
		}
	}
	strncpy(buf, tmp, siz);
	free(tmp);
	buf[siz - 1] = '\0';
}

void db3_column_text(char *buf, sqlite3_stmt *stmt, int col, unsigned int siz) {
	const char *stuff = sqlite3_column_text(stmt, col);
	buf[0] = 0;
	if (NULL != stuff) {
		strncpy(buf, stuff, siz - 1);
		clean_string(buf, siz);
	}
}

unsigned int dump_usage(void) {
	printf("usage: static2db <-i data_dir> <-n version> [opts]\n");
	printf("\t-i data_dir: path to the EVE static data dump files\n");
	printf("\t-n version: the version ID of the static data\n");
	printf("\t-N verdesc: a descriptive name for the static data version\n");
	printf("\t-s schema: the schema ID of the static data; defaults to version ID#\n");
	printf("\t-S: list supported schemas/versions\n");
	printf("\t-o sdd_dir: path of the output static data; default [%s]\n", OUTDIR);
	printf("\t-d dbname: specify database name to use; default [%s]\n", DBNAME);
	printf("\t-h hostname: specify SQL hostname; default [%s]\n", HOSTNAME);
	printf("\t-u username: SQL login username\n\t\tif not provided, trusted auth attempted for current user\n");
	printf("\t-p password: SQL login password\n");
	printf("\t-D: list available DB drivers\n");
	printf("\t-v: version information\n");
	printf("\t-P: disable JSONP creation\n");
	printf("\t-Z: disable gzip creation\n");
	printf("\t-?: print this usage page\n");
	return 1;
}

unsigned short check_drivers(SQLHENV henv, unsigned short dump) {
	SQLUSMALLINT dir = SQL_FETCH_FIRST;
	char driver[BUFLEN + 1];
	SQLSMALLINT driver_ret;
	char attr[BUFLEN + 1];
	SQLSMALLINT attr_ret;
	SQLRETURN ret;
	unsigned short found = 0;

	while (SQL_SUCCEEDED(ret = SQLDrivers(henv, dir, driver, sizeof(driver), &driver_ret, attr, sizeof(attr), &attr_ret))) {
		dir = SQL_FETCH_NEXT;
		if (1 == dump) {
			printf("%s - %s\n", driver, attr);
			if (ret == SQL_SUCCESS_WITH_INFO) printf("\tdata truncation\n");
		}
		else if (strncmp(driver, SQLDRV, sizeof(driver)) == 0) {
			found = 1;
			break;
		}
	}

	return (0 == dump && 0 == found);
}

void close_handles(SQLHENV henv, SQLHDBC hdbc, sqlite3 *udd_db3) {
	if (SQL_NULL_HDBC != hdbc) {
		SQLDisconnect(hdbc);
		SQLFreeConnect(hdbc);
	}
	if (SQL_NULL_HENV != henv) {
		SQLFreeEnv(henv);
	}
	if (NULL != udd_db3) {
		sqlite3_close(udd_db3);
	}
}

int dump_sql_error(SQLHENV henv, SQLHDBC hdbc, sqlite3 *udd_db3, SQLRETURN retcode, SQLSMALLINT type, SQLHANDLE hsql) {
	SQLCHAR SqlState[6], Msg[SQL_MAX_MESSAGE_LENGTH];
	SQLINTEGER error;
	SQLSMALLINT len;
	SQLRETURN rc;
	rc = SQLGetDiagRec(type, hsql, 1, SqlState, &error, Msg, SQL_MAX_MESSAGE_LENGTH, &len);
	printf("err\n");
	if (rc != SQL_NO_DATA) {
		if (len >= SQL_MAX_MESSAGE_LENGTH) {
			Msg[SQL_MAX_MESSAGE_LENGTH] = '\0';
		}
		else {
			Msg[len] = '\0';
		}
		fprintf(stderr, "SQL error: %d -- %s\n%s\n", error, SqlState, Msg);
	}
	else {
		fprintf(stderr, "SQL error: %d\n", retcode);
	}

	close_handles(henv, hdbc, udd_db3);

	return 1;
}

int dump_db3_error(SQLHENV henv, SQLHDBC hdbc, sqlite3 *udd_db3) {
	fprintf(stderr, "DB3 error: %s\n", sqlite3_errmsg(udd_db3));
	close_handles(henv, hdbc, udd_db3);
	return 1;
}

SQLINTEGER sql_column_long(SQLHSTMT stmt, SQLUSMALLINT col) {
	SQLINTEGER buf;
	SQLINTEGER len;
	SQLRETURN rc = SQLGetData(stmt, col + 1, SQL_INTEGER, &buf, 0, &len);
	if (SQL_NULL_DATA == len) return 0;
	return buf;
}

SQLREAL sql_column_float(SQLHSTMT stmt, SQLUSMALLINT col) {
	SQLREAL buf;
	SQLINTEGER len;
	SQLRETURN rc = SQLGetData(stmt, col + 1, SQL_REAL, &buf, 0, &len);
	if (SQL_NULL_DATA == len) return 0;
	return buf;
}

void sql_column_text(char *buf, SQLHSTMT stmt, SQLUSMALLINT col, unsigned int siz) {
	SQLINTEGER len;
	SQLRETURN rc = SQLGetData(stmt, col + 1, SQL_CHAR, buf, siz, &len);
	if (SQL_NULL_DATA != len) {
		buf[siz - 1] = 0;
		clean_string(buf, siz);
	}
	else {
		buf[0] = 0;
	}
}

SQLSMALLINT sql_column_bit(SQLHSTMT stmt, SQLUSMALLINT col) {
	SQLCHAR buf;
	SQLINTEGER len;
	SQLRETURN rc = SQLGetData(stmt, col + 1, SQL_BIT, &buf, 0, &len);
	if (SQL_NULL_DATA == len) return 0;
	return 0 == buf ? 0 : 1;
}

unsigned int post_file(const char *outdir, const char *filetag) {
	char jsonfile[BUFLEN] = NULLSTR;
	char jsonfilegz[BUFLEN] = NULLSTR;
	char jsonpfile[BUFLEN] = NULLSTR;
	char jsonpfilegz[BUFLEN] = NULLSTR;
	unsigned char filebuf[32768];

	SNPRINTF(jsonfile, BUFLEN, "%s%s.json\0", outdir, filetag);
	SNPRINTF(jsonfilegz, BUFLEN, "%s%s.json.gz\0", outdir, filetag);
	SNPRINTF(jsonpfile, BUFLEN, "%s%s.jsonp\0", outdir, filetag);
	SNPRINTF(jsonpfilegz, BUFLEN, "%s%s.jsonp.gz\0", outdir, filetag);

	printf("post process %s - ", filetag);
	
	if (ACCESS(jsonfile, 0) != 0) {
		printf("err\n");
		fprintf(stderr, "could not access file\n");
		return 1;
	}

	FILE *j = fopen(jsonfile, "r");
	if (j == NULL) {
		printf("err\n");
		fprintf(stderr, "error opening file\n");
		return 1;
	}
	FILE *f = NULL;
	if (JSONP_FLAG) {
		f = fopen(jsonpfile, "w");
		if (f == NULL) {
			fclose(j);
			printf("err\n");
			fprintf(stderr, "error opening file\n");
			return 1;
		}
	}
	
	gzFile jz = NULL;
	gzFile fz = NULL;
	if (GZIP_FLAG) {
		jz = gzopen(jsonfilegz, "wb9");
		if (jz == NULL) {
			printf("err\n");
			fprintf(stderr, "error opening file\n");
			return 1;
		}
		gzbuffer(jz, 65536);
	}
	if (GZIP_FLAG && JSONP_FLAG) {
		fz = gzopen(jsonpfilegz, "wb9");
		if (fz == NULL) {
			printf("err\n");
			fprintf(stderr, "error opening file\n");
			return 1;
		}
		gzbuffer(fz, 65536);
	}

	if (JSONP_FLAG) fprintf(f, "EVEn_");
	if (GZIP_FLAG && JSONP_FLAG) gzprintf(fz, "EVEn_");
	if (JSONP_FLAG) fprintf(f, filetag);
	if (GZIP_FLAG && JSONP_FLAG) gzprintf(fz, filetag);
	if (JSONP_FLAG) fprintf(f, "_callback(");
	if (GZIP_FLAG && JSONP_FLAG) gzprintf(fz, "_callback(");

	size_t readsize = 32768;
	while (readsize == 32768) {
		readsize = fread(filebuf, sizeof(unsigned char), 32768, j);
		if (JSONP_FLAG) fwrite(filebuf, sizeof(unsigned char), readsize, f);
		if (GZIP_FLAG) gzwrite(jz, filebuf, readsize);
		if (GZIP_FLAG && JSONP_FLAG) gzwrite(fz, filebuf, readsize);
	}
	
	if (GZIP_FLAG) gzclose_w(jz);
	if (GZIP_FLAG && JSONP_FLAG) {
		gzprintf(fz, ");");
		gzclose_w(fz);
	}
	if (JSONP_FLAG) {
		fprintf(f, ");");
		fclose(f);
	}
	fclose(j);

	printf("OK\n");

	return 0;
}
