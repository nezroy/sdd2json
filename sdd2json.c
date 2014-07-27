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

int main(int argc, char* argv[]) {
	int c;
	char *dbname = (char *)DBNAME;
	char *dbinst = (char *)DBINST;
	char *hostname = (char *)HOSTNAME;
	char *datadir = NULL;
	char *verdesc = NULL;
	char *verstr = NULL;
	char *prefix = NULL;
	unsigned int version = 0;
	char *schemastr = NULL;
	unsigned short usage = 0;
	unsigned short dblist = 0;
	unsigned short schemalist = 0;
	unsigned short prefixlist = 0;
	unsigned short trusted = 1;
	unsigned char term = '\0';

	// source files
	char udd_sql3[BUFLEN] = NULLSTR;
	char cert_yaml[BUFLEN] = NULLSTR;
	char gfxids_yaml[BUFLEN] = NULLSTR;
	char iconids_yaml[BUFLEN] = NULLSTR;
	char typeids_yaml[BUFLEN] = NULLSTR;
	char blueprints_yaml[BUFLEN] = NULLSTR;

	// SQL things
	int db3_rc;
	SQLRETURN ret;
	SQLCHAR connStrOut[BUFLEN] = NULLSTR;
	SQLCHAR dsn[BUFLEN] = NULLSTR;
	SQLCHAR auth[BUFLEN] = NULLSTR;
	SQLCHAR usr[BUFLEN] = NULLSTR;
	SQLCHAR pwd[BUFLEN] = NULLSTR;
	SQLSMALLINT connStrLen;

	// set globals
	GZIP_FLAG = 1;
	JSONP_FLAG = 1;
	SCHEMA = 0;
	JSON_DIR = NULL;
	H_ENV = SQL_NULL_HENV;
	H_DBC = SQL_NULL_HDBC;
	H_DBC2 = SQL_NULL_HDBC;
	DB3_UD = NULL;

	while ((c = getopt(argc, argv, "i:o:d:u:p:n:N:s:x:hvXDSZP")) != -1) {
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
			JSON_DIR = (char *)malloc(BUFLEN);
			strlcpy(JSON_DIR, optarg, BUFLEN);
			term = JSON_DIR[strlen(JSON_DIR) - 1];
			if (term != PATHSEP) {
				strlcat(JSON_DIR, SZPATHSEP, BUFLEN);
			}
			break;
		case 'x':
			prefix = (char *)malloc(BUFLEN);
			strlcpy(prefix, optarg, BUFLEN);
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
			printf("sdd2json version %d.%d.%d", SDD2JSON_V_MAJOR, SDD2JSON_V_MINOR, SDD2JSON_V_PATCH);
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
		case 'X':
			prefixlist = 1;
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

	if (1 == prefixlist) {
		printf("\tcrp\n");
		printf("\tdgm\n");
		printf("\tinv\n");
		printf("\tmap\n");
		printf("\tram\n");
		printf("\tsta\n");
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
			SCHEMA = VERS[i].schema_id;
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
		if (0 != SCHEMA) {
			fprintf(stderr, "warning: overriding known schema ID\n");
		}
		SCHEMA = atoi(schemastr);
	}
	if (0 == SCHEMA) {
		fprintf(stderr, "schema ID is required\n");
		return 1;
	}
	printf("static data: '%s', version %d, schema %d\n", verdesc, version, SCHEMA);

	// validate input/schema
	printf("checking input: %s - ", datadir);
	if (ACCESS(datadir, 0) != 0) {
		printf("err\n");
		fprintf(stderr, "could not access output path\n");
		return 1;
	}
		
	if (SCHEMA >= 100038) {
		strlcpy(cert_yaml, datadir, BUFLEN);
		strlcat(cert_yaml, "certificates.yaml", BUFLEN);
		if (ACCESS(cert_yaml, 0) != 0) {
			printf("err\n");
			fprintf(stderr, "could not access %s\n", cert_yaml);
			return 1;
		}
	}

	if (SCHEMA >= 100038) {
		strlcpy(gfxids_yaml, datadir, BUFLEN);
		strlcat(gfxids_yaml, "graphicIDs.yaml", BUFLEN);
		if (ACCESS(gfxids_yaml, 0) != 0) {
			printf("err\n");
			fprintf(stderr, "could not access %s\n", gfxids_yaml);
			return 1;
		}
	}

	if (SCHEMA >= 100038) {
		strlcpy(iconids_yaml, datadir, BUFLEN);
		strlcat(iconids_yaml, "iconIDs.yaml", BUFLEN);
		if (ACCESS(iconids_yaml, 0) != 0) {
			printf("err\n");
			fprintf(stderr, "could not access %s\n", iconids_yaml);
			return 1;
		}
	}

	if (SCHEMA >= 100038) {
		strlcpy(typeids_yaml, datadir, BUFLEN);
		strlcat(typeids_yaml, "typeIDs.yaml", BUFLEN);
		if (ACCESS(typeids_yaml, 0) != 0) {
			printf("err\n");
			fprintf(stderr, "could not access %s\n", typeids_yaml);
			return 1;
		}
	}

	if (SCHEMA >= 100038) {
		strlcpy(udd_sql3, datadir, BUFLEN);
		strlcat(udd_sql3, "universeDataDx.db", BUFLEN);
		if (ACCESS(udd_sql3, 0) != 0) {
			printf("err\n");
			fprintf(stderr, "could not access %s\n", udd_sql3);
			return 1;
		}
	}

	if (SCHEMA >= 100038) {
		strlcpy(blueprints_yaml, datadir, BUFLEN);
		strlcat(blueprints_yaml, "blueprints.yaml", BUFLEN);
		if (ACCESS(blueprints_yaml, 0) != 0) {
			printf("err\n");
			fprintf(stderr, "could not access %s\n", blueprints_yaml);
			return 1;
		}
	}

	printf("OK\n");

	// validate output
	if (JSON_DIR == NULL) {
		JSON_DIR = (char *)malloc(2 * BUFLEN);
		strlcpy(JSON_DIR, datadir, 2 * BUFLEN);
		strlcat(JSON_DIR, SDD, 2* BUFLEN);
		strlcat(JSON_DIR, SZPATHSEP, 2 * BUFLEN);
	}
	printf("checking output: %s - ", JSON_DIR);
	if (ACCESS(JSON_DIR, 0) != 0) {
		if (MKDIR(JSON_DIR) != 0) {
			printf("err\n");
			fprintf(stderr, "could not create output path\n");
			return 1;
		}
		if (ACCESS(JSON_DIR, 0) != 0) {
			printf("err\n");
			fprintf(stderr, "could not access output path\n");
			return 1;
		}
	}
	printf("OK\n");

	// connect to SQLLITE dbs
	printf("connecting to [%s] - ", udd_sql3);

	db3_rc = sqlite3_open(udd_sql3, &DB3_UD);
	if (SQLITE_OK != db3_rc) return dump_db3_error(DB3_UD, 1);
	printf("OK\n");

	// connect to SQL server
	if (0 != trusted) {
		SNPRINTF(auth, BUFLEN, "Trusted_Connection=yes");
	}
	else {
		SNPRINTF(auth, BUFLEN, "User ID=%s;Password=%s", usr, pwd);
	}
	printf("connecting to [%s\\%s] using [%s] - ", hostname, dbinst, auth);

	ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HENV, &H_ENV);
	if (!SQL_SUCCEEDED(ret)) return dump_sql_error(ret, SQL_HANDLE_ENV, H_ENV, 1);
	ret = SQLSetEnvAttr(H_ENV, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3_80, 0);
	if (!SQL_SUCCEEDED(ret)) return dump_sql_error(ret, SQL_HANDLE_ENV, H_ENV, 1);

	if (dblist > 0) {
		printf("list drivers\n");
		unsigned short ret = check_drivers(1);
		close_handles();
		return ret;
	}

	if (0 != check_drivers(0)) {
		printf("err\n");
		fprintf(stderr, "SQL server driver not found\n");
		close_handles();
		return 1;
	}

	ret = SQLAllocHandle(SQL_HANDLE_DBC, H_ENV, &H_DBC);
	if (!SQL_SUCCEEDED(ret)) return dump_sql_error(ret, SQL_HANDLE_ENV, H_ENV, 1);
	ret = SQLAllocHandle(SQL_HANDLE_DBC, H_ENV, &H_DBC2);
	if (!SQL_SUCCEEDED(ret)) return dump_sql_error(ret, SQL_HANDLE_ENV, H_ENV, 1);

	SNPRINTF(dsn, BUFLEN, "Driver={%s};Server=%s\\%s;Database=%s;%s;", SQLDRV, hostname, dbinst, dbname, auth);
	ret = SQLDriverConnect(H_DBC, NULL, dsn, strnlen(dsn, BUFLEN), connStrOut, BUFLEN, &connStrLen, SQL_DRIVER_NOPROMPT);
	if (!SQL_SUCCEEDED(ret)) return dump_sql_error(ret, SQL_HANDLE_DBC, H_DBC, 1);
	ret = SQLDriverConnect(H_DBC2, NULL, dsn, strnlen(dsn, BUFLEN), connStrOut, BUFLEN, &connStrLen, SQL_DRIVER_NOPROMPT);
	if (!SQL_SUCCEEDED(ret)) return dump_sql_error(ret, SQL_HANDLE_DBC, H_DBC2, 1);

	printf("OK\n");

	// create metainfo file
	char metafile[BUFLEN] = NULLSTR;
	strlcpy(metafile, JSON_DIR, BUFLEN);
	strlcat(metafile, "metainf.json", BUFLEN);
	FILE *f = fopen(metafile, "w");
	if (f == NULL) {
		printf("err\n");
		fprintf(stderr, "error opening meta file: %s", metafile);
		close_handles();
		return 1;
	}
	printf("OK\n");
	fprintf(f, "{\n");
	fprintf(f, "\"formatID\":%d,\n", FORMAT_ID);
	fprintf(f, "\"schema\":%d,\n", SCHEMA);
	fprintf(f, "\"copy\":\"%s\",\n", CCPR);
	fprintf(f, "\"version\":%d,\n", version);
	fprintf(f, "\"verdesc\":\"%s\",\n", verdesc);
	fprintf(f, "\"tables\":{\n");

	// do stuff
	int rc = 0;
	int comma = 0;

	if (NULL == prefix || strncmp(prefix, "crp", 3) == 0) {
		if (comma++ > 0) fprintf(f, ",\n");
		rc = create_crp(f);
		if (rc != 0) {
			close_handles();
			fclose(f);
			return 1;
		}
	}

	if (NULL == prefix || strncmp(prefix, "dgm", 3) == 0) {
		if (comma++ > 0) fprintf(f, ",\n");
		rc = create_dgm(f, cert_yaml);
		if (rc != 0) {
			close_handles();
			fclose(f);
			return 1;
		}
	}

	if (NULL == prefix || strncmp(prefix, "inv", 3) == 0) {
		if (comma++ > 0) fprintf(f, ",\n");
		rc = create_inv(f, typeids_yaml, iconids_yaml);
		if (rc != 0) {
			close_handles();
			fclose(f);
			return 1;
		}
	}

	if (NULL == prefix || strncmp(prefix, "map", 3) == 0) {
		if (comma++ > 0) fprintf(f, ",\n");
		rc = create_map(f);
		if (0 != rc) {
			close_handles();
			fclose(f);
			return 1;
		}
	}

	if (NULL == prefix || strncmp(prefix, "ram", 3) == 0) {
		if (comma++ > 0) fprintf(f, ",\n");
		ret = create_ram(f, blueprints_yaml);
		if (!SQL_SUCCEEDED(ret)) {
			close_handles();
			fclose(f);
			return 1;
		}
	}

	if (NULL == prefix || strncmp(prefix, "sta", 3) == 0) {
		if (comma++ > 0) fprintf(f, ",\n");
		ret = create_sta(f);
		if (!SQL_SUCCEEDED(ret)) {
			close_handles();
			fclose(f);
			return 1;
		}
	}

	fprintf(f, "\n}\n"); // end of sources

	// clean up connections
	fprintf(f, "}\n");
	fclose(f);
	printf("metainf - ", metafile);
	post_file("metainf");

	close_handles();

	free(datadir);
	free(verdesc);
	free(verstr);
	free(prefix);
	free(JSON_DIR);

	printf("\nall done!\n");

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
	printf("usage: sdd2json <-i data_dir> <-n version> [opts]\n");
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
	printf("\t-D: list available DB drivers\n");
	printf("\t-v: version information\n");
	printf("\t-x: specify a specific prefix to create (none specified == all)\n");
	printf("\t-X: list available prefixes\n");
	printf("\t-P: disable JSONP creation\n");
	printf("\t-Z: disable gzip creation\n");
	printf("\t-?: print this usage page\n");
	return 1;
}

unsigned short check_drivers(unsigned short dump) {
	SQLUSMALLINT dir = SQL_FETCH_FIRST;
	char driver[BUFLEN + 1];
	SQLSMALLINT driver_ret;
	char attr[BUFLEN + 1];
	SQLSMALLINT attr_ret;
	SQLRETURN ret;
	unsigned short found = 0;

	while (SQL_SUCCEEDED(ret = SQLDrivers(H_ENV, dir, driver, sizeof(driver), &driver_ret, attr, sizeof(attr), &attr_ret))) {
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

void close_handles() {
	if (SQL_NULL_HDBC != H_DBC) {
		SQLDisconnect(H_DBC);
		SQLFreeConnect(H_DBC);
	}
	if (SQL_NULL_HDBC != H_DBC2) {
		SQLDisconnect(H_DBC2);
		SQLFreeConnect(H_DBC2);
	}
	if (SQL_NULL_HENV != H_ENV) {
		SQLFreeEnv(H_ENV);
	}
	if (NULL != DB3_UD) {
		sqlite3_close(DB3_UD);
	}
}

int dump_sql_error(SQLRETURN retcode, SQLSMALLINT type, SQLHANDLE hsql, unsigned int statflag) {
	SQLCHAR SqlState[6], Msg[SQL_MAX_MESSAGE_LENGTH];
	SQLINTEGER error;
	SQLSMALLINT len;
	SQLRETURN rc;

	if (statflag > 0) printf("err\n");
	rc = SQLGetDiagRec(type, hsql, 1, SqlState, &error, Msg, SQL_MAX_MESSAGE_LENGTH, &len);
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

	close_handles();

	return 1;
}

int dump_db3_error(sqlite3 *db, unsigned int statflag) {
	if (statflag > 0) printf("err\n");
	fprintf(stderr, "DB3 error: %s\n", sqlite3_errmsg(db));
	close_handles();
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
		buf[0] = '\0';
	}
}

SQLSMALLINT sql_column_bit(SQLHSTMT stmt, SQLUSMALLINT col) {
	SQLCHAR buf;
	SQLINTEGER len;
	SQLRETURN rc = SQLGetData(stmt, col + 1, SQL_BIT, &buf, 0, &len);
	if (SQL_NULL_DATA == len) return 0;
	return 0 == buf ? 0 : 1;
}

unsigned int post_file(const char *filetag) {
	char jsonfile[BUFLEN] = NULLSTR;
	char jsonfilegz[BUFLEN] = NULLSTR;
	char jsonpfile[BUFLEN] = NULLSTR;
	char jsonpfilegz[BUFLEN] = NULLSTR;
	unsigned char filebuf[32768];

	SNPRINTF(jsonfile, BUFLEN, "%s%s.json\0", JSON_DIR, filetag);
	SNPRINTF(jsonfilegz, BUFLEN, "%s%s.json.gz\0", JSON_DIR, filetag);
	SNPRINTF(jsonpfile, BUFLEN, "%s%s.jsonp\0", JSON_DIR, filetag);
	SNPRINTF(jsonpfilegz, BUFLEN, "%s%s.jsonp.gz\0", JSON_DIR, filetag);

	// printf("post process %s - ", filetag);
	printf("post process - ");
	
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

	if (JSONP_FLAG) fprintf(f, "EVEoj_");
	if (GZIP_FLAG && JSONP_FLAG) gzprintf(fz, "EVEoj_");
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

int yaml_json_mapping(FILE *f, yaml_document_t *ydoc, yaml_node_t *node) {
	yaml_node_t *bufnode;
	yaml_node_pair_t *pair;
	int first = 1;
	int quoted = 1;
	char *key = NULL;
	
	if (!node || node->type != YAML_MAPPING_NODE) {
		fprintf(f, "null");
		return 0;
	}

	fprintf(f, "{");
	for (pair = node->data.mapping.pairs.start; pair < node->data.mapping.pairs.top; pair++) {
		// get the key node for current pair; should be a scalar we'll use for property name
		bufnode = yaml_document_get_node(ydoc, pair->key);
		if (!bufnode || bufnode->type != YAML_SCALAR_NODE) continue;
		if (!first) fprintf(f, ",");
		first = 0;
		key = bufnode->data.scalar.value;
		fprintf(f, "\"%s\":", key);

		bufnode = yaml_document_get_node(ydoc, pair->value);
		if (!bufnode) fprintf(f, "null");
		else if (bufnode->type == YAML_SCALAR_NODE) {
			// a number of properties don't need quoting as they are always numeric or otherwise base types
			quoted = 1;
			if (strncmp(key, "time", 4) == 0) quoted = 0;
			else if (strncmp(key, "level", 5) == 0) quoted = 0;
			else if (strncmp(key, "iconID", 6) == 0) quoted = 0;
			else if (strncmp(key, "radius", 6) == 0) quoted = 0;
			else if (strncmp(key, "consume", 7) == 0) quoted = 0;
			else if (strncmp(key, "soundID", 7) == 0) quoted = 0;
			else if (strncmp(key, "quantity", 8) == 0) quoted = 0;
			else if (strncmp(key, "graphicID", 9) == 0) quoted = 0;
			else if (strncmp(key, "probability", 11) == 0) quoted = 0;

			if (quoted > 0) fprintf(f, "\"%s\"", bufnode->data.scalar.value);
			else fprintf(f, "%s", bufnode->data.scalar.value);
		}
		else if (bufnode->type == YAML_MAPPING_NODE) yaml_json_mapping(f, ydoc, bufnode);
		else if (bufnode->type == YAML_SEQUENCE_NODE) yaml_json_sequence(f, ydoc, bufnode);
	}
	fprintf(f, "}");

	return 0;
}

int yaml_json_sequence(FILE *f, yaml_document_t *ydoc, yaml_node_t *node) {
	yaml_node_t *bufnode;
	yaml_node_item_t *item;
	int first = 1;

	if (!node || node->type != YAML_SEQUENCE_NODE) {
		fprintf(f, "null");
		return 0;
	}

	fprintf(f, "[");
	for (item = node->data.sequence.items.start; item < node->data.sequence.items.top; item++) {
		// get the key node for current pair; should be a scalar we'll use for property name
		bufnode = yaml_document_get_node(ydoc, *item);
		if (!first) fprintf(f, ",");
		first = 0;
		if (!bufnode) fprintf(f, "null");
		else if (bufnode->type == YAML_SCALAR_NODE) fprintf(f, "\"%s\"", bufnode->data.scalar.value);
		else if (bufnode->type == YAML_MAPPING_NODE) yaml_json_mapping(f, ydoc, bufnode);
		else if (bufnode->type == YAML_SEQUENCE_NODE) yaml_json_sequence(f, ydoc, bufnode);
		else fprintf(f, "null");
	}
	fprintf(f, "]");

	return 0;
}

FILE* open_datafile(const char *name) {
	char jsonfile[BUFLEN] = NULLSTR;
	FILE *f = NULL;

	SNPRINTF(jsonfile, BUFLEN, "%s%s.json", JSON_DIR, name);
	printf("%s - ", name);
	f = fopen(jsonfile, "w");
	if (f == NULL) {
		printf("err\n");
		fprintf(stderr, "error opening file for write: %s\n", jsonfile);
		return NULL;
	}
	fprintf(f, "{\n");
	fprintf(f, "\"formatID\":%d,\n", FORMAT_ID);
	fprintf(f, "\"schema\":%u,\n", SCHEMA);
	fprintf(f, "\"copy\":\"%s\",\n", CCPR);
	fprintf(f, "\"tables\":{\n");
	fflush(f);

	return f;
}

unsigned int close_datafile(const char *name, FILE *f) {
	fprintf(f, "\n}\n}\n");
	fclose(f);
	// printf("OK\n");
	return post_file(name);
}

FILE* open_segment(const char *name, unsigned int segment) {
	char jsonfile[BUFLEN] = NULLSTR;
	FILE *f = NULL;

	SNPRINTF(jsonfile, BUFLEN, "%s%s_%02u.json", JSON_DIR, name, segment);
	printf("%s_%02u - ", name, segment);
	f = fopen(jsonfile, "w");
	if (f == NULL) {
		printf("err\n");
		fprintf(stderr, "error opening file for write: %s\n", jsonfile);
		return NULL;
	}
	fprintf(f, "{\n");
	fprintf(f, "\"formatID\":%d,\n", FORMAT_ID);
	fprintf(f, "\"schema\":%u,\n", SCHEMA);
	fprintf(f, "\"copy\":\"%s\",\n", CCPR);
	fprintf(f, "\"tables\":{\n");
	fprintf(f, "\"%s\":{\n", name);
	fprintf(f, "\"d\":{\n");
	fflush(f);

	return f;
}

unsigned int close_segment(const char *name, unsigned int segment, FILE *f, int count) {
	char jsonfile[BUFLEN] = NULLSTR;
	SNPRINTF(jsonfile, BUFLEN, "%s_%02u", name, segment);

	fprintf(f, "\n},\n\"L\":%d\n}\n}\n}\n", count);
	fclose(f);
	// printf("OK\n");
	return post_file(jsonfile);
}
