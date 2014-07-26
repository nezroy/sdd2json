#include <stdio.h>
#include <string.h>
#include "compat.h"

#include <sql.h>
#include <sqlext.h>

#include <yaml.h>

#include "sdd2json.h"

// all tables this creates
unsigned int create_invCategories(FILE*, FILE*);
unsigned int create_invContrabandTypes(FILE*, FILE*);
unsigned int create_invControlTowerResourcePurposes(FILE*, FILE*);
unsigned int create_invControlTowerResources(FILE*, FILE*);
unsigned int create_invFlags(FILE*, FILE*);
unsigned int create_invGroups(FILE*, FILE*);
unsigned int create_invIcons(FILE*, FILE*, const char*);
unsigned int create_invMarketGroups(FILE*, FILE*);
unsigned int create_invMetaGroups(FILE*, FILE*);
unsigned int create_invMetaTypes(FILE*, FILE*);
unsigned int create_invTypeMaterials(FILE*, FILE*);
unsigned int create_invTypeReactions(FILE*, FILE*);
unsigned int create_invTypes(FILE*, FILE*, yaml_document_t*, int*);
unsigned int create_invTypesDesc(FILE*, yaml_document_t*, int*);
unsigned int create_invItems(FILE*);

int create_invTypesDesc_segment(FILE*, unsigned int, unsigned int, SQLINTEGER*, SQLINTEGER*, yaml_document_t*, int*);
int create_invItems_segment(FILE*, unsigned int, unsigned int, SQLINTEGER*, SQLINTEGER*);

unsigned int create_inv(FILE *mf, const char *typeids_yaml, const char *iconids_yaml) {
	int rc = 0;
	char *src;
	FILE *f = NULL;
	FILE *f_yaml = NULL;
	int yret;
	yaml_node_t *node;
	yaml_node_t *bufnode;
	yaml_node_pair_t *pair;
	yaml_parser_t parser;
	yaml_document_t ydoc;
	int node_idxs[50000];
	int typeID;

	// parse typeids yaml into reverse lookup array of node indexes by type ID (crude... sue me)
	// init these
	for (int i = 0; i < 50000; i++) {
		node_idxs[i] = -1;
	}
	f_yaml = fopen(typeids_yaml, "rb");
	if (f_yaml == NULL) {
		printf("err\n");
		fprintf(stderr, "error opening types file\n");
		return 1;
	}
	yaml_parser_initialize(&parser);
	yaml_parser_set_input_file(&parser, f_yaml);
	yret = yaml_parser_load(&parser, &ydoc);
	if (!yret) {
		printf("err\n");
		fprintf(stderr, "error parsing types file\n");
		yaml_parser_delete(&parser);
		return 1;
	}

	// expect first node to be a mapping node for the whole document (typeIDs to mappings)
	node = yaml_document_get_root_node(&ydoc);
	if (!node || node->type != YAML_MAPPING_NODE) {
		printf("err\n");
		fprintf(stderr, "error parsing icons file - unexpected root node type\n");
		yaml_parser_delete(&parser);
		return 1;
	}
	for (pair = node->data.mapping.pairs.start; pair < node->data.mapping.pairs.top; pair++) {
		// get the key node for this mapping; should be the typeID
		bufnode = yaml_document_get_node(&ydoc, pair->key);
		if (bufnode->type != YAML_SCALAR_NODE) continue;
		typeID = atoi(bufnode->data.scalar.value);

		// get the value node for this mapping; should be another mapping
		bufnode = yaml_document_get_node(&ydoc, pair->value);
		if (bufnode->type != YAML_MAPPING_NODE) continue;
		node_idxs[typeID] = pair->value; // save off the idx of the value node for later use
	}

	// invMeta
	src = "invMeta";
	if (!(f = open_datafile(src))) return 1;
	rc = create_invCategories(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_invContrabandTypes(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_invControlTowerResourcePurposes(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_invControlTowerResources(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_invFlags(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_invGroups(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_invIcons(mf, f, iconids_yaml); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_invMarketGroups(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_invMetaGroups(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_invMetaTypes(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_invTypeMaterials(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_invTypeReactions(mf, f); if (0 != rc) return rc;
	if (close_datafile(src, f)) return 1;

	// invTypes
	src = "invTypes";
	if (!(f = open_datafile(src))) return 1;
	fprintf(mf, ",\n"); rc = create_invTypes(mf, f, &ydoc, node_idxs); if (0 != rc) return rc;
	if (close_datafile(src, f)) return 1;

	// invTypesDesc (segmented)
	fprintf(mf, ",\n"); rc = create_invTypesDesc(mf, &ydoc, node_idxs); if (0 != rc) return rc;

	// invItems (segmented)
	fprintf(mf, ",\n"); rc = create_invItems(mf); if (0 != rc) return rc;

	// close YAML parsing
	yaml_document_delete(&ydoc);
	yaml_parser_delete(&parser);
	fclose(f_yaml);

	return 0;
}

unsigned int create_invIcons(FILE *mf, FILE *f, const char* iconids_yaml) {
	FILE *f_yaml = NULL;
	int yret;
	yaml_node_t *node;
	yaml_node_t *bufnode;
	yaml_node_t *valnode;
	yaml_node_pair_t *pair;
	yaml_node_pair_t *bufpair;
	int iconID = 0;
	char *iconFile;
	char *description;
	int szterm = 0;
	yaml_parser_t parser;
	yaml_document_t ydoc;
	int first = 1;
	
	// meta
	fprintf(mf, "\"invIcons\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"iconFile\",\"description\"],\n");
	fprintf(mf, "\"k\":\"iconID\",\n");
	fprintf(mf, "\"t\":\"yaml\"\n");
	fprintf(mf, "}");

	// parse icons file
	f_yaml = fopen(iconids_yaml, "rb");
	if (f_yaml == NULL) {
		printf("err\n");
		fprintf(stderr, "error opening icons file\n");
		return 1;
	}
	yaml_parser_initialize(&parser);
	yaml_parser_set_input_file(&parser, f_yaml);
	yret = yaml_parser_load(&parser, &ydoc);
	if (!yret) {
		printf("err\n");
		fprintf(stderr, "error parsing icons file\n");
		yaml_parser_delete(&parser);
		return 1;
	}

	// expect first node to be a mapping node for the whole document (iconIDs to mappings)
	node = yaml_document_get_root_node(&ydoc);
	if (!node || node->type != YAML_MAPPING_NODE) {
		printf("err\n");
		fprintf(stderr, "error parsing icons file - unexpected root node type\n");
		yaml_parser_delete(&parser);
		return 1;
	}

	// data
	fprintf(f, "\"invIcons\":{\n");
	fprintf(f, "\"d\":{\n");
	for (pair = node->data.mapping.pairs.start; pair < node->data.mapping.pairs.top; pair++) {
		// get the key node for this mapping; should be the iconID
		bufnode = yaml_document_get_node(&ydoc, pair->key);
		if (bufnode->type != YAML_SCALAR_NODE) continue;
		iconID = atoi(bufnode->data.scalar.value);

		// get the value node for this mapping; should be another mapping
		bufnode = yaml_document_get_node(&ydoc, pair->value);
		if (bufnode->type != YAML_MAPPING_NODE) continue;
		for (bufpair = bufnode->data.mapping.pairs.start; bufpair < bufnode->data.mapping.pairs.top; bufpair++) {
			// get the key node for this mapping; test for iconFile
			valnode = yaml_document_get_node(&ydoc, bufpair->key);
			if (valnode->type != YAML_SCALAR_NODE) continue;
			if (strncmp(valnode->data.scalar.value, "iconFile", 8) == 0) {
				valnode = yaml_document_get_node(&ydoc, bufpair->value);
				if (valnode->type != YAML_SCALAR_NODE) continue;
				szterm = (sizeof(char) * valnode->data.scalar.length) + 1;
				iconFile = calloc(szterm, sizeof(char));
				memcpy(iconFile, valnode->data.scalar.value, szterm - 1);
			}
			else if (strncmp(valnode->data.scalar.value, "description", 11) == 0) {
				valnode = yaml_document_get_node(&ydoc, bufpair->value);
				if (valnode->type != YAML_SCALAR_NODE) continue;
				szterm = (sizeof(char) * valnode->data.scalar.length) + 1;
				description = calloc(szterm, sizeof(char));
				memcpy(description, valnode->data.scalar.value, szterm - 1);
			}
		}

		if (!first) fprintf(f, ",\n");
		first = 0;
		fprintf(f, "\"%d\":[\"%s\",\"%s\"]", iconID, iconFile, (description != NULL) ? description : "");
		free(iconFile); iconFile = NULL;
		free(description); description = NULL;
	}
	yaml_document_delete(&ydoc);
	yaml_parser_delete(&parser);
	fclose(f_yaml);
	
	fprintf(f, "\n}\n}");

	return 0;
}

unsigned int create_invCategories(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[3001];

	// meta
	fprintf(mf, "\"invCategories\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"categoryName\",\"description\",\"iconID\",\"published\"],\n");
	fprintf(mf, "\"k\":\"categoryID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"invCategories\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM invCategories ORDER BY categoryID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf);
		sql_column_text(txtbuf, stmt, 2, 3001);
		fprintf(f, "\"%s\",", txtbuf);
		fprintf(f, "%ld,", sql_column_long(stmt, 3));
		fprintf(f, "%s", sql_column_bit(stmt, 4) == 0 ? "false" : "true");
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_invContrabandTypes(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	SQLINTEGER prev_type;
	SQLINTEGER typeID;
	SQLINTEGER factionID;

	// meta
	fprintf(mf, "\"invContrabandTypes\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"standingLoss\",\"confiscateMinSec\",\"fineByValue\",\"attackMinSec\"],\n");
	fprintf(mf, "\"k\":\"typeID:factionID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"invContrabandTypes\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM invContrabandTypes ORDER BY typeID,factionID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	prev_type = 0;
	while (SQL_SUCCEEDED(rc)) {
		factionID = sql_column_long(stmt, 0);
		typeID = sql_column_long(stmt, 1);
		if (prev_type != typeID) {
			if (0 != prev_type) {
				fprintf(f, "\n},\n");
			}
			fprintf(f, "\"%ld\":{\n\"%ld\":[", typeID, factionID);
			prev_type = typeID;
		}
		else {
			fprintf(f, ",\n\"%ld\":[", factionID);
		}
		fprintf(f, "%g,", sql_column_float(stmt, 2));
		fprintf(f, "%g,", sql_column_float(stmt, 3));
		fprintf(f, "%g,", sql_column_float(stmt, 4));
		fprintf(f, "%g", sql_column_float(stmt, 5));
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "]");
		else fprintf(f, "]\n}\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_invControlTowerResourcePurposes(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[101];

	// meta
	fprintf(mf, "\"invControlTowerResourcePurposes\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"purposeText\"],\n");
	fprintf(mf, "\"k\":\"purpose\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"invControlTowerResourcePurposes\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM invControlTowerResourcePurposes ORDER BY purpose", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\"", txtbuf);
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_invControlTowerResources(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	SQLINTEGER prev_type;
	SQLINTEGER typeID;
	SQLINTEGER factionID;

	// meta
	fprintf(mf, "\"invControlTowerResources\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"purpose\",\"quantity\",\"minSecurityLevel\",\"factionID\"],\n");
	fprintf(mf, "\"k\":\"controlTowerTypeID:resourceTypeID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"invControlTowerResources\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM invControlTowerResources ORDER BY controlTowerTypeID,resourceTypeID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	prev_type = 0;
	while (SQL_SUCCEEDED(rc)) {
		typeID = sql_column_long(stmt, 0);
		factionID = sql_column_long(stmt, 1);
		if (prev_type != typeID) {
			if (0 != prev_type) {
				fprintf(f, "\n},\n");
			}
			fprintf(f, "\"%ld\":{\n\"%ld\":[", typeID, factionID);
			prev_type = typeID;
		}
		else {
			fprintf(f, ",\n\"%ld\":[", factionID);
		}
		fprintf(f, "%ld,", sql_column_long(stmt, 2));
		fprintf(f, "%ld,", sql_column_long(stmt, 3));
		fprintf(f, "%g,", sql_column_float(stmt, 4));
		fprintf(f, "%ld", sql_column_long(stmt, 5));
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "]");
		else fprintf(f, "]\n}\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_invFlags(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[201];

	// meta
	fprintf(mf, "\"invFlags\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"flagName\",\"flagText\",\"orderID\"],\n");
	fprintf(mf, "\"k\":\"flagID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"invFlags\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM invFlags ORDER BY flagID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 201);
		fprintf(f, "\"%s\",", txtbuf);
		sql_column_text(txtbuf, stmt, 2, 101);
		fprintf(f, "\"%s\",", txtbuf);
		fprintf(f, "%ld", sql_column_long(stmt, 3));
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_invGroups(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[3001];

	// meta
	fprintf(mf, "\"invGroups\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"categoryID\",\"groupName\",\"description\",\"iconID\",\"useBasePrice\",\"allowManufacture\",\"allowRecycler\",\"anchored\",\"anchorable\",\"fittableNonSingleton\",\"published\"],\n");
	fprintf(mf, "\"k\":\"groupID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"invGroups\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM invGroups ORDER BY groupID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		fprintf(f, "%ld,", sql_column_long(stmt, 1));
		sql_column_text(txtbuf, stmt, 2, 101);
		fprintf(f, "\"%s\",", txtbuf);
		sql_column_text(txtbuf, stmt, 3, 3001);
		fprintf(f, "\"%s\",", txtbuf);
		fprintf(f, "%ld,", sql_column_long(stmt, 4)); // iconID
		fprintf(f, "%s,", sql_column_bit(stmt, 5) == 0 ? "false" : "true");
		fprintf(f, "%s,", sql_column_bit(stmt, 6) == 0 ? "false" : "true");
		fprintf(f, "%s,", sql_column_bit(stmt, 7) == 0 ? "false" : "true");
		fprintf(f, "%s,", sql_column_bit(stmt, 8) == 0 ? "false" : "true"); // anchored
		fprintf(f, "%s,", sql_column_bit(stmt, 9) == 0 ? "false" : "true");
		fprintf(f, "%s,", sql_column_bit(stmt, 10) == 0 ? "false" : "true");
		fprintf(f, "%s", sql_column_bit(stmt, 11) == 0 ? "false" : "true");
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_invMarketGroups(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[3001];

	// meta
	fprintf(mf, "\"invMarketGroups\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"parentGroupID\",\"marketGroupName\",\"description\",\"iconID\",\"hasTypes\"],\n");
	fprintf(mf, "\"k\":\"marketGroupID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"invMarketGroups\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM invMarketGroups ORDER BY marketGroupID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		fprintf(f, "%ld,", sql_column_long(stmt, 1));
		sql_column_text(txtbuf, stmt, 2, 101);
		fprintf(f, "\"%s\",", txtbuf);
		sql_column_text(txtbuf, stmt, 3, 3001);
		fprintf(f, "\"%s\",", txtbuf);
		fprintf(f, "%ld,", sql_column_long(stmt, 4)); // iconID
		fprintf(f, "%s", sql_column_bit(stmt, 5) == 0 ? "false" : "true");
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_invMetaGroups(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[1001];

	// meta
	fprintf(mf, "\"invMetaGroups\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"metaGroupName\",\"description\",\"iconID\"],\n");
	fprintf(mf, "\"k\":\"metaGroupID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"invMetaGroups\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM invMetaGroups ORDER BY metaGroupID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf);
		sql_column_text(txtbuf, stmt, 2, 1001);
		fprintf(f, "\"%s\",", txtbuf);
		fprintf(f, "%ld", sql_column_long(stmt, 3)); // iconID
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_invMetaTypes(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;

	// meta
	fprintf(mf, "\"invMetaTypes\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"parentTypeID\",\"metaGroupID\"],\n");
	fprintf(mf, "\"k\":\"typeID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"invMetaTypes\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM invMetaTypes ORDER BY typeID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		fprintf(f, "%ld,", sql_column_long(stmt, 1));
		fprintf(f, "%ld", sql_column_long(stmt, 2));
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_invTypeMaterials(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	SQLINTEGER prev_type;
	SQLINTEGER typeID;
	SQLINTEGER factionID;

	// meta
	fprintf(mf, "\"invTypeMaterials\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"quantity\"],\n");
	fprintf(mf, "\"k\":\"typeID:materialTypeID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"invTypeMaterials\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM invTypeMaterials ORDER BY typeID,materialTypeID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	prev_type = 0;
	while (SQL_SUCCEEDED(rc)) {
		typeID = sql_column_long(stmt, 0);
		factionID = sql_column_long(stmt, 1);
		if (prev_type != typeID) {
			if (0 != prev_type) {
				fprintf(f, "\n},\n");
			}
			fprintf(f, "\"%ld\":{\n\"%ld\":[", typeID, factionID);
			prev_type = typeID;
		}
		else {
			fprintf(f, ",\n\"%ld\":[", factionID);
		}
		fprintf(f, "%ld", sql_column_long(stmt, 2));
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "]");
		else fprintf(f, "]\n}\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_invTypeReactions(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	SQLINTEGER prev_type;
	SQLINTEGER typeID;
	SQLINTEGER prev_reactionType = 0;
	SQLINTEGER reactionTypeID;
	SQLSMALLINT input;

	// meta
	fprintf(mf, "\"invTypeReactions\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"quantity\"],\n");
	fprintf(mf, "\"k\":\"typeID:reactionTypeID:input\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"invTypeReactions\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM invTypeReactions ORDER BY typeID,reactionTypeID,input", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	prev_type = 0;
	
	while (SQL_SUCCEEDED(rc)) {
		typeID = sql_column_long(stmt, 0);
		input = sql_column_bit(stmt, 1);
		reactionTypeID = sql_column_long(stmt, 2);
		if (prev_type != typeID) {
			if (0 != prev_type) {
				fprintf(f, "\n}\n},\n");
			}
			fprintf(f, "\"%ld\":{\n\"%ld\":{\n\"%d\":[", typeID, reactionTypeID, input);
			prev_type = typeID;
			prev_reactionType = reactionTypeID;
		}
		else if (prev_reactionType != reactionTypeID) {
			if (0 != prev_reactionType) {
				fprintf(f, "\n},\n");
			}
			fprintf(f, ",\n\"%ld\":{\n\"%d\":[", reactionTypeID, input);
			prev_reactionType = reactionTypeID;
		}
		else {
			fprintf(f, ",\n\"%ld\":[", input);
		}
		fprintf(f, "%ld", sql_column_long(stmt, 3));
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "]");
		else fprintf(f, "]\n}\n}\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_invItems(FILE *mf) {
	const char *tbl = "invItems";
	unsigned int segment = 0;
	unsigned int segsize = 15000;
	// char jsonfile[BUFLEN] = NULLSTR;
	int segrc;
	SQLINTEGER minID;
	SQLINTEGER maxID;
	// char *condition;
	FILE *sf = NULL;

	// meta
	fprintf(mf, "\"%s\":{\n", tbl);
	fprintf(mf, "\"c\":[\"typeID\",\"ownerID\",\"locationID\",\"flagID\",\"quantity\",\"itemName\",\"uniqueName\",\"groupID\",\"x\",\"y\",\"z\",\"yaw\",\"pitch\",\"roll\"],\n");
	fprintf(mf, "\"k\":\"itemID\",\n");
	fprintf(mf, "\"t\":\"sql\",\n");
	fprintf(mf, "\"s\":[\n"); // this table is segmented

	// segmented data
	printf("%s - segmented\n", tbl);
	do {
		if (!(sf = open_segment(tbl, segment))) return 1;
		if (0 != segment) fprintf(mf, ",\n");
		minID = -1;
		maxID = -1;
		segrc = create_invItems_segment(sf, segment, segsize, &minID, &maxID);
		if (close_segment(tbl, segment, sf)) return 1;
		fprintf(mf, "[\"%02u\",%ld,%ld]", segment++, minID, maxID);
	} while (segrc == segsize);
	if (segrc < 0) return -segrc;
	fprintf(mf, "\n]\n}");

	return 0;
}

int create_invItems_segment(FILE *f, unsigned int segment, unsigned int segsize, SQLINTEGER *minID, SQLINTEGER *maxID) {
	SQLHSTMT stmt = SQL_NULL_HSTMT;
	SQLRETURN rc = 0;
	char txtbuf[201] = NULLSTR;
	SQLINTEGER id;
	char sql[2 * BUFLEN] = NULLSTR;

	// create invItems segment
	SNPRINTF(sql, 2 * BUFLEN, "SELECT i.*, n.itemName, u.itemName as uniqueName, u.groupID, p.x, p.y, p.z, p.yaw, p.pitch, p.roll FROM invItems AS i LEFT OUTER JOIN invNames AS n ON i.itemID = n.itemID LEFT OUTER JOIN invUniqueNames AS u ON i.itemID = u.itemID LEFT OUTER JOIN invPositions AS p ON i.itemID = p.itemID ORDER BY itemID OFFSET %u ROWS FETCH NEXT %u ROWS ONLY\0", segment * segsize, segsize);
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return -dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, sql, SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return -dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	int count = 0;
	while (SQL_SUCCEEDED(rc)) {
		id = sql_column_long(stmt, 0);
		if (count == 0 && minID != NULL) *minID = id;
		fprintf(f, "\"%ld\":[", id);
		fprintf(f, "%ld,", sql_column_long(stmt, 1));
		fprintf(f, "%ld,", sql_column_long(stmt, 2));
		fprintf(f, "%ld,", sql_column_long(stmt, 3));
		fprintf(f, "%ld,", sql_column_long(stmt, 4));
		fprintf(f, "%ld,", sql_column_long(stmt, 5));
		sql_column_text(txtbuf, stmt, 6, 201);
		fprintf(f, "\"%s\",", txtbuf);
		sql_column_text(txtbuf, stmt, 7, 201);
		fprintf(f, "\"%s\",", txtbuf);
		fprintf(f, "%ld,", sql_column_long(stmt, 8));
		fprintf(f, "%g,", sql_column_float(stmt, 9));
		fprintf(f, "%g,", sql_column_float(stmt, 10));
		fprintf(f, "%g,", sql_column_float(stmt, 11));
		fprintf(f, "%g,", sql_column_float(stmt, 12));
		fprintf(f, "%g,", sql_column_float(stmt, 13));
		fprintf(f, "%g", sql_column_float(stmt, 14));
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else {
			if (maxID != NULL) *maxID = id;
			fprintf(f, "]\n");
		}
		if (0 == (count++ % 1000)) {
			fflush(f);
		}
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	return count;
}

unsigned int create_invTypes(FILE *mf, FILE *f, yaml_document_t *ydoc, int *node_idxs) {
	const char *tbl = "invTypes";
	char txtbuf[101] = NULLSTR;
	SQLHSTMT stmt = SQL_NULL_HSTMT;
	SQLRETURN rc = 0;
	yaml_node_t *node;
	yaml_node_t *bufnode;
	yaml_node_pair_t *pair;
	SQLINTEGER ID;
	int iconID;

	// meta
	fprintf(mf, "\"%s\":{\n", tbl);
	fprintf(mf, "\"j\":\"invTypes\",\n");
	fprintf(mf, "\"c\":[\"groupID\",\"typeName\",\"mass\",\"volume\",\"capacity\",\"portionSize\",\"raceID\",\"basePrice\",\"published\",\"marketGroupID\",\"chanceOfDuplicating\",\"iconID\"],\n");
	fprintf(mf, "\"k\":\"typeID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"%s\":{\n", tbl);
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM invTypes ORDER BY typeID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	int count = 0;
	while (SQL_SUCCEEDED(rc)) {
		ID = sql_column_long(stmt, 0);
		fprintf(f, "\"%ld\":[", ID);
		fprintf(f, "%ld,", sql_column_long(stmt, 1));
		sql_column_text(txtbuf, stmt, 2, 101);
		fprintf(f, "\"%s\",", txtbuf);
		// skip desc
		fprintf(f, "%g,", sql_column_float(stmt, 4)); // mass
		fprintf(f, "%g,", sql_column_float(stmt, 5));
		fprintf(f, "%g,", sql_column_float(stmt, 6));
		fprintf(f, "%ld,", sql_column_long(stmt, 7)); // portionSize
		fprintf(f, "%ld,", sql_column_long(stmt, 8));
		fprintf(f, "%g,", sql_column_float(stmt, 9)); // basePrice
		fprintf(f, "%s,", sql_column_bit(stmt, 10) == 0 ? "false" : "true");
		fprintf(f, "%ld,", sql_column_long(stmt, 11));
		fprintf(f, "%g,", sql_column_float(stmt, 12));

		iconID = 0;
		if (ID > 0 && ID < 50000) {
			// find this item in the node_idxs
			if (node_idxs[ID] > -1) {
				node = yaml_document_get_node(ydoc, node_idxs[ID]);
				if (node->type == YAML_MAPPING_NODE) {
					for (pair = node->data.mapping.pairs.start; pair < node->data.mapping.pairs.top; pair++) {
						// get the key for this mapping; test for iconID
						bufnode = yaml_document_get_node(ydoc, pair->key);
						if (bufnode->type != YAML_SCALAR_NODE) continue;
						if (strncmp(bufnode->data.scalar.value, "iconID", 6) != 0) continue;

						// this is the iconID pair, get its value
						bufnode = yaml_document_get_node(ydoc, pair->value);
						if (bufnode->type != YAML_SCALAR_NODE) break;
						iconID = atoi(bufnode->data.scalar.value);
					}
				}
			}
		}
		fprintf(f, "%d", iconID);

		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
		if (0 == (count++ % 1000)) {
			fflush(f);
		}
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_invTypesDesc(FILE *mf, yaml_document_t *ydoc, int *node_idxs) {
	const char *tbl = "invTypesDesc";
	unsigned int segment = 0;
	unsigned int segsize = 7500;
	int segrc;
	SQLINTEGER minID;
	SQLINTEGER maxID;
	FILE *sf = NULL;

	// meta
	fprintf(mf, "\"%s\":{\n", tbl);
	fprintf(mf, "\"c\":[\"description\",\"yamldata\"],\n");
	fprintf(mf, "\"k\":\"typeID\",\n");
	fprintf(mf, "\"t\":\"sql\",\n");
	fprintf(mf, "\"s\":[\n"); // this table is segmented

	// segmented data
	printf("%s - segmented\n", tbl);
	do {
		if (!(sf = open_segment(tbl, segment))) return 1;
		if (0 != segment) fprintf(mf, ",\n");
		minID = -1;
		maxID = -1;
		segrc = create_invTypesDesc_segment(sf, segment, segsize, &minID, &maxID, ydoc, node_idxs);
		if (close_segment(tbl, segment, sf)) return 1;
		fprintf(mf, "[\"%02u\",%ld,%ld]", segment++, minID, maxID);
	} while (segrc == segsize);
	if (segrc < 0) return -segrc;
	fprintf(mf, "\n]\n}");

	return 0;
}

int create_invTypesDesc_segment(FILE *f, unsigned int segment, unsigned int segsize, SQLINTEGER *minID, SQLINTEGER *maxID, yaml_document_t *ydoc, int *node_idxs) {
	char sql[BUFLEN] = NULLSTR;
	char txtbuf[3001] = NULLSTR;
	SQLHSTMT stmt = SQL_NULL_HSTMT;
	SQLRETURN rc = 0;

	// create invTypesDesc segment
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return -dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	SNPRINTF(sql, BUFLEN, "SELECT * FROM invTypes ORDER BY typeID OFFSET %u ROWS FETCH NEXT %u ROWS ONLY\0", segment * segsize, segsize);
	rc = SQLExecDirect(stmt, sql, SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return -dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	int count = 0;
	while (SQL_SUCCEEDED(rc)) {
		SQLINTEGER id = sql_column_long(stmt, 0);
		if (count == 0 && minID != NULL) *minID = id;
		fprintf(f, "\"%ld\":[", id);
		sql_column_text(txtbuf, stmt, 3, 3001);
		fprintf(f, "\"%s\",", txtbuf);

		// serialize YAML data here if avail
		if (id >= 0 && id < 50000 && node_idxs[id] > 0) yaml_json_mapping(f, ydoc, yaml_document_get_node(ydoc, node_idxs[id]));
		else fprintf(f, "null");

		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else {
			if (maxID != NULL) *maxID = id;
			fprintf(f, "]\n");
		}
		if (0 == (count++ % 1000)) {
			fflush(f);
		}
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	return count;
}
