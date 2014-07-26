#include <stdio.h>
#include <string.h>
#include "compat.h"

#include <sql.h>
#include <sqlext.h>

#include "sdd2json.h"

// all tables this creates
unsigned int create_crpActivities(FILE*, FILE*);
unsigned int create_crpNPCDivisions(FILE*, FILE*);
unsigned int create_crpNPCCorporations(FILE*, FILE*);
unsigned int create_agtAgentTypes(FILE*, FILE*);
unsigned int create_agtAgents(FILE*, FILE*);

unsigned int create_crp(FILE *mf) {
	int rc;
	const char *src;
	FILE *f = NULL;

	// crpData
	src = "crpData";
	if (!(f = open_datafile(src))) return 1;
	rc = create_crpActivities(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_crpNPCDivisions(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_crpNPCCorporations(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_agtAgentTypes(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_agtAgents(mf, f); if (0 != rc) return rc;
	if (close_datafile(src, f)) return 1;

	return 0;
}

unsigned int create_crpActivities(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[1001];

	// meta
	fprintf(mf, "\"crpActivities\":{\n");
	fprintf(mf, "\"j\":\"crpData\",\n");
	fprintf(mf, "\"c\":[\"activityName\",\"description\"],\n");
	fprintf(mf, "\"k\":\"activityID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"crpActivities\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM crpActivities ORDER BY activityID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf); // activityName
		sql_column_text(txtbuf, stmt, 2, 1001);
		fprintf(f, "\"%s\"", txtbuf); // description
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_crpNPCDivisions(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[1001];

	// meta
	fprintf(mf, "\"crpNPCDivisions\":{\n");
	fprintf(mf, "\"j\":\"crpData\",\n");
	fprintf(mf, "\"c\":[\"divisionName\",\"description\",\"leaderType\"],\n");
	fprintf(mf, "\"k\":\"divisionID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"crpNPCDivisions\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM crpNPCDivisions ORDER BY divisionID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf); // divisionName
		sql_column_text(txtbuf, stmt, 2, 1001);
		fprintf(f, "\"%s\",", txtbuf); // description
		sql_column_text(txtbuf, stmt, 3, 101);
		fprintf(f, "\"%s\"", txtbuf); // leaderType
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_crpNPCCorporations(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	SQLHSTMT substmt;
	SQLINTEGER ID = 0;
	char sql[BUFLEN] = NULLSTR;
	char txtbuf[4001];

	// meta
	fprintf(mf, "\"crpNPCCorporations\":{\n");
	fprintf(mf, "\"j\":\"crpData\",\n");
	fprintf(mf, "\"c\":[\"size\",\"extent\",\"solarSystemID\",\"investorID1\",\"investorShares1\",\"investorID2\",\"investorShares2\",\"investorID3\",\"investorShares3\",\"investorID4\",\"investorShares4\",\"friendID\",\"enemyID\",\"publicShares\",\"initialPrice\",\"minSecurity\",\"scattered\",\"fringe\",\"corridor\",\"hub\",\"border\",\"factionID\",\"sizeFactor\",\"stationCount\",\"stationSystemCount\",\"description\",\"iconID\",\"corpName\",\"divisions\",\"research\",\"trades\"],\n");
	fprintf(mf, "\"k\":\"corporationID\",\n");
	fprintf(mf, "\"m\":{\"divisions\":\"{ divisionID: divisionSize, ... }\",\"research\":\"[ skillID, ... ]\",\"trades\":\"[ typeID, ... ]\"},\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"crpNPCCorporations\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT c.*, i.itemName FROM crpNPCCorporations AS c LEFT OUTER JOIN invNames AS i ON c.corporationID = i.itemID ORDER BY c.corporationID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		ID = sql_column_long(stmt, 0);
		fprintf(f, "\"%ld\":[", ID);
		sql_column_text(txtbuf, stmt, 1, 2);
		fprintf(f, "\"%s\",", txtbuf); // size
		sql_column_text(txtbuf, stmt, 2, 2);
		fprintf(f, "\"%s\",", txtbuf); // extent
		fprintf(f, "%ld,", sql_column_long(stmt, 3)); // solarSystemID
		fprintf(f, "%ld,", sql_column_long(stmt, 4)); // investorID1
		fprintf(f, "%ld,", sql_column_long(stmt, 5)); // investorShares1
		fprintf(f, "%ld,", sql_column_long(stmt, 6)); // investorID2
		fprintf(f, "%ld,", sql_column_long(stmt, 7)); // investorShares2
		fprintf(f, "%ld,", sql_column_long(stmt, 8)); // investorID3
		fprintf(f, "%ld,", sql_column_long(stmt, 9)); // investorShares3
		fprintf(f, "%ld,", sql_column_long(stmt, 10)); // investorID4
		fprintf(f, "%ld,", sql_column_long(stmt, 11)); // investorShares4
		fprintf(f, "%ld,", sql_column_long(stmt, 12)); // friendID
		fprintf(f, "%ld,", sql_column_long(stmt, 13)); // enemyID
		fprintf(f, "%ld,", sql_column_long(stmt, 14)); // publicShares
		fprintf(f, "%ld,", sql_column_long(stmt, 15)); // initialPrice
		fprintf(f, "%g,", sql_column_float(stmt, 16)); // minSecurity
		fprintf(f, "%s,", sql_column_bit(stmt, 17) == 0 ? "false" : "true"); // scattered
		fprintf(f, "%ld,", sql_column_long(stmt, 18)); // fringe
		fprintf(f, "%ld,", sql_column_long(stmt, 19)); // corridor
		fprintf(f, "%ld,", sql_column_long(stmt, 20)); // hub
		fprintf(f, "%ld,", sql_column_long(stmt, 21)); // border
		fprintf(f, "%ld,", sql_column_long(stmt, 22)); // factionID
		fprintf(f, "%g,", sql_column_float(stmt, 23)); // sizeFactor
		fprintf(f, "%ld,", sql_column_long(stmt, 24)); // stationCount
		fprintf(f, "%ld,", sql_column_long(stmt, 25)); // stationSystemCount
		sql_column_text(txtbuf, stmt, 26, 4001);
		fprintf(f, "\"%s\",", txtbuf); // description
		fprintf(f, "%ld,", sql_column_long(stmt, 27)); // iconID
		sql_column_text(txtbuf, stmt, 28, 201);
		fprintf(f, "\"%s\",", txtbuf); // corpName

		// divisions
		fprintf(f, "{");
		rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC2, &substmt);
		if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC2, 1);
		SNPRINTF(sql, BUFLEN, "SELECT * FROM crpNPCCorporationDivisions WHERE corporationID = %ld ORDER BY divisionID\0", ID);
		rc = SQLExecDirect(substmt, sql, SQL_NTS);
		if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, substmt, 1);
		rc = SQLFetch(substmt);
		while (SQL_SUCCEEDED(rc)) {
			fprintf(f, "\"%ld\":", sql_column_long(substmt, 1));
			fprintf(f, "%ld", sql_column_long(substmt, 2));
			rc = SQLFetch(substmt);
			if (SQL_SUCCEEDED(rc)) fprintf(f, ",");
		}
		fprintf(f, "},");
		SQLFreeHandle(SQL_HANDLE_STMT, substmt);

		// research
		fprintf(f, "[");
		rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC2, &substmt);
		if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC2, 1);
		SNPRINTF(sql, BUFLEN, "SELECT * FROM crpNPCCorporationResearchFields WHERE corporationID = %ld ORDER BY skillID\0", ID);
		rc = SQLExecDirect(substmt, sql, SQL_NTS);
		if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, substmt, 1);
		rc = SQLFetch(substmt);
		while (SQL_SUCCEEDED(rc)) {
			fprintf(f, "%ld", sql_column_long(substmt, 0));
			rc = SQLFetch(substmt);
			if (SQL_SUCCEEDED(rc)) fprintf(f, ",");
		}
		fprintf(f, "],");
		SQLFreeHandle(SQL_HANDLE_STMT, substmt);

		// trades
		fprintf(f, "[");
		rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC2, &substmt);
		if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC2, 1);
		SNPRINTF(sql, BUFLEN, "SELECT * FROM crpNPCCorporationTrades WHERE corporationID = %ld ORDER BY typeID\0", ID);
		rc = SQLExecDirect(substmt, sql, SQL_NTS);
		if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, substmt, 1);
		rc = SQLFetch(substmt);
		while (SQL_SUCCEEDED(rc)) {
			fprintf(f, "%ld", sql_column_long(substmt, 1));
			rc = SQLFetch(substmt);
			if (SQL_SUCCEEDED(rc)) fprintf(f, ",");
		}
		fprintf(f, "]");
		SQLFreeHandle(SQL_HANDLE_STMT, substmt);

		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_agtAgentTypes(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[51];

	// meta
	fprintf(mf, "\"agtAgentTypes\":{\n");
	fprintf(mf, "\"j\":\"crpData\",\n");
	fprintf(mf, "\"c\":[\"agentType\"],\n");
	fprintf(mf, "\"k\":\"agentTypeID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"agtAgentTypes\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM agtAgentTypes ORDER BY agentTypeID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 51);
		fprintf(f, "\"%s\"", txtbuf); // agentType
		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_agtAgents(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	SQLHSTMT substmt;
	SQLINTEGER ID = 0;
	char sql[BUFLEN] = NULLSTR;

	// meta
	fprintf(mf, "\"agtAgents\":{\n");
	fprintf(mf, "\"j\":\"crpData\",\n");
	fprintf(mf, "\"c\":[\"divisionID\",\"corporationID\",\"locationID\",\"level\",\"quality\",\"agentTypeID\",\"isLocator\",\"research\"],\n");
	fprintf(mf, "\"k\":\"agentID\",\n");
	fprintf(mf, "\"m\":{\"research\":\"[ typeID, ... ]\"},\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"agtAgents\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM agtAgents ORDER BY agentID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		ID = sql_column_long(stmt, 0);
		fprintf(f, "\"%ld\":[", ID);
		fprintf(f, "%ld,", sql_column_long(stmt, 1)); // divisionID
		fprintf(f, "%ld,", sql_column_long(stmt, 2)); // corporationID
		fprintf(f, "%ld,", sql_column_long(stmt, 3)); // locationID
		fprintf(f, "%ld,", sql_column_long(stmt, 4)); // level
		fprintf(f, "%ld,", sql_column_long(stmt, 5)); // quality
		fprintf(f, "%ld,", sql_column_long(stmt, 6)); // agentTypeID
		fprintf(f, "%s,", sql_column_bit(stmt, 7) == 0 ? "false" : "true"); // isLocator

		// research
		fprintf(f, "[");
		rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC2, &substmt);
		if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC2, 1);
		SNPRINTF(sql, BUFLEN, "SELECT * FROM agtResearchAgents WHERE agentID = %ld ORDER BY typeID\0", ID);
		rc = SQLExecDirect(substmt, sql, SQL_NTS);
		if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, substmt, 1);
		rc = SQLFetch(substmt);
		while (SQL_SUCCEEDED(rc)) {
			fprintf(f, "%ld", sql_column_long(substmt, 1));
			rc = SQLFetch(substmt);
			if (SQL_SUCCEEDED(rc)) fprintf(f, ",");
		}
		fprintf(f, "]");
		SQLFreeHandle(SQL_HANDLE_STMT, substmt);

		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}
