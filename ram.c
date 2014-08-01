#include <stdio.h>
#include <string.h>
#include "compat.h"

#include <sql.h>
#include <sqlext.h>

#include <yaml.h>

#include "sdd2json.h"

// all tables this creates
unsigned int create_ramBlueprints(FILE*, FILE*, const char*);
unsigned int create_ramActivities(FILE*, FILE*);
unsigned int create_ramAssemblyLineStations(FILE*, FILE*);
unsigned int create_ramAssemblyLineTypes(FILE*, FILE*);
unsigned int create_ramInstallationTypeContents(FILE*, FILE*);

unsigned int create_ram(FILE *mf, const char *blueprints_yaml) {
	int rc;
	char *src;
	FILE *f = NULL;

	// ramData
	src = "ramData";
	if (!(f = open_datafile(src))) return 1;
	rc = create_ramActivities(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_ramAssemblyLineStations(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_ramAssemblyLineTypes(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_ramInstallationTypeContents(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_ramBlueprints(mf, f, blueprints_yaml); if (0 != rc) return rc;
	if (close_datafile(src, f)) return 1;

	return 0;
}

unsigned int create_ramActivities(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	unsigned int count = 0;
	char txtbuf[1001];

	// meta
	fprintf(mf, "\"ramActivities\":{\n");
	fprintf(mf, "\"j\":\"ramData\",\n");
	fprintf(mf, "\"c\":[\"activityName\",\"iconNo\",\"description\",\"published\"],\n");
	fprintf(mf, "\"k\":\"activityID\",\n");
	fprintf(mf, "\"t\":\"sql\",\n");

	// data
	fprintf(f, "\"ramActivities\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM ramActivities ORDER BY activityID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf);
		sql_column_text(txtbuf, stmt, 2, 6);
		fprintf(f, "\"%s\",", txtbuf);
		sql_column_text(txtbuf, stmt, 3, 1001);
		fprintf(f, "\"%s\",", txtbuf);
		fprintf(f, "%s", sql_column_bit(stmt, 4) == 0 ? "false" : "true");
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
		count++;
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	fprintf(mf, "\"l\":%u\n}", count);

	return 0;
}

unsigned int create_ramAssemblyLineStations(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	SQLINTEGER prev_station;
	SQLINTEGER stationID;
	SQLINTEGER lineTypeID;
	unsigned int count = 0;

	// meta
	fprintf(mf, "\"ramAssemblyLineStations\":{\n");
	fprintf(mf, "\"j\":\"ramData\",\n");
	fprintf(mf, "\"c\":[\"quantity\",\"stationTypeID\",\"ownerID\",\"solarSystemID\",\"regionID\"],\n");
	fprintf(mf, "\"k\":\"stationID:assemblyLineTypeID\",\n");
	fprintf(mf, "\"t\":\"sql\",\n");

	// data
	fprintf(f, "\"ramAssemblyLineStations\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM ramAssemblyLineStations ORDER BY stationID,assemblyLineTypeID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	prev_station = 0;
	while (SQL_SUCCEEDED(rc)) {
		stationID = sql_column_long(stmt, 0);
		lineTypeID = sql_column_long(stmt, 1);
		if (prev_station != stationID) {
			if (0 != prev_station) {
				fprintf(f, "\n},\n");
			}
			fprintf(f, "\"%ld\":{\n\"%ld\":[", stationID, lineTypeID);
			prev_station = stationID;
			count++;
		}
		else {
			fprintf(f, ",\n\"%ld\":[", lineTypeID);
		}
		fprintf(f, "%ld,", sql_column_long(stmt, 2)); // quantity
		fprintf(f, "%ld,", sql_column_long(stmt, 3)); // stationTypeID
		fprintf(f, "%ld,", sql_column_long(stmt, 4)); // ownerID
		fprintf(f, "%ld,", sql_column_long(stmt, 5)); // solarSystemID
		fprintf(f, "%ld", sql_column_long(stmt, 6)); // regionID
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "]");
		else fprintf(f, "]\n}\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	fprintf(mf, "\"l\":%u\n}", count);

	return 0;
}

unsigned int create_ramAssemblyLineTypes(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	SQLHSTMT substmt;
	SQLINTEGER ID = 0;
	unsigned int count = 0;
	char sql[BUFLEN] = NULLSTR;
	char txtbuf[1001];

	// meta
	fprintf(mf, "\"ramAssemblyLineTypes\":{\n");
	fprintf(mf, "\"j\":\"ramData\",\n");
	fprintf(mf, "\"c\":[\"assemblyLineTypeName\",\"description\",\"baseTimeMultiplier\",\"baseMaterialMultiplier\",\"baseCostMultiplier\",\"volume\",\"activityID\",\"minCostPerHour\",\"categoryMulti\",\"groupMulti\"],\n");
	fprintf(mf, "\"k\":\"assemblyLineTypeID\",\n");
	fprintf(mf, "\"m\":{\"categoryMulti\":\"{ categoryID: [timeMultiplier, materialMultiplier, costMultiplier], ... }\",\"groupMulti\":\"{ groupID: [timeMultiplier, materialMultiplier, costMultiplier], ... }\"},\n");
	fprintf(mf, "\"t\":\"sql\",\n");

	// data
	fprintf(f, "\"ramAssemblyLineTypes\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM ramAssemblyLineTypes ORDER BY assemblyLineTypeID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		ID = sql_column_long(stmt, 0);
		fprintf(f, "\"%ld\":[", ID);
		sql_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf); // assemblyLineTypeName
		sql_column_text(txtbuf, stmt, 2, 1001);
		fprintf(f, "\"%s\",", txtbuf); // description
		fprintf(f, "%g,", sql_column_float(stmt, 3)); // baseTimeMultiplier
		fprintf(f, "%g,", sql_column_float(stmt, 4)); // baseMaterialMultiplier
		fprintf(f, "%g,", sql_column_float(stmt, 5)); // baseCostMultiplier
		fprintf(f, "%g,", sql_column_float(stmt, 6)); // volume
		fprintf(f, "%ld,", sql_column_long(stmt, 7)); // activityID
		fprintf(f, "%g,", sql_column_float(stmt, 8)); // minCostPerHour

		// category
		fprintf(f, "{");
		rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC2, &substmt);
		if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC2, 1);
		SNPRINTF(sql, BUFLEN, "SELECT * FROM ramAssemblyLineTypeDetailPerCategory WHERE assemblyLineTypeID = %ld ORDER BY categoryID\0", ID);
		rc = SQLExecDirect(substmt, sql, SQL_NTS);
		if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, substmt, 1);
		rc = SQLFetch(substmt);
		while (SQL_SUCCEEDED(rc)) {
			fprintf(f, "\"%ld\":[", sql_column_long(substmt, 1));
			fprintf(f, "%g,", sql_column_float(substmt, 2));
			fprintf(f, "%g,", sql_column_float(substmt, 3));
			fprintf(f, "%g]", sql_column_float(substmt, 4));
			rc = SQLFetch(substmt);
			if (SQL_SUCCEEDED(rc)) fprintf(f, ",");
		}
		fprintf(f, "},");
		SQLFreeHandle(SQL_HANDLE_STMT, substmt);

		// group
		fprintf(f, "{");
		rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC2, &substmt);
		if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC2, 1);
		SNPRINTF(sql, BUFLEN, "SELECT * FROM ramAssemblyLineTypeDetailPerGroup WHERE assemblyLineTypeID = %ld ORDER BY groupID\0", ID);
		rc = SQLExecDirect(substmt, sql, SQL_NTS);
		if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, substmt, 1);
		rc = SQLFetch(substmt);
		while (SQL_SUCCEEDED(rc)) {
			fprintf(f, "\"%ld\":[", sql_column_long(substmt, 1));
			fprintf(f, "%g,", sql_column_float(substmt, 2));
			fprintf(f, "%g,", sql_column_float(substmt, 3));
			fprintf(f, "%g]", sql_column_float(substmt, 4));
			rc = SQLFetch(substmt);
			if (SQL_SUCCEEDED(rc)) fprintf(f, ",");
		}
		fprintf(f, "}");
		SQLFreeHandle(SQL_HANDLE_STMT, substmt);

		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
		count++;
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	fprintf(mf, "\"l\":%u\n}", count);

	return 0;
}

unsigned int create_ramInstallationTypeContents(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	SQLINTEGER prev_type;
	SQLINTEGER typeID;
	SQLINTEGER lineTypeID;
	unsigned int count = 0;

	// meta
	fprintf(mf, "\"ramInstallationTypeContents\":{\n");
	fprintf(mf, "\"j\":\"ramData\",\n");
	fprintf(mf, "\"c\":[\"quantity\"],\n");
	fprintf(mf, "\"k\":\"typeID:assemblyLineTypeID\",\n");
	fprintf(mf, "\"t\":\"sql\",\n");

	// data
	fprintf(f, "\"ramInstallationTypeContents\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM ramInstallationTypeContents ORDER BY installationTypeID,assemblyLineTypeID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	prev_type = 0;
	while (SQL_SUCCEEDED(rc)) {
		typeID = sql_column_long(stmt, 0);
		lineTypeID = sql_column_long(stmt, 1);
		if (prev_type != typeID) {
			if (0 != prev_type) {
				fprintf(f, "\n},\n");
			}
			fprintf(f, "\"%ld\":{\n\"%ld\":[", typeID, lineTypeID);
			prev_type = typeID;
			count++;
		}
		else {
			fprintf(f, ",\n\"%ld\":[", lineTypeID);
		}
		fprintf(f, "%ld", sql_column_long(stmt, 2)); // quantity
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "]");
		else fprintf(f, "]\n}\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	fprintf(mf, "\"l\":%u\n}", count);

	return 0;
}

unsigned int create_ramBlueprints(FILE *mf, FILE *f, const char *yaml) {
	FILE *f_yaml = NULL;
	int yret;
	unsigned int count = 0;
	yaml_node_t *node;
	yaml_node_t *bufnode;
	yaml_node_t *valnode;
	int actnode_idx;
	yaml_node_pair_t *pair;
	yaml_node_pair_t *bufpair;
	yaml_parser_t parser;
	yaml_document_t ydoc;
	int bpTypeID = 0;
	int bpTypeIDcheck = 0;
	int prodLimit = 0;

	// meta
	fprintf(mf, "\"ramBlueprints\":{\n");
	fprintf(mf, "\"j\":\"ramData\",\n");
	fprintf(mf, "\"c\":[\"maxProductionLimit\",\"activities\"],\n");
	fprintf(mf, "\"k\":\"blueprintTypeID\",\n");
	fprintf(mf, "\"m\":{\"activities\":\"{ activityID: { #see blueprints.yaml# }, ... }\"},\n");
	fprintf(mf, "\"t\":\"yaml\",\n");

	// open yaml for parsing
	f_yaml = fopen(yaml, "rb");
	if (f_yaml == NULL) {
		printf("err\n");
		fprintf(stderr, "error opening blueprints file\n");
		return 1;
	}
	yaml_parser_initialize(&parser);
	yaml_parser_set_input_file(&parser, f_yaml);
	yret = yaml_parser_load(&parser, &ydoc);
	if (!yret) {
		printf("err\n");
		fprintf(stderr, "error parsing blueprints file\n");
		yaml_parser_delete(&parser);
		return 1;
	}

	// ramBlueprints
	fprintf(f, "\"ramBlueprints\":{\n");
	fprintf(f, "\"d\":{\n");

	// expect first node to be a mapping node for the whole document (blueprintTypeIDs to mappings)
	node = yaml_document_get_root_node(&ydoc);
	if (!node || node->type != YAML_MAPPING_NODE) {
		printf("err\n");
		fprintf(stderr, "error parsing blueprints yaml - unexpected root node type\n");
		yaml_parser_delete(&parser);
		return 1;
	}
	for (pair = node->data.mapping.pairs.start; pair < node->data.mapping.pairs.top; pair++) {
		// get the key node for this mapping; should be the blueprintTypeID
		bufnode = yaml_document_get_node(&ydoc, pair->key);
		if (bufnode->type != YAML_SCALAR_NODE) continue;
		bpTypeID = atoi(bufnode->data.scalar.value);

		// get the value node for this mapping; should be another mapping
		bufnode = yaml_document_get_node(&ydoc, pair->value);
		if (bufnode->type != YAML_MAPPING_NODE) continue;
		actnode_idx = 0;
		bpTypeIDcheck = 0;
		prodLimit = 0;
		for (bufpair = bufnode->data.mapping.pairs.start; bufpair < bufnode->data.mapping.pairs.top; bufpair++) {
			// get the key node for this mapping; test for a few things to handle
			valnode = yaml_document_get_node(&ydoc, bufpair->key);
			if (valnode->type != YAML_SCALAR_NODE) continue;
			if (strncmp(valnode->data.scalar.value, "activities", 10) == 0) actnode_idx = bufpair->value;
			else if (strncmp(valnode->data.scalar.value, "blueprintTypeID", 15) == 0) {
				valnode = yaml_document_get_node(&ydoc, bufpair->value);
				if (!valnode || valnode->type != YAML_SCALAR_NODE) continue;
				bpTypeIDcheck = atoi(valnode->data.scalar.value);
			}
			else if (strncmp(valnode->data.scalar.value, "maxProductionLimit", 18) == 0) {
				valnode = yaml_document_get_node(&ydoc, bufpair->value);
				if (!valnode || valnode->type != YAML_SCALAR_NODE) continue;
				prodLimit = atoi(valnode->data.scalar.value);
			}
		}

		if (bpTypeIDcheck != bpTypeID) {
			printf("err\n");
			fprintf(stderr, "error parsing blueprints yaml - typeID mismatch: %d != %d\n", bpTypeIDcheck, bpTypeID);
			yaml_parser_delete(&parser);
			return 1;
		}

		if (count) fprintf(f, ",\n");
		count++;
		fprintf(f, "\"%d\":[", bpTypeID);
		fprintf(f, "%d,", prodLimit);
		if (actnode_idx) yaml_json_mapping(f, &ydoc, yaml_document_get_node(&ydoc, actnode_idx));
		else fprintf(f, "null");
		fprintf(f, "]");
	}
	yaml_document_delete(&ydoc);
	yaml_parser_delete(&parser);
	fclose(f_yaml);
	fprintf(f, "}\n}");

	fprintf(mf, "\"l\":%u\n}", count);

	return 0;
}
