#include <stdio.h>
#include <string.h>
#include "compat.h"

#include <sql.h>
#include <sqlext.h>

#include "sdd2json.h"

// all tables this creates
unsigned int create_staServices(const char*, FILE*, FILE*);
unsigned int create_staOperations(const char*, FILE*, FILE*);
unsigned int create_staStationTypes(const char*, FILE*, FILE*);
unsigned int create_staStations(const char*, FILE*, FILE*);

unsigned int create_sta(FILE *mf) {
	int rc;
	char *src;
	FILE *f = NULL;

	// staData
	src = "staData";
	if (!(f = open_datafile(src))) return 1;
	rc = create_staServices(src, mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_staOperations(src, mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_staStationTypes(src, mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_staStations(src, mf, f); if (0 != rc) return rc;
	if (close_datafile(src, f)) return 1;

	return 0;
}

unsigned int create_staServices(const char *j, FILE *mf, FILE *f) {
	const char *tbl = "staServices";
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[1001];

	// meta
	fprintf(mf, "\"%s\":{\n", tbl);
	fprintf(mf, "\"j\":\"%s\",\n", j);
	fprintf(mf, "\"c\":[\"serviceName\",\"description\"],\n");
	fprintf(mf, "\"k\":\"serviceID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"%s\":{\n", tbl);
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM staServices ORDER BY serviceID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf); // serviceName
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

unsigned int create_staOperations(const char *j, FILE *mf, FILE *f) {
	const char *tbl = "staOperations";
	SQLRETURN rc;
	SQLHSTMT stmt;
	SQLHSTMT substmt;
	SQLINTEGER activityID;
	SQLINTEGER operationID;
	char txtbuf[1001];
	char sql[BUFLEN] = NULLSTR;

	// meta
	fprintf(mf, "\"%s\":{\n", tbl);
	fprintf(mf, "\"j\":\"%s\",\n", j);
	fprintf(mf, "\"c\":[\"activityID\",\"operationName\",\"decsription\",\"fringe\",\"corridor\",\"hub\",\"border\",\"ratio\",\"caldariStationTypeID\",\"minmatarStationTypeID\",\"amarrStationTypeID\",\"gallenteStationTypeID\",\"joveStationTypeID\",\"services\"],\n");
	fprintf(mf, "\"k\":\"operationID\",\n");
	fprintf(mf, "\"m\":{\"services\":\"[ serviceID, ... ]\"},\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"%s\":{\n", tbl);
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM staOperations ORDER BY operationID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		activityID = sql_column_long(stmt, 0);
		operationID = sql_column_long(stmt, 1);
		fprintf(f, "\"%ld\":[", operationID);
		sql_column_text(txtbuf, stmt, 2, 101);
		fprintf(f, "\"%s\",", txtbuf); // operationName
		sql_column_text(txtbuf, stmt, 3, 1001);
		fprintf(f, "\"%s\",", txtbuf); // description
		fprintf(f, "%ld,", sql_column_long(stmt, 4)); // fringe
		fprintf(f, "%ld,", sql_column_long(stmt, 5)); // corridor
		fprintf(f, "%ld,", sql_column_long(stmt, 6)); // hub
		fprintf(f, "%ld,", sql_column_long(stmt, 7)); // border
		fprintf(f, "%ld,", sql_column_long(stmt, 8)); // ratio
		fprintf(f, "%ld,", sql_column_long(stmt, 9)); // caldariStationTypeID
		fprintf(f, "%ld,", sql_column_long(stmt, 10)); // minmatarStationTypeID
		fprintf(f, "%ld,", sql_column_long(stmt, 11)); // amarrStationTypeID
		fprintf(f, "%ld,", sql_column_long(stmt, 12)); // gallenteStationTypeID
		fprintf(f, "%ld,", sql_column_long(stmt, 13)); // joveStationTypeID

		// services
		fprintf(f, "[");
		rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC2, &substmt);
		if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC2, 1);
		SNPRINTF(sql, BUFLEN, "SELECT * FROM staOperationServices WHERE operationID = %ld ORDER BY serviceID\0", operationID);
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

unsigned int create_staStationTypes(const char *j, FILE *mf, FILE *f) {
	const char *tbl = "staStationTypes";
	SQLRETURN rc;
	SQLHSTMT stmt;

	// meta
	fprintf(mf, "\"%s\":{\n", tbl);
	fprintf(mf, "\"j\":\"%s\",\n", j);
	fprintf(mf, "\"c\":[\"dockEntryX\",\"dockEntryY\",\"dockEntryZ\",\"dockOrientationX\",\"dockOrientationY\",\"dockOrientationZ\",\"operationID\",\"officeSlots\",\"reprocessingEfficiency\",\"conquerable\"],\n");
	fprintf(mf, "\"k\":\"stationTypeID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"%s\":{\n", tbl);
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM staStationTypes ORDER BY stationTypeID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		fprintf(f, "%g,", sql_column_float(stmt, 1)); // dockEntryX
		fprintf(f, "%g,", sql_column_float(stmt, 2)); // dockEntryY
		fprintf(f, "%g,", sql_column_float(stmt, 3)); // dockEntryZ
		fprintf(f, "%g,", sql_column_float(stmt, 4)); // dockOrientationX
		fprintf(f, "%g,", sql_column_float(stmt, 5)); // dockOrientationY
		fprintf(f, "%g,", sql_column_float(stmt, 6)); // dockOrientationZ
		fprintf(f, "%ld,", sql_column_long(stmt, 7)); // operationID
		fprintf(f, "%ld,", sql_column_long(stmt, 8)); // officeSlots
		fprintf(f, "%g,", sql_column_float(stmt, 9)); // reprocessingEfficiency
		fprintf(f, "%s", sql_column_long(stmt, 10) == 0 ? "false" : "true"); // conquerable

		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}

unsigned int create_staStations(const char *j, FILE *mf, FILE *f) {
	const char *tbl = "staStations";
	SQLRETURN rc;
	SQLHSTMT stmt;
	char txtbuf[101];

	// meta
	fprintf(mf, "\"%s\":{\n", tbl);
	fprintf(mf, "\"j\":\"%s\",\n", j);
	fprintf(mf, "\"c\":[\"security\",\"dockingCostPerVolume\",\"maxShipVolumeDockable\",\"officeRentalCost\",\"operationID\",\"stationTypeID\",\"corporationID\",\"solarSystemID\",\"constellationID\",\"regionID\",\"stationName\",\"x\",\"y\",\"z\",\"reprocessingEfficiency\",\"reprocessingStationsTake\",\"reprocessingHangarFlag\"],\n");
	fprintf(mf, "\"k\":\"stationID\",\n");
	fprintf(mf, "\"t\":\"sql\"\n");
	fprintf(mf, "}");

	// data
	fprintf(f, "\"%s\":{\n", tbl);
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM staStations ORDER BY stationID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		fprintf(f, "%ld,", sql_column_long(stmt, 1)); // security
		fprintf(f, "%g,", sql_column_float(stmt, 2)); // dockingCostPerVolume
		fprintf(f, "%g,", sql_column_float(stmt, 3)); // maxShipVolumeDockable
		fprintf(f, "%ld,", sql_column_long(stmt, 4)); // officeRentalCost
		fprintf(f, "%ld,", sql_column_long(stmt, 5)); // operationID
		fprintf(f, "%ld,", sql_column_long(stmt, 6)); // stationTypeID
		fprintf(f, "%ld,", sql_column_long(stmt, 7)); // corporationID
		fprintf(f, "%ld,", sql_column_long(stmt, 8)); // solarSystemID
		fprintf(f, "%ld,", sql_column_long(stmt, 9)); // constellationID
		fprintf(f, "%ld,", sql_column_long(stmt, 10)); // regionID
		sql_column_text(txtbuf, stmt, 11, 101);
		fprintf(f, "\"%s\",", txtbuf); // stationName
		fprintf(f, "%g,", sql_column_float(stmt, 12)); // x
		fprintf(f, "%g,", sql_column_float(stmt, 13)); // y
		fprintf(f, "%g,", sql_column_float(stmt, 14)); // z
		fprintf(f, "%g,", sql_column_float(stmt, 15)); // reprocessingEfficiency
		fprintf(f, "%g,", sql_column_float(stmt, 16)); // reprocessingStationsTake
		fprintf(f, "%ld", sql_column_long(stmt, 17)); // reprocessingHangarFlag

		rc = SQLFetch(stmt);
		if (SQL_SUCCEEDED(rc)) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	fprintf(f, "}\n}");

	return 0;
}
