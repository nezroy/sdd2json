#include <stdio.h>
#include <string.h>
#include "compat.h"

#include <sql.h>
#include <sqlext.h>

#include "sdd2json.h"
#include "inv.h"

unsigned int create_inv(const char *outdir, unsigned int schema, SQLHDBC db, FILE *mf) {
	int rc = 0;

	fprintf(mf, "\"invBlueprintTypes\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"parentBlueprintTypeID\",\"productTypeID\",\"productionTime\",\"techLevel\",\"researchProductivity\",\"researchMaterialTime\",\"researchCopyTime\",\"productivityModifier\",\"materialModifier\",\"wasteFactor\",\"maxProductionLimit\"],\n");
	fprintf(mf, "\"k\":\"blueprintTypeID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"invCategories\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"categoryName\",\"description\",\"iconID\",\"published\"],\n");
	fprintf(mf, "\"k\":\"categoryID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"invContrabandTypes\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"standingLoss\",\"confiscateMinSec\",\"fineByValue\",\"attackMinSec\"],\n");
	fprintf(mf, "\"k\":\"typeID:factionID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"invControlTowerResourcePurposes\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"purposeText\"],\n");
	fprintf(mf, "\"k\":\"purpose\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"invControlTowerResources\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"purpose\",\"quantity\",\"minSecurityLevel\",\"factionID\"],\n");
	fprintf(mf, "\"k\":\"controlTowerTypeID:resourceTypeID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"invFlags\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"flagName\",\"flagText\",\"orderID\"],\n");
	fprintf(mf, "\"k\":\"flagID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"invGroups\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"categoryID\",\"groupName\",\"description\",\"iconID\",\"useBasePrice\",\"allowManufacture\",\"allowRecycler\",\"anchored\",\"anchorable\",\"fittableNonSingleton\",\"published\"],\n");
	fprintf(mf, "\"k\":\"groupID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"invMarketGroups\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"parentGroupID\",\"marketGroupName\",\"description\",\"iconID\",\"hasTypes\"],\n");
	fprintf(mf, "\"k\":\"marketGroupID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"invMetaGroups\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"metaGroupName\",\"description\",\"iconID\"],\n");
	fprintf(mf, "\"k\":\"metaGroupID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"invMetaTypes\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"parentTypeID\",\"metaGroupID\"],\n");
	fprintf(mf, "\"k\":\"typeID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"invTypeMaterials\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"quantity\"],\n");
	fprintf(mf, "\"k\":\"typeID:materialTypeID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"invTypeReactions\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"quantity\"],\n");
	fprintf(mf, "\"k\":\"typeID:reactionTypeID:input\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"invTypes\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"groupID\",\"typeName\",\"description\",\"mass\",\"volume\",\"capacity\",\"portionSize\",\"raceID\",\"basePrice\",\"published\",\"marketGroupID\",\"chanceOfDuplicating\"],\n");
	fprintf(mf, "\"k\":\"typeID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"invItems\":{\n");
	fprintf(mf, "\"j\":\"invMeta\",\n");
	fprintf(mf, "\"c\":[\"typeID\",\"ownerID\",\"locationID\",\"flagID\",\"quantity\",\"itemName\",\"uniqueName\",\"groupID\",\"x\",\"y\",\"z\",\"yaw\",\"pitch\",\"roll\"],\n");
	fprintf(mf, "\"k\":\"itemID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}\n");
	rc = create_invMeta(outdir, schema, db);
	if (0 != rc) return rc;
	rc = post_file(outdir, "invMeta");
	if (0 != rc) return rc;

	return 0;
}

unsigned int create_invMeta(const char *outdir, unsigned int schema, SQLHDBC db) {
	char jsonfile[BUFLEN] = NULLSTR;
	char txtbuf[101] = NULLSTR;
	char descbuf[3001] = NULLSTR;
	SQLINTEGER prev_type;
	SQLINTEGER typeID;
	SQLINTEGER factionID;
	SQLHSTMT stmt = SQL_NULL_HSTMT;
	SQLRETURN rc = 0;
	FILE *f = NULL;

	// create invMeta
	strlcpy(jsonfile, outdir, BUFLEN);
	strlcat(jsonfile, "invMeta.json", BUFLEN);
	printf("creating %s - ", jsonfile);
	f = fopen(jsonfile, "w");
	if (f == NULL) {
		printf("err\n");
		fprintf(stderr, "error opening file");
		return 1;
	}
	fprintf(f, "{\n");
	fprintf(f, "\"formatID\":1,\n");
	fprintf(f, "\"schema\":%u,\n", schema);
	fprintf(f, "\"copy\":\"%s\",\n", CCPR);

	// invBlueprintTypes
	fprintf(f, "\"invBlueprintTypes\":{\n");
	fprintf(f, "\"d\":{\n");
	SQLAllocHandle(SQL_HANDLE_STMT, db, &stmt);
	rc = SQLExecDirect(stmt, "SELECT * FROM invBlueprintTypes ORDER BY blueprintTypeID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return 2;
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		fprintf(f, "%ld,", sql_column_long(stmt, 1));
		fprintf(f, "%ld,", sql_column_long(stmt, 2));
		fprintf(f, "%ld,", sql_column_long(stmt, 3));
		fprintf(f, "%ld,", sql_column_long(stmt, 4));
		fprintf(f, "%ld,", sql_column_long(stmt, 5));
		fprintf(f, "%ld,", sql_column_long(stmt, 6));
		fprintf(f, "%ld,", sql_column_long(stmt, 7));
		fprintf(f, "%ld,", sql_column_long(stmt, 8));
		fprintf(f, "%ld,", sql_column_long(stmt, 9));
		fprintf(f, "%ld,", sql_column_long(stmt, 10));
		fprintf(f, "%ld,", sql_column_long(stmt, 11));
		fprintf(f, "%ld", sql_column_long(stmt, 12));
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n");
	fprintf(f, "},\n"); // end of invBlueprintTypes
	fflush(f);

	// invCategories
	fprintf(f, "\"invCategories\":{\n");
	fprintf(f, "\"d\":{\n");
	SQLAllocHandle(SQL_HANDLE_STMT, db, &stmt);
	rc = SQLExecDirect(stmt, "SELECT * FROM invCategories ORDER BY categoryID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return 2;
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf);
		sql_column_text(descbuf, stmt, 2, 3001);
		fprintf(f, "\"%s\",", descbuf);
		fprintf(f, "%ld,", sql_column_long(stmt, 3));
		fprintf(f, "%s", sql_column_bit(stmt, 4) == 0 ? "false" : "true");
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n");
	fprintf(f, "},\n"); // end of invCategories
	fflush(f);

	// invContrabandTypes
	fprintf(f, "\"invContrabandTypes\":{\n");
	fprintf(f, "\"d\":{\n");
	SQLAllocHandle(SQL_HANDLE_STMT, db, &stmt);
	rc = SQLExecDirect(stmt, "SELECT * FROM invContrabandTypes ORDER BY typeID,factionID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return 2;
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
	fprintf(f, "}\n");
	fprintf(f, "},\n"); // end of invContrabandTypes
	fflush(f);	

	// invControlTowerResourcePurposes
	fprintf(f, "\"invControlTowerResourcePurposes\":{\n");
	fprintf(f, "\"d\":{\n");
	SQLAllocHandle(SQL_HANDLE_STMT, db, &stmt);
	rc = SQLExecDirect(stmt, "SELECT * FROM invControlTowerResourcePurposes ORDER BY purpose", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return 2;
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
	fprintf(f, "}\n");
	fprintf(f, "},\n"); // end of invControlTowerResourcePurposes
	fflush(f);

	// invControlTowerResources
	fprintf(f, "\"invControlTowerResources\":{\n");
	fprintf(f, "\"d\":{\n");
	SQLAllocHandle(SQL_HANDLE_STMT, db, &stmt);
	rc = SQLExecDirect(stmt, "SELECT * FROM invControlTowerResources ORDER BY controlTowerTypeID,resourceTypeID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return 2;
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
	fprintf(f, "}\n");
	fprintf(f, "},\n"); // end of invControlTowerResources
	fflush(f);

	// invFlags
	fprintf(f, "\"invFlags\":{\n");
	fprintf(f, "\"d\":{\n");
	SQLAllocHandle(SQL_HANDLE_STMT, db, &stmt);
	rc = SQLExecDirect(stmt, "SELECT * FROM invFlags ORDER BY flagID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return 2;
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(descbuf, stmt, 1, 201);
		fprintf(f, "\"%s\",", descbuf);
		sql_column_text(txtbuf, stmt, 2, 101);
		fprintf(f, "\"%s\",", txtbuf);
		fprintf(f, "%ld", sql_column_long(stmt, 3));
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n");
	fprintf(f, "},\n"); // end of invFlags
	fflush(f);

	// invGroups
	fprintf(f, "\"invGroups\":{\n");
	fprintf(f, "\"d\":{\n");
	SQLAllocHandle(SQL_HANDLE_STMT, db, &stmt);
	rc = SQLExecDirect(stmt, "SELECT * FROM invGroups ORDER BY groupID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return 2;
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		fprintf(f, "%ld,", sql_column_long(stmt, 1));
		sql_column_text(txtbuf, stmt, 2, 101);
		fprintf(f, "\"%s\",", txtbuf);
		sql_column_text(descbuf, stmt, 3, 3001);
		fprintf(f, "\"%s\",", descbuf);
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
	fprintf(f, "}\n");
	fprintf(f, "},\n"); // end of invGroups
	fflush(f);

	// invMarketGroups
	fprintf(f, "\"invMarketGroups\":{\n");
	fprintf(f, "\"d\":{\n");
	SQLAllocHandle(SQL_HANDLE_STMT, db, &stmt);
	rc = SQLExecDirect(stmt, "SELECT * FROM invMarketGroups ORDER BY marketGroupID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return 2;
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		fprintf(f, "%ld,", sql_column_long(stmt, 1));
		sql_column_text(txtbuf, stmt, 2, 101);
		fprintf(f, "\"%s\",", txtbuf);
		sql_column_text(descbuf, stmt, 3, 3001);
		fprintf(f, "\"%s\",", descbuf);
		fprintf(f, "%ld,", sql_column_long(stmt, 4)); // iconID
		fprintf(f, "%s", sql_column_bit(stmt, 5) == 0 ? "false" : "true");
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n");
	fprintf(f, "},\n"); // end of invMarketGroups
	fflush(f);

	// invMetaGroups
	fprintf(f, "\"invMetaGroups\":{\n");
	fprintf(f, "\"d\":{\n");
	SQLAllocHandle(SQL_HANDLE_STMT, db, &stmt);
	rc = SQLExecDirect(stmt, "SELECT * FROM invMetaGroups ORDER BY metaGroupID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return 2;
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf);
		sql_column_text(descbuf, stmt, 2, 1001);
		fprintf(f, "\"%s\",", descbuf);
		fprintf(f, "%ld", sql_column_long(stmt, 3)); // iconID
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n");
	fprintf(f, "},\n"); // end of invMetaGroups
	fflush(f);

	// invMetaTypes
	fprintf(f, "\"invMetaTypes\":{\n");
	fprintf(f, "\"d\":{\n");
	SQLAllocHandle(SQL_HANDLE_STMT, db, &stmt);
	rc = SQLExecDirect(stmt, "SELECT * FROM invMetaTypes ORDER BY typeID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return 2;
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
	fprintf(f, "}\n");
	fprintf(f, "},\n"); // end of invMetaTypes
	fflush(f);

	// invTypeMaterials
	fprintf(f, "\"invTypeMaterials\":{\n");
	fprintf(f, "\"d\":{\n");
	SQLAllocHandle(SQL_HANDLE_STMT, db, &stmt);
	rc = SQLExecDirect(stmt, "SELECT * FROM invTypeMaterials ORDER BY typeID,materialTypeID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return 2;
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
	fprintf(f, "}\n");
	fprintf(f, "},\n"); // end of invTypeMaterials
	fflush(f);

	// invTypeReactions
	fprintf(f, "\"invTypeReactions\":{\n");
	fprintf(f, "\"d\":{\n");
	SQLAllocHandle(SQL_HANDLE_STMT, db, &stmt);
	rc = SQLExecDirect(stmt, "SELECT * FROM invTypeReactions ORDER BY typeID,reactionTypeID,input", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return 2;
	rc = SQLFetch(stmt);
	prev_type = 0;
	SQLINTEGER prev_reactionType = 0;
	while (SQL_SUCCEEDED(rc)) {
		typeID = sql_column_long(stmt, 0);
		SQLSMALLINT input = sql_column_bit(stmt, 1);
		SQLINTEGER reactionTypeID = sql_column_long(stmt, 2);
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
	fprintf(f, "}\n");
	fprintf(f, "},\n"); // end of invTypeReactions
	fflush(f);

	// segment stuff
	printf("OK\n");
	unsigned int segment = 0;
	unsigned int segsize = 80000;
	int segrc = 0;

	// invTypes
	printf("creating invTypes - segmented\n");
	fprintf(f, "\"invTypes\":{\n");
	fprintf(f, "\"s\":[\n");
	segment = 0;
	segsize = 11000;
	do {
		if (0 != segment) fprintf(f, ",\n");
		SQLINTEGER minID;
		SQLINTEGER maxID;
		segrc = create_invTypes_segment(outdir, schema, db, segment, segsize, &minID, &maxID);
		fprintf(f, "[\"%02u\",%d,%d]", segment++, minID, maxID);
	} while (segrc == segsize);
	if (segrc < 0) return -segrc;
	fprintf(f, "\n]\n");
	fprintf(f, "},\n"); // end of invTypes
	fflush(f);

	// invItems
	printf("creating invItems - segmented\n");
	fprintf(f, "\"invItems\":{\n");
	fprintf(f, "\"s\":[\n");
	segment = 0;
	segsize = 25000;
	do {
		if (0 != segment) fprintf(f, ",\n");
		SQLINTEGER minID;
		SQLINTEGER maxID;
		segrc = create_invItems_segment(outdir, schema, db, segment, segsize, &minID, &maxID);
		fprintf(f, "[\"%02u\",%d,%d]", segment++, minID, maxID);
	} while (segrc == segsize);
	if (segrc < 0) return -segrc;
	fprintf(f, "\n]\n");
	fprintf(f, "}\n"); // end of invItems
	fflush(f);

	fprintf(f, "}\n");
	fclose(f);
	return 0;
}

int create_invTypes_segment(const char *outdir, unsigned int schema, sqlite3 *db, unsigned int segment, unsigned int segsize, SQLINTEGER *minID, SQLINTEGER *maxID) {
	char jsonfile[BUFLEN] = NULLSTR;
	char sql[BUFLEN] = NULLSTR;
	char txtbuf[101] = NULLSTR;
	char descbuf[3001] = NULLSTR;
	SQLHSTMT stmt = SQL_NULL_HSTMT;
	SQLRETURN rc = 0;
	FILE *f = NULL;

	// create invTypes segment
	SNPRINTF(jsonfile, BUFLEN, "%sinvTypes_%02u.json", outdir, segment);
	printf("\tcreating segment %02u - ", segment);
	f = fopen(jsonfile, "w");
	if (f == NULL) {
		printf("err\n");
		fprintf(stderr, "error opening file");
		return -1;
	}
	fprintf(f, "{\n");
	fprintf(f, "\"formatID\":1,\n");
	fprintf(f, "\"schema\":%u,\n", schema);
	fprintf(f, "\"copy\":\"%s\",\n", CCPR);

	fprintf(f, "\"d\":{\n");
	SQLAllocHandle(SQL_HANDLE_STMT, db, &stmt);
	SNPRINTF(sql, BUFLEN, "SELECT * FROM invTypes ORDER BY typeID OFFSET %u ROWS FETCH NEXT %u ROWS ONLY\0", segment * segsize, segsize);
	rc = SQLExecDirect(stmt, sql, SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return -2;
	rc = SQLFetch(stmt);
	int count = 0;
	while (SQL_SUCCEEDED(rc)) {
		SQLINTEGER id = sql_column_long(stmt, 0);
		if (count == 0 && minID != NULL) *minID = id;
		fprintf(f, "\"%ld\":[", id);
		fprintf(f, "%ld,", sql_column_long(stmt, 1));
		sql_column_text(txtbuf, stmt, 2, 101);
		fprintf(f, "\"%s\",", txtbuf);
		sql_column_text(descbuf, stmt, 3, 3001);
		fprintf(f, "\"%s\",", descbuf);
		fprintf(f, "%g,", sql_column_float(stmt, 4)); // mass
		fprintf(f, "%g,", sql_column_float(stmt, 5));
		fprintf(f, "%g,", sql_column_float(stmt, 6));
		fprintf(f, "%ld,", sql_column_long(stmt, 7)); // portionSize
		fprintf(f, "%ld,", sql_column_long(stmt, 8));
		fprintf(f, "%ld,", sql_column_long(stmt, 9));
		fprintf(f, "%g,", sql_column_float(stmt, 10)); // basePrice
		fprintf(f, "%s,", sql_column_bit(stmt, 11) == 0 ? "false" : "true");
		fprintf(f, "%ld,", sql_column_long(stmt, 12));
		fprintf(f, "%g", sql_column_float(stmt, 13));
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
	fprintf(f, "}\n");

	fprintf(f, "}\n");
	fclose(f);

	printf("OK\n");

	return count;
}

int create_invItems_segment(const char *outdir, unsigned int schema, sqlite3 *db, unsigned int segment, unsigned int segsize, SQLINTEGER *minID, SQLINTEGER *maxID) {
	char jsonfile[BUFLEN] = NULLSTR;
	char sql[2 * BUFLEN] = NULLSTR;
	char namebuf[201] = NULLSTR;
	sqlite3_stmt *stmt = NULL;
	int rc = 0;
	FILE *f = NULL;

	// create invItems segment
	SNPRINTF(jsonfile, BUFLEN, "%sinvItems_%02u.json", outdir, segment);
	printf("\tcreating segment %02u - ", segment);
	f = fopen(jsonfile, "w");
	if (f == NULL) {
		printf("err\n");
		fprintf(stderr, "error opening file");
		return -1;
	}
	fprintf(f, "{\n");
	fprintf(f, "\"formatID\":1,\n");
	fprintf(f, "\"schema\":%u,\n", schema);
	fprintf(f, "\"copy\":\"%s\",\n", CCPR);

	fprintf(f, "\"d\":{\n");
	SNPRINTF(sql, 2 * BUFLEN, "SELECT i.*, n.itemName, u.itemName as uniqueName, u.groupID, p.x, p.y, p.z, p.yaw, p.pitch, p.roll FROM invItems AS i LEFT OUTER JOIN invNames AS n ON i.itemID = n.itemID LEFT OUTER JOIN invUniqueNames AS u ON i.itemID = u.itemID LEFT OUTER JOIN invPositions AS p ON i.itemID = p.itemID ORDER BY itemID OFFSET %u ROWS FETCH NEXT %u ROWS ONLY\0", segment * segsize, segsize);
	SQLAllocHandle(SQL_HANDLE_STMT, db, &stmt);
	rc = SQLExecDirect(stmt, sql, SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return -2;
	rc = SQLFetch(stmt);
	int count = 0;
	while (SQL_SUCCEEDED(rc)) {
		SQLINTEGER id = sql_column_long(stmt, 0);
		if (count == 0 && minID != NULL) *minID = id;
		fprintf(f, "\"%ld\":[", id);
		fprintf(f, "%ld,", sql_column_long(stmt, 1));
		fprintf(f, "%ld,", sql_column_long(stmt, 2));
		fprintf(f, "%ld,", sql_column_long(stmt, 3));
		fprintf(f, "%ld,", sql_column_long(stmt, 4));
		fprintf(f, "%ld,", sql_column_long(stmt, 5));
		sql_column_text(namebuf, stmt, 6, 201);
		fprintf(f, "\"%s\",", namebuf);
		sql_column_text(namebuf, stmt, 7, 201);
		fprintf(f, "\"%s\",", namebuf);
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
	fprintf(f, "}\n");

	fprintf(f, "}\n");
	fclose(f);

	printf("OK\n");
	SNPRINTF(jsonfile, BUFLEN, "invItems_%02u", segment);
	post_file(outdir, jsonfile);

	return count;
}
