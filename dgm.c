#include <stdio.h>
#include <string.h>
#include "compat.h"

#include <sql.h>
#include <sqlext.h>
#include <yaml.h>

#include "sdd2json.h"

// all tables this creates
unsigned int create_chrAncestries(const char*, FILE*, FILE*);
unsigned int create_chrAttributes(const char*, FILE*, FILE*);
unsigned int create_chrBloodlines(const char*, FILE*, FILE*);
unsigned int create_chrFactions(const char*, FILE*, FILE*);
unsigned int create_chrRaces(const char*, FILE*, FILE*);
unsigned int create_chrCertificates(const char*, FILE*, FILE*, const char*);
unsigned int create_eveUnits(const char*, FILE*, FILE*);
unsigned int create_dgmAttributeCategories(const char*, FILE*, FILE*);
unsigned int create_dgmAttributeTypes(const char*, FILE*, FILE*);
unsigned int create_dgmEffects(const char*, FILE*, FILE*);
unsigned int create_dgmTypes(const char*, FILE*, FILE*);

unsigned int create_dgm(FILE *mf, const char *certs_yaml) {
	int rc;
	char *src;
	FILE *f = NULL;
	char jsonfile[BUFLEN] = NULLSTR;

	// dgmData
	src = "dgmMeta";
	if (!(f = open_datafile(src))) return 1;
	rc = create_chrAncestries(src, mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_chrAttributes(src, mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_chrBloodlines(src, mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_chrFactions(src, mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_chrRaces(src, mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_chrCertificates(src, mf, f, certs_yaml); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_eveUnits(src, mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_dgmAttributeCategories(src, mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_dgmAttributeTypes(src, mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_dgmEffects(src, mf, f); if (0 != rc) return rc;
	if (close_datafile(src, f)) return 1;

	// dgmTypes
	src = "dgmTypes";
	if (!(f = open_datafile(src))) return 1;
	fprintf(mf, ",\n"); rc = create_dgmTypes(src, mf, f); if (0 != rc) return rc;
	if (close_datafile(src, f)) return 1;

	return 0;
}

unsigned int create_chrAncestries(const char *j, FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[1001];
	const char *tbl = "chrAncestries";

	// meta
	fprintf(mf, "\"%s\":{\n", tbl);
	fprintf(mf, "\"j\":\"%s\",\n", j);
	fprintf(mf, "\"c\":[\"ancestryName\",\"bloodlineID\",\"description\",\"perception\",\"willpower\",\"charisma\",\"memory\",\"intelligence\",\"iconID\",\"shortDescription\"],\n");
	fprintf(mf, "\"k\":\"ancestryID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"%s\":{\n", tbl);
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM chrAncestries ORDER BY ancestryID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf); // ancestryName
		fprintf(f, "%ld,", sql_column_long(stmt, 2)); // bloodlineID
		sql_column_text(txtbuf, stmt, 3, 1001);
		fprintf(f, "\"%s\",", txtbuf); // description
		fprintf(f, "%ld,", sql_column_long(stmt, 4)); // perception
		fprintf(f, "%ld,", sql_column_long(stmt, 5)); // willpower
		fprintf(f, "%ld,", sql_column_long(stmt, 6)); // charisma
		fprintf(f, "%ld,", sql_column_long(stmt, 7)); // memory
		fprintf(f, "%ld,", sql_column_long(stmt, 8)); // intelligence
		fprintf(f, "%ld,", sql_column_long(stmt, 9)); // iconID
		sql_column_text(txtbuf, stmt, 10, 501);
		fprintf(f, "\"%s\"", txtbuf); // shortDescription
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_chrAttributes(const char *j, FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[1001];
	const char *tbl = "chrAttributes";

	// meta
	fprintf(mf, "\"%s\":{\n", tbl);
	fprintf(mf, "\"j\":\"%s\",\n", j);
	fprintf(mf, "\"c\":[\"attributeName\",\"description\",\"iconID\",\"shortDescription\",\"notes\"],\n");
	fprintf(mf, "\"k\":\"attributeID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"%s\":{\n", tbl);
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM chrAttributes ORDER BY attributeID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf); // attributeName
		sql_column_text(txtbuf, stmt, 2, 1001);
		fprintf(f, "\"%s\",", txtbuf); // description
		fprintf(f, "%ld,", sql_column_long(stmt, 3)); // iconID
		sql_column_text(txtbuf, stmt, 4, 501);
		fprintf(f, "\"%s\",", txtbuf); // shortDescription
		sql_column_text(txtbuf, stmt, 5, 501);
		fprintf(f, "\"%s\"", txtbuf); // notes
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_chrBloodlines(const char *j, FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[1001];
	const char *tbl = "chrBloodlines";

	// meta
	fprintf(mf, "\"%s\":{\n", tbl);
	fprintf(mf, "\"j\":\"%s\",\n", j);
	fprintf(mf, "\"c\":[\"bloodlineName\",\"raceID\",\"description\",\"maleDescription\",\"femaleDescription\",\"shipTypeID\",\"corporationID\",\"perception\",\"willpower\",\"charisma\",\"memory\",\"intelligence\",\"iconID\",\"shortDescription\",\"shortMaleDescription\",\"shortFemaleDescription\"],\n");
	fprintf(mf, "\"k\":\"bloodlineID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"%s\":{\n", tbl);
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM chrBloodlines ORDER BY bloodlineID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf); // bloodlineName
		fprintf(f, "%ld,", sql_column_long(stmt, 2)); // raceID
		sql_column_text(txtbuf, stmt, 3, 1001);
		fprintf(f, "\"%s\",", txtbuf); // description
		sql_column_text(txtbuf, stmt, 4, 1001);
		fprintf(f, "\"%s\",", txtbuf); // maleDescription
		sql_column_text(txtbuf, stmt, 5, 1001);
		fprintf(f, "\"%s\",", txtbuf); // femaleDescription
		fprintf(f, "%ld,", sql_column_long(stmt, 6)); // shipTypeID
		fprintf(f, "%ld,", sql_column_long(stmt, 7)); // corporationID
		fprintf(f, "%ld,", sql_column_long(stmt, 8)); // perception
		fprintf(f, "%ld,", sql_column_long(stmt, 9)); // willpower
		fprintf(f, "%ld,", sql_column_long(stmt, 10)); // charisma
		fprintf(f, "%ld,", sql_column_long(stmt, 11)); // memory
		fprintf(f, "%ld,", sql_column_long(stmt, 12)); // intelligence
		fprintf(f, "%ld,", sql_column_long(stmt, 13)); // iconID
		sql_column_text(txtbuf, stmt, 14, 501);
		fprintf(f, "\"%s\",", txtbuf); // shortDescription
		sql_column_text(txtbuf, stmt, 15, 501);
		fprintf(f, "\"%s\",", txtbuf); // shortMaleDescription
		sql_column_text(txtbuf, stmt, 16, 501);
		fprintf(f, "\"%s\"", txtbuf); // shortFemaleDescription
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_chrFactions(const char *j, FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[1001];
	const char *tbl = "chrFactions";

	// meta
	fprintf(mf, "\"%s\":{\n", tbl);
	fprintf(mf, "\"j\":\"%s\",\n", j);
	fprintf(mf, "\"c\":[\"factionName\",\"description\",\"raceIDs\",\"solarSystemID\",\"corporationID\",\"sizeFactor\",\"stationCount\",\"stationSystemCount\",\"militiaCorporationID\",\"iconID\"],\n");
	fprintf(mf, "\"k\":\"factionID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"%s\":{\n", tbl);
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM chrFactions ORDER BY factionID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf); // factionName
		sql_column_text(txtbuf, stmt, 2, 1001);
		fprintf(f, "\"%s\",", txtbuf); // description
		fprintf(f, "%ld,", sql_column_long(stmt, 3)); // raceIDs
		fprintf(f, "%ld,", sql_column_long(stmt, 4)); // solarSystemID
		fprintf(f, "%ld,", sql_column_long(stmt, 5)); // corporationID
		fprintf(f, "%g,", sql_column_float(stmt, 6)); // sizeFactor
		fprintf(f, "%ld,", sql_column_long(stmt, 7)); // stationCount
		fprintf(f, "%ld,", sql_column_long(stmt, 8)); // stationSystemCount
		fprintf(f, "%ld,", sql_column_long(stmt, 9)); // militiaCorporationID
		fprintf(f, "%ld", sql_column_long(stmt, 10)); // iconID
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_chrRaces(const char *j, FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[1001];
	const char *tbl = "chrRaces";

	// meta
	fprintf(mf, "\"%s\":{\n", tbl);
	fprintf(mf, "\"j\":\"%s\",\n", j);
	fprintf(mf, "\"c\":[\"raceName\",\"description\",\"iconID\",\"shortDescription\"],\n");
	fprintf(mf, "\"k\":\"raceID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"%s\":{\n", tbl);
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM chrRaces ORDER BY raceID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf); // raceName
		sql_column_text(txtbuf, stmt, 2, 1001);
		fprintf(f, "\"%s\",", txtbuf); // description
		fprintf(f, "%ld,", sql_column_long(stmt, 3)); // iconID
		sql_column_text(txtbuf, stmt, 4, 501);
		fprintf(f, "\"%s\"", txtbuf); // shortDescription
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_chrCertificates(const char *j, FILE *mf, FILE *f, const char *certs_yaml) {
	const char *tbl = "chrCertificates";
	FILE *f_yaml = NULL;
	int yret;
	yaml_node_t *node;
	yaml_node_t *bufnode;
	yaml_node_t *valnode;
	yaml_node_pair_t *pair;
	yaml_node_pair_t *bufpair;
	int certID = 0;
	int groupID = 0;
	char *description = NULL;
	char *name = NULL;
	int recommended_idx = 0;
	int skilltypes_idx = 0;
	int first = 0;
	yaml_parser_t parser;
	yaml_document_t ydoc;

	// meta
	fprintf(mf, "\"%s\":{\n", tbl);
	fprintf(mf, "\"j\":\"%s\",\n", j);
	fprintf(mf, "\"c\":[\"name\",\"description\",\"groupID\",\"recommendedFor\",\"skillTypes\"],\n");
	fprintf(mf, "\"m\":{\"recommendedFor\":\"[ typeID, ... ]\",\"skillTypes\":\"{ skillID: { certLevel: skillLevel, ... }, ... }\"},\n");
	fprintf(mf, "\"k\":\"raceID\",\n");
	fprintf(mf, "\"t\":\"yaml\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"%s\":{\n", tbl);
	fprintf(f, "\"d\":{\n");
	f_yaml = fopen(certs_yaml, "rb");
	if (f_yaml == NULL) {
		printf("err\n");
		fprintf(stderr, "error opening certs file\n");
		return 1;
	}
	yaml_parser_initialize(&parser);
	yaml_parser_set_input_file(&parser, f_yaml);
	yret = yaml_parser_load(&parser, &ydoc);
	if (!yret) {
		printf("err\n");
		fprintf(stderr, "error parsing certs file\n");
		yaml_parser_delete(&parser);
		return 1;
	}
	// expect first node to be a mapping node for the whole document (certIDs to mappings)
	node = yaml_document_get_root_node(&ydoc);
	if (!node || node->type != YAML_MAPPING_NODE) {
		printf("err\n");
		fprintf(stderr, "error parsing certs file - unexpected root node type\n");
		yaml_parser_delete(&parser);
		return 1;
	}
	first = 1;
	for (pair = node->data.mapping.pairs.start; pair < node->data.mapping.pairs.top; pair++) {
		// get the key node for this mapping; should be the certID
		bufnode = yaml_document_get_node(&ydoc, pair->key);
		if (bufnode->type != YAML_SCALAR_NODE) continue;
		certID = atoi(bufnode->data.scalar.value);

		// get the value node for this mapping; should be another mapping
		bufnode = yaml_document_get_node(&ydoc, pair->value);
		if (bufnode->type != YAML_MAPPING_NODE) continue;
		groupID = 0;
		description = NULL;
		name = NULL;
		recommended_idx = 0;
		skilltypes_idx = 0;
		for (bufpair = bufnode->data.mapping.pairs.start; bufpair < bufnode->data.mapping.pairs.top; bufpair++) {
			// get the key node for this mapping; test for a few things to handle
			valnode = yaml_document_get_node(&ydoc, bufpair->key);
			if (valnode->type != YAML_SCALAR_NODE) continue;
			if (strncmp(valnode->data.scalar.value, "recommendedFor", 14) == 0) recommended_idx = bufpair->value;
			if (strncmp(valnode->data.scalar.value, "skillTypes", 10) == 0) skilltypes_idx = bufpair->value;
			else if (strncmp(valnode->data.scalar.value, "groupID", 7) == 0) {
				valnode = yaml_document_get_node(&ydoc, bufpair->value);
				if (!valnode || valnode->type != YAML_SCALAR_NODE) continue;
				groupID = atoi(valnode->data.scalar.value);
			}
			else if (strncmp(valnode->data.scalar.value, "name", 4) == 0) {
				valnode = yaml_document_get_node(&ydoc, bufpair->value);
				if (!valnode || valnode->type != YAML_SCALAR_NODE) continue;
				name = calloc(valnode->data.scalar.length + 1, sizeof(char));
				memcpy(name, valnode->data.scalar.value, valnode->data.scalar.length);
			}
			else if (strncmp(valnode->data.scalar.value, "description", 4) == 0) {
				valnode = yaml_document_get_node(&ydoc, bufpair->value);
				if (!valnode || valnode->type != YAML_SCALAR_NODE) continue;
				description = calloc(valnode->data.scalar.length + 1, sizeof(char));
				memcpy(description, valnode->data.scalar.value, valnode->data.scalar.length);
			}
		}

		if (!first) fprintf(f, ",\n");
		first = 0;
		fprintf(f, "\"%d\":[\"%s\",\"%s\",\"%d\",", certID, name, description, groupID);
		if (recommended_idx) yaml_json_sequence(f, &ydoc, yaml_document_get_node(&ydoc, recommended_idx));
		else fprintf(f, "null");
		fprintf(f, ",");
		if (skilltypes_idx) yaml_json_mapping(f, &ydoc, yaml_document_get_node(&ydoc, skilltypes_idx));
		else fprintf(f, "null");
		fprintf(f, "]");
	}
	yaml_document_delete(&ydoc);
	yaml_parser_delete(&parser);
	fclose(f_yaml);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_eveUnits(const char *j, FILE *mf, FILE *f) {
	const char *tbl = "eveUnits";
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[1001];

	// meta
	fprintf(mf, "\"%s\":{\n", tbl);
	fprintf(mf, "\"j\":\"%s\",\n", j);
	fprintf(mf, "\"c\":[\"unitName\",\"displayName\",\"description\"],\n");
	fprintf(mf, "\"k\":\"unitID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"%s\":{\n", tbl);
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM eveUnits ORDER BY unitID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf); // unitName
		sql_column_text(txtbuf, stmt, 2, 51);
		fprintf(f, "\"%s\",", txtbuf); // displayName
		sql_column_text(txtbuf, stmt, 3, 1001);
		fprintf(f, "\"%s\"", txtbuf); // description
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_dgmAttributeCategories(const char *j, FILE *mf, FILE *f) {
	const char *tbl = "dgmAttributeCategories";
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[201];

	// meta
	fprintf(mf, "\"%s\":{\n", tbl);
	fprintf(mf, "\"j\":\"%s\",\n", j);
	fprintf(mf, "\"c\":[\"categoryName\",\"categoryDescription\"],\n");
	fprintf(mf, "\"k\":\"categoryID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"%s\":{\n", tbl);
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM dgmAttributeCategories ORDER BY categoryID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 51);
		fprintf(f, "\"%s\",", txtbuf); // categoryName
		sql_column_text(txtbuf, stmt, 2, 201);
		fprintf(f, "\"%s\"", txtbuf); // categoryDescription
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_dgmAttributeTypes(const char *j, FILE *mf, FILE *f) {
	const char *tbl = "dgmAttributeTypes";
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[1001];

	// meta
	fprintf(mf, "\"%s\":{\n", tbl);
	fprintf(mf, "\"j\":\"%s\",\n", j);
	fprintf(mf, "\"c\":[\"attributeName\",\"description\",\"iconID\",\"defaultValue\",\"published\",\"displayName\",\"unitID\",\"stackable\",\"highIsGood\",\"categoryID\"],\n");
	fprintf(mf, "\"k\":\"attributeID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"%s\":{\n", tbl);
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM dgmAttributeTypes ORDER BY attributeID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf); // attributeName
		sql_column_text(txtbuf, stmt, 2, 1001);
		fprintf(f, "\"%s\",", txtbuf); // description
		fprintf(f, "%ld,", sql_column_long(stmt, 3)); // iconID
		fprintf(f, "%g,", sql_column_float(stmt, 4)); // defaultValue
		fprintf(f, "%s,", sql_column_long(stmt, 5) == 0 ? "false" : "true"); // published
		sql_column_text(txtbuf, stmt, 6, 101);
		fprintf(f, "\"%s\",", txtbuf); // // displayName
		fprintf(f, "%ld,", sql_column_long(stmt, 7)); // unitID
		fprintf(f, "%s,", sql_column_long(stmt, 8) == 0 ? "false" : "true"); // stackable
		fprintf(f, "%s,", sql_column_long(stmt, 9) == 0 ? "false" : "true"); // highIsGood
		fprintf(f, "%ld", sql_column_long(stmt, 10)); // categoryID

		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_dgmEffects(const char *j, FILE *mf, FILE *f) {
	const char *tbl = "dgmEffects";
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[1001];

	// meta
	fprintf(mf, "\"%s\":{\n", tbl);
	fprintf(mf, "\"j\":\"%s\",\n", j);
	fprintf(mf, "\"c\":[\"effectName\",\"effectCategory\",\"preExpression\",\"postExpression\",\"description\",\"guid\",\"iconID\",\"isOffensive\",\"isAssistance\",\"durationAttributeID\",\"trackingSpeedAttributeID\",\"dischargeAttributeID\",\"rangeAttributeID\",\"falloffAttributeID\",\"disallowAutoRepeat\",\"published\",\"displayName\",\"isWarpSafe\",\"rangeChance\",\"electronicChance\",\"propulsionChance\",\"distribution\",\"sfxName\",\"npcUsageChanceAttributeID\",\"npcActivationChanceAttributeID\",\"fittingUsageChanceAttributeID\"],\n");
	fprintf(mf, "\"k\":\"attributeID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"%s\":{\n", tbl);
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM dgmEffects ORDER BY effectID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 401);
		fprintf(f, "\"%s\",", txtbuf); // effectName
		fprintf(f, "%ld,", sql_column_long(stmt, 2)); // effectCategory
		fprintf(f, "%ld,", sql_column_long(stmt, 3)); // preExpression
		fprintf(f, "%ld,", sql_column_long(stmt, 4)); // postExpression
		sql_column_text(txtbuf, stmt, 5, 1001);
		fprintf(f, "\"%s\",", txtbuf); // description
		sql_column_text(txtbuf, stmt, 6, 61);
		fprintf(f, "\"%s\",", txtbuf); // guid
		fprintf(f, "%ld,", sql_column_long(stmt, 7)); // iconID
		fprintf(f, "%s,", sql_column_long(stmt, 8) == 0 ? "false" : "true"); // isOffensive
		fprintf(f, "%s,", sql_column_long(stmt, 9) == 0 ? "false" : "true"); // isAssistance
		fprintf(f, "%ld,", sql_column_long(stmt, 10)); // durationAttributeID
		fprintf(f, "%ld,", sql_column_long(stmt, 11)); // trackingSpeedAttributeID
		fprintf(f, "%ld,", sql_column_long(stmt, 12)); // dischargeAttributeID
		fprintf(f, "%ld,", sql_column_long(stmt, 13)); // rangeAttributeID
		fprintf(f, "%ld,", sql_column_long(stmt, 14)); // falloffAttributeID
		fprintf(f, "%s,", sql_column_long(stmt, 15) == 0 ? "false" : "true"); // disallowAutoRepeat
		fprintf(f, "%s,", sql_column_long(stmt, 16) == 0 ? "false" : "true"); // published
		sql_column_text(txtbuf, stmt, 17, 101);
		fprintf(f, "\"%s\",", txtbuf); // // displayName
		fprintf(f, "%s,", sql_column_long(stmt, 18) == 0 ? "false" : "true"); // isWarpSafe
		fprintf(f, "%s,", sql_column_long(stmt, 19) == 0 ? "false" : "true"); // rangeChance
		fprintf(f, "%s,", sql_column_long(stmt, 20) == 0 ? "false" : "true"); // electronicChance
		fprintf(f, "%s,", sql_column_long(stmt, 21) == 0 ? "false" : "true"); // propulsionChance
		fprintf(f, "%ld,", sql_column_long(stmt, 22)); // distribution
		sql_column_text(txtbuf, stmt, 23, 21);
		fprintf(f, "\"%s\",", txtbuf); // sfxName
		fprintf(f, "%ld,", sql_column_long(stmt, 24)); // npcUsageChanceAttributeID
		fprintf(f, "%ld,", sql_column_long(stmt, 25)); // npcActivationChanceAttributeID
		fprintf(f, "%ld", sql_column_long(stmt, 26)); // fittingUsageChanceAttributeID

		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_dgmTypes(const char *j, FILE *mf, FILE *f) {
	const char *tbl = "dgmTypes";
	SQLRETURN rc;
	SQLHSTMT stmt;
	SQLHSTMT substmt;
	SQLINTEGER typeID;
	SQLINTEGER valueInt;
	SQLFLOAT valueFloat;
	char sql[BUFLEN] = NULLSTR;

	// meta
	fprintf(mf, "\"%s\":{\n", tbl);
	fprintf(mf, "\"j\":\"%s\",\n", j);
	fprintf(mf, "\"c\":[\"attributes\",\"effects\"],\n");
	fprintf(mf, "\"k\":\"typeID\",\n");
	fprintf(mf, "\"m\":{\"attributes\":\"{ attributeID: value, ... }\",\"effects\":\"{ effectID: isDefault, ... }\"},\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"%s\":{\n", tbl);
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT typeID FROM dgmTypeEffects UNION SELECT typeID FROM dgmTypeAttributes ORDER BY 1", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		typeID = sql_column_long(stmt, 0);
		fprintf(f, "\"%ld\":[", typeID);

		// attributes
		fprintf(f, "{");
		rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC2, &substmt);
		if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC2, 1);
		SNPRINTF(sql, BUFLEN, "SELECT * FROM dgmTypeAttributes WHERE typeID = %ld ORDER BY attributeID\0", typeID);
		rc = SQLExecDirect(substmt, sql, SQL_NTS);
		if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, substmt, 1);
		rc = SQLFetch(substmt);
		while (SQL_SUCCEEDED(rc)) {
			fprintf(f, "\"%ld\":", sql_column_long(substmt, 1));
			valueInt = sql_column_long(substmt, 2);
			valueFloat = sql_column_float(substmt, 3);
			if (valueFloat != 0) fprintf(f, "%g", valueFloat);
			else fprintf(f, "%ld", valueInt);
			rc = SQLFetch(substmt);
			if (SQL_SUCCEEDED(rc)) fprintf(f, ",");
		}
		fprintf(f, "},");
		SQLFreeHandle(SQL_HANDLE_STMT, substmt);

		// effects
		fprintf(f, "{");
		rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC2, &substmt);
		if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC2, 1);
		SNPRINTF(sql, BUFLEN, "SELECT * FROM dgmTypeEffects WHERE typeID = %ld ORDER BY effectID\0", typeID);
		rc = SQLExecDirect(substmt, sql, SQL_NTS);
		if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, substmt, 1);
		rc = SQLFetch(substmt);
		while (SQL_SUCCEEDED(rc)) {
			fprintf(f, "\"%ld\":", sql_column_long(substmt, 1));
			fprintf(f, "%s", sql_column_long(substmt, 2) == 0 ? "false" : "true"); // isDefault
			rc = SQLFetch(substmt);
			if (SQL_SUCCEEDED(rc)) fprintf(f, ",");
		}
		fprintf(f, "},");
		SQLFreeHandle(SQL_HANDLE_STMT, substmt);

		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}
