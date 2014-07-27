#include <stdio.h>
#include <string.h>
#include "compat.h"

#include <sql.h>
#include <sqlext.h>
#include <sqlite3.h>

#include "sdd2json.h"

#define CONT_SIZ 50000
#define CONT_OFF 30000000
#define JITA_ID 30000142

void prepare_contiguous();
void follow_and_mark(unsigned int, unsigned int**);
unsigned short CONT_FLAG[CONT_SIZ];

// all tables this creates
unsigned int create_mapLandmarks(FILE*, FILE*);
unsigned int create_mapRegions(FILE*, FILE*, char);
unsigned int create_mapConstellations(FILE*, FILE*, char);
unsigned int create_mapSolarSystemJumps(FILE*, FILE*, char);
unsigned int create_mapSolarSystems(FILE*, FILE*, char);
unsigned int create_mapRegionJumps(FILE*, FILE*);
unsigned int create_mapConstellationJumps(FILE*, FILE*);
unsigned int create_mapJumps(FILE*, FILE*);
unsigned int create_warCombatZoneSystems(FILE*, FILE*);
unsigned int create_warCombatZones(FILE*, FILE*);
unsigned int create_mapCelestials(FILE*, FILE*, char);
unsigned int create_mapDenorm(FILE*, char, const char*);
unsigned int create_mapCelestialStatistics(FILE*, char);

int create_mapDenorm_segment(FILE*, unsigned int, unsigned int, sqlite3_int64*, sqlite3_int64*, const char*, char);
int create_mapCelest_segment(FILE*, unsigned int, unsigned int, sqlite3_int64*, sqlite3_int64*, char);

unsigned int create_map(FILE *mf) {
	int rc;
	char *src;
	FILE *f = NULL;

	prepare_contiguous();

	// mapKSpace
	src = "mapKSpace";
	if (!(f = open_datafile(src))) return 1;
	rc = create_mapLandmarks(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_mapRegions(mf, f, 'K'); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_mapConstellations(mf, f, 'K'); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_mapSolarSystemJumps(mf, f, 'K'); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_mapSolarSystems(mf, f, 'K'); if (0 != rc) return rc;
	if (close_datafile(src, f)) return 1;

	// mapJSpace
	src = "mapJSpace";
	if (!(f = open_datafile(src))) return 1;
	fprintf(mf, ",\n"); rc = create_mapRegions(mf, f, 'J'); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_mapConstellations(mf, f, 'J'); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_mapSolarSystemJumps(mf, f, 'J'); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_mapSolarSystems(mf, f, 'J'); if (0 != rc) return rc;
	if (close_datafile(src, f)) return 1;

	// mapWSpace
	src = "mapWSpace";
	if (!(f = open_datafile(src))) return 1;
	fprintf(mf, ",\n"); rc = create_mapRegions(mf, f, 'W'); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_mapConstellations(mf, f, 'W'); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_mapSolarSystems(mf, f, 'W'); if (0 != rc) return rc;
	if (close_datafile(src, f)) return 1;

	// mapJumps
	src = "mapJumps";
	if (!(f = open_datafile(src))) return 1;
	fprintf(mf, ",\n"); rc = create_mapRegionJumps(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_mapConstellationJumps(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_mapJumps(mf, f); if (0 != rc) return rc;
	if (close_datafile(src, f)) return 1;

	// mapKCelestials
	src = "mapKCelestials";
	if (!(f = open_datafile(src))) return 1;
	fprintf(mf, ",\n"); rc = create_warCombatZoneSystems(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_warCombatZones(mf, f); if (0 != rc) return rc;
	TBLSEP(f, mf); rc = create_mapCelestials(mf, f, 'K'); if (0 != rc) return rc;
	if (close_datafile(src, f)) return 1;

	// mapKCelestialStatistics and celestial denorms (all segmented)
	fprintf(mf, ",\n"); rc = create_mapCelestialStatistics(mf, 'K'); if (0 != rc) return rc;
	fprintf(mf, ",\n"); rc = create_mapDenorm(mf, 'K', "Planets"); if (0 != rc) return rc;
	fprintf(mf, ",\n"); rc = create_mapDenorm(mf, 'K', "Belts"); if (0 != rc) return rc;
	fprintf(mf, ",\n"); rc = create_mapDenorm(mf, 'K', "Moons"); if (0 != rc) return rc;
	fprintf(mf, ",\n"); rc = create_mapDenorm(mf, 'K', "Gates"); if (0 != rc) return rc;

	// mapJCelestials
	src = "mapJCelestials";
	if (!(f = open_datafile(src))) return 1;
	fprintf(mf, ",\n"); rc = create_mapCelestials(mf, f, 'J'); if (0 != rc) return rc;
	if (close_datafile(src, f)) return 1;

	// mapJCelestialStatistics and celestial denorms (all segmented)
	fprintf(mf, ",\n"); rc = create_mapCelestialStatistics(mf, 'J'); if (0 != rc) return rc;
	fprintf(mf, ",\n"); rc = create_mapDenorm(mf, 'J', "Planets"); if (0 != rc) return rc;
	fprintf(mf, ",\n"); rc = create_mapDenorm(mf, 'J', "Belts"); if (0 != rc) return rc;
	fprintf(mf, ",\n"); rc = create_mapDenorm(mf, 'J', "Moons"); if (0 != rc) return rc;
	fprintf(mf, ",\n"); rc = create_mapDenorm(mf, 'J', "Gates"); if (0 != rc) return rc;

	// mapWCelestials
	src = "mapWCelestials";
	if (!(f = open_datafile(src))) return 1;
	fprintf(mf, ",\n"); rc = create_mapCelestials(mf, f, 'W'); if (0 != rc) return rc;
	if (close_datafile(src, f)) return 1;

	// mapWCelestialStatistics and celestial denorms (all segmented)
	fprintf(mf, ",\n"); rc = create_mapCelestialStatistics(mf, 'W'); if (0 != rc) return rc;
	fprintf(mf, ",\n"); rc = create_mapDenorm(mf, 'W', "Planets"); if (0 != rc) return rc;
	fprintf(mf, ",\n"); rc = create_mapDenorm(mf, 'W', "Moons"); if (0 != rc) return rc;

	return 0;
}

unsigned int create_mapLandmarks(FILE *mf, FILE *f) {
	int rc = 0;
	sqlite3_stmt *stmt = NULL;
	unsigned int count = 0;
	char txtbuf[7001];

	// meta
	fprintf(mf, "\"mapLandmarks\":{\n");
	fprintf(mf, "\"j\":\"mapKSpace\",\n");
	fprintf(mf, "\"c\":[\"landmarkName\",\"description\",\"locationID\",\"x\",\"y\",\"z\",\"iconID\"],\n");
	fprintf(mf, "\"k\":\"landmarkID\",\n");
	fprintf(mf, "\"t\":\"db3\",\n");

	// data
	fprintf(f, "\"mapLandmarks\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = sqlite3_prepare_v2(DB3_UD, "SELECT * FROM mapLandmarks ORDER BY landmarkID", -1, &stmt, NULL);
	if (SQLITE_OK != rc) return dump_db3_error(DB3_UD, 1);
	rc = sqlite3_step(stmt);
	while (SQLITE_ROW == rc) {
		fprintf(f, "\"%lld\":[", sqlite3_column_int64(stmt, 0));
		db3_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf);
		db3_column_text(txtbuf, stmt, 2, 7001);
		fprintf(f, "\"%s\",", txtbuf);
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 3));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 4));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 5));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 6));
		fprintf(f, "%lld", sqlite3_column_int64(stmt, 7));
		rc = sqlite3_step(stmt);
		if (SQLITE_ROW == rc) fprintf(f, "],\n");
		else fprintf(f, "]\n");
		count++;
	}
	rc = sqlite3_finalize(stmt);
	fprintf(f, "}\n}");

	fprintf(mf, "\"l\":%u\n}", count);

	return 0;
}

unsigned int create_mapRegions(FILE *mf, FILE *f, char space) {
	int rc = 0;
	sqlite3_stmt *stmt = NULL;
	unsigned int count = 0;
	char txtbuf[101];
	char *sql = NULL;

	// meta
	fprintf(mf, "\"map%cRegions\":{\n", space);
	fprintf(mf, "\"j\":\"map%cSpace\",\n", space);
	fprintf(mf, "\"c\":[\"regionName\",\"x\",\"y\",\"z\",\"xMin\",\"xMax\",\"yMin\",\"yMax\",\"zMin\",\"zMax\",\"factionID\",\"radius\",\"graphicID\",\"wormholeClassID\"],\n");
	fprintf(mf, "\"k\":\"regionID\",\n");
	fprintf(mf, "\"t\":\"db3\",\n");

	// data
	fprintf(f, "\"map%cRegions\":{\n", space);
	fprintf(f, "\"d\":{\n");
	if (space == 'K') sql = "SELECT r.*, s.graphicID, w.wormholeClassID FROM mapRegions AS r LEFT OUTER JOIN mapLocationScenes AS s ON r.regionID = s.locationID LEFT OUTER JOIN mapLocationWormholeClasses AS w ON r.regionID = w.locationID WHERE r.regionID < 11000000 AND r.regionID != 10000004 AND r.regionID != 10000017 AND r.regionID != 10000019 ORDER BY r.regionID";
	else if (space == 'J') sql = "SELECT r.*, s.graphicID, w.wormholeClassID FROM mapRegions AS r LEFT OUTER JOIN mapLocationScenes AS s ON r.regionID = s.locationID LEFT OUTER JOIN mapLocationWormholeClasses AS w ON r.regionID = w.locationID WHERE r.regionID = 10000004 OR r.regionID = 10000017 OR r.regionID = 10000019 ORDER BY r.regionID";
	else if (space == 'W') sql = "SELECT r.*, s.graphicID, w.wormholeClassID FROM mapRegions AS r LEFT OUTER JOIN mapLocationScenes AS s ON r.regionID = s.locationID LEFT OUTER JOIN mapLocationWormholeClasses AS w ON r.regionID = w.locationID WHERE r.regionID >= 11000000 ORDER BY r.regionID";
	rc = sqlite3_prepare_v2(DB3_UD, sql, -1, &stmt, NULL);
	if (SQLITE_OK != rc) return dump_db3_error(DB3_UD, 1);
	rc = sqlite3_step(stmt);
	while (SQLITE_ROW == rc) {
		fprintf(f, "\"%lld\":[", sqlite3_column_int64(stmt, 0));
		db3_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf);
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 2));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 3));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 4));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 5));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 6));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 7));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 8));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 9));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 10));
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 11));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 12));
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 13));
		fprintf(f, "%lld", sqlite3_column_int64(stmt, 14));
		rc = sqlite3_step(stmt);
		if (SQLITE_ROW == rc) fprintf(f, "],\n");
		else fprintf(f, "]\n");
		count++;
	}
	rc = sqlite3_finalize(stmt);
	fprintf(f, "}\n}");

	fprintf(mf, "\"l\":%u\n}", count);

	return 0;
}

unsigned int create_mapConstellations(FILE *mf, FILE *f, char space) {
	int rc = 0;
	sqlite3_stmt *stmt = NULL;
	unsigned int count = 0;
	char txtbuf[101];
	char *sql = NULL;

	// meta
	fprintf(mf, "\"map%cConstellations\":{\n", space);
	fprintf(mf, "\"j\":\"map%cSpace\",\n", space);
	fprintf(mf, "\"c\":[\"regionID\",\"constellationName\",\"x\",\"y\",\"z\",\"xMin\",\"xMax\",\"yMin\",\"yMax\",\"zMin\",\"zMax\",\"factionID\",\"radius\",\"wormholeClassID\"],\n");
	fprintf(mf, "\"k\":\"constellationID\",\n");
	fprintf(mf, "\"t\":\"db3\",\n");

	// data
	fprintf(f, "\"map%cConstellations\":{\n", space);
	fprintf(f, "\"d\":{\n");
	if (space == 'K') sql = "SELECT r.*, w.wormholeClassID FROM mapConstellations AS r LEFT OUTER JOIN mapLocationWormholeClasses AS w ON r.constellationID = w.locationID WHERE r.regionID < 11000000 AND r.regionID != 10000004 AND r.regionID != 10000017 AND r.regionID != 10000019 ORDER BY r.constellationID";
	else if (space == 'J') sql = "SELECT r.*, w.wormholeClassID FROM mapConstellations AS r LEFT OUTER JOIN mapLocationWormholeClasses AS w ON r.constellationID = w.locationID WHERE r.regionID = 10000004 OR r.regionID = 10000017 OR r.regionID = 10000019 ORDER BY r.constellationID";
	else if (space == 'W') sql = "SELECT r.*, w.wormholeClassID FROM mapConstellations AS r LEFT OUTER JOIN mapLocationWormholeClasses AS w ON r.constellationID = w.locationID WHERE r.regionID >= 11000000 ORDER BY r.constellationID";
	rc = sqlite3_prepare_v2(DB3_UD, sql, -1, &stmt, NULL);
	if (SQLITE_OK != rc) return dump_db3_error(DB3_UD, 1);
	rc = sqlite3_step(stmt);
	while (SQLITE_ROW == rc) {
		fprintf(f, "\"%lld\":[", sqlite3_column_int64(stmt, 1));
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 0));
		db3_column_text(txtbuf, stmt, 2, 101);
		fprintf(f, "\"%s\",", txtbuf);
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 3));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 4));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 5));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 6));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 7));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 8));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 9));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 10));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 11));
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 12));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 13));
		fprintf(f, "%lld", sqlite3_column_int64(stmt, 14));
		rc = sqlite3_step(stmt);
		if (SQLITE_ROW == rc) fprintf(f, "],\n");
		else fprintf(f, "]\n");
		count++;
	}
	rc = sqlite3_finalize(stmt);
	fprintf(f, "}\n}");

	fprintf(mf, "\"l\":%u\n}", count);

	return 0;
}

unsigned int create_mapSolarSystemJumps(FILE *mf, FILE *f, char space) {
	int rc = 0;
	unsigned int count = 0;
	sqlite3_stmt *stmt = NULL;
	sqlite3_int64 prev_from = 0;
	sqlite3_int64 fromID = 0;
	sqlite3_int64 toID = 0;

	// meta
	fprintf(mf, "\"map%cSolarSystemJumps\":{\n", space);
	fprintf(mf, "\"j\":\"map%cSpace\",\n", space);
	fprintf(mf, "\"c\":[\"fromRegionID\",\"fromConstellationID\",\"toConstellationID\",\"toRegionID\"],\n");
	fprintf(mf, "\"k\":\"fromSolarSystemID:toSolarSystemID\",\n");
	fprintf(mf, "\"t\":\"db3\",\n");

	// data
	fprintf(f, "\"map%cSolarSystemJumps\":{\n", space);
	fprintf(f, "\"d\":{\n");
	rc = sqlite3_prepare_v2(DB3_UD, "SELECT * FROM mapSolarSystemJumps ORDER BY fromSolarSystemID, toSolarSystemID", -1, &stmt, NULL);
	if (SQLITE_OK != rc) return dump_db3_error(DB3_UD, 1);
	rc = sqlite3_step(stmt);
	prev_from = 0;
	while (SQLITE_ROW == rc) {
		fromID = sqlite3_column_int64(stmt, 2);
		toID = sqlite3_column_int64(stmt, 3);
		if (prev_from != fromID) {
			if (0 != prev_from) {
				fprintf(f, "\n},\n");
			}
			fprintf(f, "\"%lld\":{\n\"%lld\":[", fromID, toID);
			prev_from = fromID;
			count++;
		}
		else {
			fprintf(f, ",\n\"%lld\":[", toID);
		}
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 0));
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 1));
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 4));
		fprintf(f, "%lld", sqlite3_column_int64(stmt, 5));
		rc = sqlite3_step(stmt);
		if (SQLITE_ROW == rc) fprintf(f, "]");
		else fprintf(f, "]\n}\n");
	}
	rc = sqlite3_finalize(stmt);
	fprintf(f, "}\n}");

	fprintf(mf, "\"l\":%u\n}", count);

	return 0;
}

unsigned int create_mapSolarSystems(FILE *mf, FILE *f, char space) {
	int rc = 0;
	sqlite3_stmt *stmt = NULL;
	unsigned int count = 0;
	char txtbuf[101];
	char *sql = NULL;
	sqlite3_int64 solarSystemID;

	// meta
	fprintf(mf, "\"map%cSolarSystems\":{\n", space);
	fprintf(mf, "\"j\":\"map%cSpace\",\n", space);
	fprintf(mf, "\"c\":[\"regionID\",\"constellationID\",\"solarSystemName\",\"x\",\"y\",\"z\",\"xMin\",\"xMax\",\"yMin\",\"yMax\",\"zMin\",\"zMax\",\"luminosity\",\"border\",\"fringe\",\"corridor\",\"hub\",\"international\",\"regional\",\"constellation\",\"contiguous\",\"security\",\"factionID\",\"radius\",\"sunTypeID\",\"securityClass\",\"wormholeClassID\",\"stationCount\"],\n");
	fprintf(mf, "\"k\":\"solarSystemID\",\n");
	fprintf(mf, "\"t\":\"db3\",\n");

	// data
	fprintf(f, "\"map%cSolarSystems\":{\n", space);
	fprintf(f, "\"d\":{\n");
	if (space == 'K') sql = "SELECT r.*, w.wormholeClassID, dd.stationCount FROM mapSolarSystems AS r LEFT OUTER JOIN mapLocationWormholeClasses AS w ON r.solarSystemID = w.locationID LEFT OUTER JOIN (SELECT d.solarSystemID AS stationSolarSystemID, count(*) AS stationCount FROM mapDenormalize AS d WHERE groupID = 15 GROUP BY d.solarSystemID) AS dd ON dd.stationSolarSystemID = r.solarSystemID WHERE r.regionID < 11000000 AND r.regionID != 10000004 AND r.regionID != 10000017 AND r.regionID != 10000019 ORDER BY r.solarSystemID";
	else if (space == 'J') sql = "SELECT r.*, w.wormholeClassID, dd.stationCount FROM mapSolarSystems AS r LEFT OUTER JOIN mapLocationWormholeClasses AS w ON r.solarSystemID = w.locationID LEFT OUTER JOIN (SELECT d.solarSystemID AS stationSolarSystemID, count(*) AS stationCount FROM mapDenormalize AS d WHERE groupID = 15 GROUP BY d.solarSystemID) AS dd ON dd.stationSolarSystemID = r.solarSystemID WHERE r.regionID = 10000004 OR r.regionID = 10000017 OR r.regionID = 10000019 ORDER BY r.solarSystemID";
	else if (space == 'W') sql = "SELECT r.*, w.wormholeClassID, 0 FROM mapSolarSystems AS r LEFT OUTER JOIN mapLocationWormholeClasses AS w ON r.solarSystemID = w.locationID WHERE r.regionID >= 11000000 ORDER BY r.solarSystemID";
	rc = sqlite3_prepare_v2(DB3_UD, sql, -1, &stmt, NULL);
	if (SQLITE_OK != rc) return dump_db3_error(DB3_UD, 1);
	rc = sqlite3_step(stmt);
	while (SQLITE_ROW == rc) {
		solarSystemID = sqlite3_column_int64(stmt, 2);
		fprintf(f, "\"%lld\":[", solarSystemID);
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 0));
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 1));
		db3_column_text(txtbuf, stmt, 3, 101);
		fprintf(f, "\"%s\",", txtbuf);
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 4));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 5));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 6));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 7));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 8));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 9));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 10));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 11));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 12)); // zMax
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 13));
		fprintf(f, "%s,", sqlite3_column_int(stmt, 14) == 0 ? "false" : "true");
		fprintf(f, "%s,", sqlite3_column_int(stmt, 15) == 0 ? "false" : "true");
		fprintf(f, "%s,", sqlite3_column_int(stmt, 16) == 0 ? "false" : "true");
		fprintf(f, "%s,", sqlite3_column_int(stmt, 17) == 0 ? "false" : "true");
		fprintf(f, "%s,", sqlite3_column_int(stmt, 18) == 0 ? "false" : "true"); // international
		fprintf(f, "%s,", sqlite3_column_int(stmt, 19) == 0 ? "false" : "true");
		fprintf(f, "%s,", sqlite3_column_int(stmt, 20) == 0 ? "false" : "true");
		if (space == 'K') fprintf(f, "%s,", CONT_FLAG[solarSystemID - CONT_OFF] == 0 ? "false" : "true"); // added, contiguous
		else fprintf(f, "false,");
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 21)); // security
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 22));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 23));
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 24)); // sunTypeID
		db3_column_text(txtbuf, stmt, 25, 3);
		fprintf(f, "\"%s\",", txtbuf);
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 26));
		fprintf(f, "%lld", sqlite3_column_int64(stmt, 27));
		rc = sqlite3_step(stmt);
		if (SQLITE_ROW == rc) fprintf(f, "],\n");
		else fprintf(f, "]\n");
		count++;
	}
	rc = sqlite3_finalize(stmt);
	fprintf(f, "}\n}");

	fprintf(mf, "\"l\":%u\n}", count);

	return 0;
}

unsigned int create_mapRegionJumps(FILE *mf, FILE *f) {
	int rc = 0;
	unsigned int count = 0;
	sqlite3_stmt *stmt = NULL;
	sqlite3_int64 prev_from = 0;
	sqlite3_int64 fromID = 0;
	sqlite3_int64 toID = 0;

	// meta
	fprintf(mf, "\"mapRegionJumps\":{\n");
	fprintf(mf, "\"j\":\"mapJumps\",\n");
	fprintf(mf, "\"c\":[],\n");
	fprintf(mf, "\"k\":\"fromRegionID:toRegionID\",\n");
	fprintf(mf, "\"t\":\"db3\",\n");

	// data
	fprintf(f, "\"mapRegionJumps\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = sqlite3_prepare_v2(DB3_UD, "SELECT * FROM mapRegionJumps ORDER BY fromRegionID, toRegionID", -1, &stmt, NULL);
	if (SQLITE_OK != rc) return dump_db3_error(DB3_UD, 1);
	rc = sqlite3_step(stmt);
	prev_from = 0;
	while (SQLITE_ROW == rc) {
		fromID = sqlite3_column_int64(stmt, 0);
		toID = sqlite3_column_int64(stmt, 1);
		if (prev_from != fromID) {
			if (0 != prev_from) {
				fprintf(f, "\n},\n");
			}
			fprintf(f, "\"%lld\":{\n\"%lld\":[", fromID, toID);
			prev_from = fromID;
			count++;
		}
		else {
			fprintf(f, ",\n\"%lld\":[", toID);
		}
		rc = sqlite3_step(stmt);
		if (SQLITE_ROW == rc) fprintf(f, "]");
		else fprintf(f, "]\n}\n");
	}
	rc = sqlite3_finalize(stmt);
	fprintf(f, "}\n}");

	fprintf(mf, "\"l\":%u\n}", count);

	return 0;
}

unsigned int create_mapConstellationJumps(FILE *mf, FILE *f) {
	int rc = 0;
	unsigned int count = 0;
	sqlite3_stmt *stmt = NULL;
	sqlite3_int64 prev_from = 0;
	sqlite3_int64 fromID = 0;
	sqlite3_int64 toID = 0;

	// meta
	fprintf(mf, "\"mapConstellationJumps\":{\n");
	fprintf(mf, "\"j\":\"mapJumps\",\n");
	fprintf(mf, "\"c\":[\"fromRegionID\",\"toRegionID\"],\n");
	fprintf(mf, "\"k\":\"fromConstellationID:toConstellationID\",\n");
	fprintf(mf, "\"t\":\"db3\",\n");

	// data
	fprintf(f, "\"mapConstellationJumps\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = sqlite3_prepare_v2(DB3_UD, "SELECT * FROM mapConstellationJumps ORDER BY fromConstellationID, toConstellationID", -1, &stmt, NULL);
	if (SQLITE_OK != rc) return dump_db3_error(DB3_UD, 1);
	rc = sqlite3_step(stmt);
	prev_from = 0;
	while (SQLITE_ROW == rc) {
		fromID = sqlite3_column_int64(stmt, 1);
		toID = sqlite3_column_int64(stmt, 2);
		if (prev_from != fromID) {
			if (0 != prev_from) {
				fprintf(f, "\n},\n");
			}
			fprintf(f, "\"%lld\":{\n\"%lld\":[", fromID, toID);
			prev_from = fromID;
			count++;
		}
		else {
			fprintf(f, ",\n\"%lld\":[", toID);
		}
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 0));
		fprintf(f, "%lld", sqlite3_column_int64(stmt, 3));
		rc = sqlite3_step(stmt);
		if (SQLITE_ROW == rc) fprintf(f, "]");
		else fprintf(f, "]\n}\n");
	}
	rc = sqlite3_finalize(stmt);
	fprintf(f, "}\n}");

	fprintf(mf, "\"l\":%u\n}", count);

	return 0;
}

unsigned int create_mapJumps(FILE *mf, FILE *f) {
	int rc = 0;
	unsigned int count = 0;
	sqlite3_stmt *stmt = NULL;

	// meta
	fprintf(mf, "\"mapJumps\":{\n");
	fprintf(mf, "\"j\":\"mapJumps\",\n");
	fprintf(mf, "\"c\":[\"destinationID\"],\n");
	fprintf(mf, "\"k\":\"stargateID\",\n");
	fprintf(mf, "\"t\":\"db3\",\n");

	// data
	fprintf(f, "\"mapJumps\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = sqlite3_prepare_v2(DB3_UD, "SELECT * FROM mapJumps ORDER BY stargateID", -1, &stmt, NULL);
	if (SQLITE_OK != rc) return dump_db3_error(DB3_UD, 1);
	rc = sqlite3_step(stmt);
	while (SQLITE_ROW == rc) {
		fprintf(f, "\"%lld\":[", sqlite3_column_int64(stmt, 0));
		fprintf(f, "%lld", sqlite3_column_int64(stmt, 1));
		rc = sqlite3_step(stmt);
		if (SQLITE_ROW == rc) fprintf(f, "],\n");
		else fprintf(f, "]\n");
		count++;
	}
	rc = sqlite3_finalize(stmt);
	fprintf(f, "}\n}");

	fprintf(mf, "\"l\":%u\n}", count);

	return 0;
}

unsigned int create_warCombatZoneSystems(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	unsigned int count = 0;

	// meta
	fprintf(mf, "\"warCombatZoneSystems\":{\n");
	fprintf(mf, "\"j\":\"mapKCelestials\",\n");
	fprintf(mf, "\"c\":[\"combatZoneID\"],\n");
	fprintf(mf, "\"k\":\"solarSystemID\",\n");
	fprintf(mf, "\"t\":\"db3\",\n");

	// data
	fprintf(f, "\"warCombatZoneSystems\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM warCombatZoneSystems ORDER BY solarSystemID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		fprintf(f, "%ld", sql_column_long(stmt, 1));
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

unsigned int create_warCombatZones(FILE *mf, FILE *f) {
	SQLRETURN rc;
	SQLHSTMT stmt;
	unsigned int count = 0;
	char txtbuf[501];

	// meta
	fprintf(mf, "\"warCombatZones\":{\n");
	fprintf(mf, "\"j\":\"mapKCelestials\",\n");
	fprintf(mf, "\"c\":[\"combatZoneName\",\"factionID\",\"centerSystemID\",\"description\"],\n");
	fprintf(mf, "\"k\":\"combatZoneID\",\n");
	fprintf(mf, "\"t\":\"db3\",\n");

	// data
	fprintf(f, "\"warCombatZones\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H_DBC, &stmt);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_DBC, H_DBC, 1);
	rc = SQLExecDirect(stmt, "SELECT * FROM warCombatZones ORDER BY combatZoneID", SQL_NTS);
	if (!SQL_SUCCEEDED(rc)) return dump_sql_error(rc, SQL_HANDLE_STMT, stmt, 1);
	rc = SQLFetch(stmt);
	while (SQL_SUCCEEDED(rc)) {
		fprintf(f, "\"%ld\":[", sql_column_long(stmt, 0));
		sql_column_text(txtbuf, stmt, 1, 101);
		fprintf(f, "\"%s\",", txtbuf); // combatZoneName
		fprintf(f, "%ld,", sql_column_long(stmt, 2)); // factionID
		fprintf(f, "%ld,", sql_column_long(stmt, 3)); // centerSystemID
		sql_column_text(txtbuf, stmt, 4, 501);
		fprintf(f, "\"%s\"", txtbuf); // description
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

unsigned int create_mapCelestials(FILE *mf, FILE *f, char space) {
	int rc = 0;
	sqlite3_stmt *stmt = NULL;
	unsigned int count = 0;
	char txtbuf[101];
	char *sql = NULL;

	// meta
	fprintf(mf, "\"map%cCelestials\":{\n", space);
	fprintf(mf, "\"j\":\"map%cCelestials\",\n", space);
	fprintf(mf, "\"c\":[\"typeID\",\"groupID\",\"solarSystemID\",\"constellationID\",\"regionID\",\"orbitID\",\"x\",\"y\",\"z\",\"radius\",\"itemName\",\"security\",\"celestialIndex\",\"orbitIndex\"],\n");
	fprintf(mf, "\"k\":\"itemID\",\n");
	fprintf(mf, "\"t\":\"db3\",\n");

	// data; exclude regions, constellations, systems, moons, planets, belts, and gates
	fprintf(f, "\"map%cCelestials\":{\n", space);
	fprintf(f, "\"d\":{\n");
	if (space == 'K') sql = "SELECT * FROM mapDenormalize WHERE (groupID = 6 OR groupID > 10) AND ((regionID < 11000000 AND regionID != 10000004 AND regionID != 10000017 AND regionID != 10000019) OR (regionID IS NULL AND itemID < 11000000 AND itemID != 10000004 AND itemID != 10000017 AND itemID != 10000019)) ORDER BY itemID";
	else if (space == 'J') sql = "SELECT * FROM mapDenormalize WHERE (groupID = 6 OR groupID > 10) AND (regionID = 10000004 OR regionID = 10000017 OR regionID = 10000019 OR (regionID IS NULL AND (itemID = 10000004 OR itemID = 10000017 OR itemID = 10000019))) ORDER BY itemID";
	else if (space == 'W') sql = "SELECT * FROM mapDenormalize WHERE (groupID = 6 OR groupID > 10) AND (regionID >= 11000000 OR (regionID IS NULL AND itemID >= 11000000)) ORDER BY itemID";
	rc = sqlite3_prepare_v2(DB3_UD, sql, -1, &stmt, NULL);
	if (SQLITE_OK != rc) return dump_db3_error(DB3_UD, 1);
	rc = sqlite3_step(stmt);
	while (SQLITE_ROW == rc) {
		fprintf(f, "\"%lld\":[", sqlite3_column_int64(stmt, 0));
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 1)); // typeID
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 2)); // groupID
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 3)); // solarSystemID
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 4)); // constellationID
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 5)); // regionID
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 6)); // orbitID
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 7)); // x
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 8)); // y
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 9)); // z
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 10)); // radius
		db3_column_text(txtbuf, stmt, 11, 101);
		fprintf(f, "\"%s\",", txtbuf);
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 12)); // security
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 13)); // celestialIndex
		fprintf(f, "%lld", sqlite3_column_int64(stmt, 14)); // orbitIndex
		rc = sqlite3_step(stmt);
		if (SQLITE_ROW == rc) fprintf(f, "],\n");
		else fprintf(f, "]\n");
		count++;
	}
	rc = sqlite3_finalize(stmt);
	fprintf(f, "}\n}");

	fprintf(mf, "\"l\":%u\n}", count);

	return 0;
}

unsigned int create_mapDenorm(FILE *mf, char space, const char *tag) {
	unsigned int segment = 0;
	unsigned int segsize = 15000;
	char jsonfile[BUFLEN] = NULLSTR;
	int segrc;
	unsigned int count = 0;
	sqlite3_int64 minID;
	sqlite3_int64 maxID;
	char *condition;
	FILE *sf = NULL;

	if (strncmp(tag, "Planets", 7)) condition = "groupID = 7";
	else if (strncmp(tag, "Moons", 5)) condition = "groupID = 8";
	else if (strncmp(tag, "Belts", 5)) condition = "groupID = 9";
	else if (strncmp(tag, "Gates", 5)) condition = "groupID = 10";

	// meta
	SNPRINTF(jsonfile, BUFLEN, "map%c%s", space, tag);
	fprintf(mf, "\"map%c%s\":{\n", space, tag);
	fprintf(mf, "\"c\":[\"typeID\",\"groupID\",\"solarSystemID\",\"constellationID\",\"regionID\",\"orbitID\",\"x\",\"y\",\"z\",\"radius\",\"itemName\",\"security\",\"celestialIndex\",\"orbitIndex\"],\n");
	fprintf(mf, "\"k\":\"itemID\",\n");
	fprintf(mf, "\"t\":\"db3\",\n");
	fprintf(mf, "\"s\":[\n"); // this table is segmented

	// segmented data
	printf("%s - segmented\n", jsonfile);
	do {
		if (!(sf = open_segment(jsonfile, segment))) return 1;
		if (0 != segment) fprintf(mf, ",\n");
		minID = -1;
		maxID = -1;
		segrc = create_mapDenorm_segment(sf, segment, segsize, &minID, &maxID, condition, space);
		if (close_segment(jsonfile, segment, sf, segrc)) return 1;
		fprintf(mf, "[\"%02u\",%lld,%lld]", segment++, minID, maxID);
		count += segrc;
	} while (segrc == segsize);
	if (segrc < 0) return -segrc;
	fprintf(mf, "\n],\n\"l\":%u\n}", count);

	return 0;
}

unsigned int create_mapCelestialStatistics(FILE *mf, char space) {
	unsigned int segment = 0;
	unsigned int segsize = 15000;
	char jsonfile[BUFLEN] = NULLSTR;
	int segrc;
	unsigned int count = 0;
	sqlite3_int64 minID;
	sqlite3_int64 maxID;
	FILE *sf = NULL;

	// meta
	SNPRINTF(jsonfile, BUFLEN, "map%cCelestialStatistics", space);
	fprintf(mf, "\"map%cCelestialStatistics\":{\n", space);
	fprintf(mf, "\"c\":[\"temperature\",\"spectralClass\",\"luminosity\",\"age\",\"life\",\"orbitRadius\",\"eccentricity\",\"massDust\",\"massGas\",\"fragmented\",\"density\",\"surfaceGravity\",\"escapeVelocity\",\"orbitPeriod\",\"rotationRate\",\"locked\",\"pressure\",\"radius\",\"mass\"],\n");
	fprintf(mf, "\"k\":\"celestialID\",\n");
	fprintf(mf, "\"t\":\"db3\",\n");
	fprintf(mf, "\"s\":[\n"); // this table is segmented

	// segmented data
	printf("%s - segmented\n", jsonfile);
	do {
		if (!(sf = open_segment(jsonfile, segment))) return 1;
		if (0 != segment) fprintf(mf, ",\n");
		minID = -1;
		maxID = -1;
		segrc = create_mapCelest_segment(sf, segment, segsize, &minID, &maxID, space);
		if (close_segment(jsonfile, segment, sf, segrc)) return 1;
		fprintf(mf, "[\"%02u\",%lld,%lld]", segment++, minID, maxID);
		count += segrc;
	} while (segrc == segsize);
	if (segrc < 0) return -segrc;
	fprintf(mf, "\n],\n\"l\":%u\n}", count);

	return 0;
}

int create_mapDenorm_segment(FILE *f, unsigned int segment, unsigned int segsize, sqlite3_int64 *minID, sqlite3_int64 *maxID, const char *tagw, char space) {
	char sql[2 * BUFLEN] = NULLSTR;
	char txtbuf[101] = NULLSTR;
	sqlite3_stmt *stmt = NULL;
	int rc = 0;

	if (space == 'K') SNPRINTF(sql, 2 * BUFLEN, "SELECT * FROM mapDenormalize WHERE (%s) AND ((regionID < 11000000 AND regionID != 10000004 AND regionID != 10000017 AND regionID != 10000019) OR (regionID IS NULL AND itemID < 11000000 AND itemID != 10000004 AND itemID != 10000017 AND itemID != 10000019)) ORDER BY itemID LIMIT %u OFFSET %u\0", tagw, segsize, segment * segsize);
	else if (space == 'J') SNPRINTF(sql, 2 * BUFLEN, "SELECT * FROM mapDenormalize WHERE (%s) AND (regionID = 10000004 OR regionID = 10000017 OR regionID = 10000019 OR (regionID IS NULL AND (itemID = 10000004 OR itemID = 10000017 OR itemID = 10000019))) ORDER BY itemID LIMIT %u OFFSET %u\0", tagw, segsize, segment * segsize);
	else if (space == 'W') SNPRINTF(sql, 2 * BUFLEN, "SELECT * FROM mapDenormalize WHERE (%s) AND (regionID >= 11000000 OR (regionID IS NULL AND itemID >= 11000000)) ORDER BY itemID LIMIT %u OFFSET %u\0", tagw, segsize, segment * segsize);
	rc = sqlite3_prepare_v2(DB3_UD, sql, -1, &stmt, NULL);
	if (SQLITE_OK != rc) return -dump_db3_error(DB3_UD, 1);
	rc = sqlite3_step(stmt);
	int count = 0;
	while (SQLITE_ROW == rc) {
		sqlite3_int64 itemID = sqlite3_column_int64(stmt, 0);
		if (count == 0 && minID != NULL) *minID = itemID;
		fprintf(f, "\"%lld\":[", itemID);
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 1)); // typeID
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 2)); // groupID
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 3)); // solarSystemID
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 4)); // constellationID
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 5)); // regionID
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 6)); // orbitID
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 7)); // x
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 8)); // y
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 9)); // z
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 10)); // radius
		db3_column_text(txtbuf, stmt, 11, 101);
		fprintf(f, "\"%s\",", txtbuf);
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 12)); // security
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 13)); // celestialIndex
		fprintf(f, "%lld", sqlite3_column_int64(stmt, 14)); // orbitIndex
		rc = sqlite3_step(stmt);
		if (SQLITE_ROW == rc) fprintf(f, "],\n");
		else {
			if (maxID != NULL) *maxID = itemID;
			fprintf(f, "]\n");
		}
		if (0 == (count++ % 1000)) {
			fflush(f);
		}
	}
	rc = sqlite3_finalize(stmt);

	return count;
}

int create_mapCelest_segment(FILE *f, unsigned int segment, unsigned int segsize, sqlite3_int64 *minID, sqlite3_int64 *maxID, char space) {
	char sql[2 * BUFLEN] = NULLSTR;
	char txtbuf[101] = NULLSTR;
	sqlite3_stmt *stmt = NULL;
	int rc = 0;

	// create mapCelestialStatistics
	if (space == 'K') SNPRINTF(sql, 2 * BUFLEN, "SELECT m.* FROM mapCelestialStatistics AS m LEFT OUTER JOIN mapDenormalize AS d ON m.celestialID = d.itemID WHERE ((d.regionID < 11000000 AND d.regionID != 10000004 AND d.regionID != 10000017 AND d.regionID != 10000019) OR (d.regionID IS NULL AND d.itemID < 11000000 AND d.itemID != 10000004 AND d.itemID != 10000017 AND d.itemID != 10000019)) ORDER BY m.celestialID LIMIT %u OFFSET %u\0", segsize, segment * segsize);
	else if (space == 'J') SNPRINTF(sql, 2 * BUFLEN, "SELECT m.* FROM mapCelestialStatistics AS m LEFT OUTER JOIN mapDenormalize AS d ON m.celestialID = d.itemID WHERE (d.regionID = 10000004 OR d.regionID = 10000017 OR d.regionID = 10000019 OR (d.regionID IS NULL AND (d.itemID = 10000004 OR d.itemID = 10000017 OR d.itemID = 10000019))) ORDER BY m.celestialID LIMIT %u OFFSET %u\0", segsize, segment * segsize);
	else if (space == 'W') SNPRINTF(sql, 2 * BUFLEN, "SELECT m.* FROM mapCelestialStatistics AS m LEFT OUTER JOIN mapDenormalize AS d ON m.celestialID = d.itemID WHERE d.regionID >= 11000000 OR (d.regionID IS NULL AND d.itemID >= 11000000) ORDER BY m.celestialID LIMIT %u OFFSET %u\0", segsize, segment * segsize);
	rc = sqlite3_prepare_v2(DB3_UD, sql, -1, &stmt, NULL);
	if (SQLITE_OK != rc) return -dump_db3_error(DB3_UD, 1);
	rc = sqlite3_step(stmt);
	int count = 0;
	while (SQLITE_ROW == rc) {
		sqlite3_int64 celestialID = sqlite3_column_int64(stmt, 0);
		if (count == 0 && minID != NULL) *minID = celestialID;
		fprintf(f, "\"%lld\":[", celestialID);
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 1));
		db3_column_text(txtbuf, stmt, 2, 11);
		fprintf(f, "\"%s\",", txtbuf);
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 3));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 4)); // age
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 5));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 6));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 7));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 8)); // massDust
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 9));
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 10)); // fragmented
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 11));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 12));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 13)); // escapeVelocity
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 14));
		fprintf(f, "%Lg,", sqlite3_column_double(stmt, 15));
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 16)); // locked
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 17));
		fprintf(f, "%lld,", sqlite3_column_int64(stmt, 18));
		fprintf(f, "%lld", sqlite3_column_int64(stmt, 19));
		rc = sqlite3_step(stmt);
		if (SQLITE_ROW == rc) fprintf(f, "],\n");
		else {
			if (maxID != NULL) *maxID = celestialID;
			fprintf(f, "]\n");
		}
		if (0 == (count++ % 1000)) {
			fflush(f);
		}
	}
	rc = sqlite3_finalize(stmt);

	return count;
}

void prepare_contiguous() {
	unsigned int **jumptbl;
	sqlite3_stmt *stmt = NULL;
	sqlite3_int64 prevFromID = 0;
	sqlite3_int64 fromID = 0;
	sqlite3_int64 toID = 0;
	unsigned int cur_jumps = 0;

	// index is solarsystem ID - CONT_OFF; a stupidly inefficient sparse array
	jumptbl = (unsigned int**)malloc(CONT_OFF * sizeof(unsigned int*));
	for (int i = 0; i < CONT_SIZ; i++) {
		CONT_FLAG[i] = 0;
		jumptbl[i] = (unsigned int*)malloc(10 * sizeof(unsigned int)); // by SQL inspection I know max jumps out of a system is 8
		for (int j = 0; j < 10; j++) {
			jumptbl[i][j] = 0;
		}
	}

	// load all jumps into our jump table
	int rc = sqlite3_prepare_v2(DB3_UD, "SELECT j.fromSolarSystemID, j.toSolarSystemID FROM mapSolarSystemJumps AS j LEFT OUTER JOIN mapSolarSystems AS mF ON j.fromSolarSystemID = mF.solarSystemID LEFT OUTER JOIN mapSolarSystems AS mT ON j.toSolarSystemID = mT.solarSystemID WHERE mF.security >= 0.45 AND mT.security >= 0.45 ORDER BY j.fromSolarSystemID, j.toSolarSystemID", -1, &stmt, NULL);
	if (SQLITE_OK != rc) {
		dump_db3_error(DB3_UD, 0);
		fprintf(stderr, "could not prepare contiguity data\n");
		return;
	}
	rc = sqlite3_step(stmt);
	while (SQLITE_ROW == rc) {
		fromID = sqlite3_column_int64(stmt, 0);
		toID = sqlite3_column_int64(stmt, 1);
		if (prevFromID != fromID) {
			cur_jumps = 0;
			prevFromID = fromID;
		}
		jumptbl[(unsigned int)fromID - CONT_OFF][cur_jumps++] = (unsigned int)toID - CONT_OFF;
		rc = sqlite3_step(stmt);
	}
	rc = sqlite3_finalize(stmt);

	// follow all jumps from Jita, marking each connected system as we go
	follow_and_mark(JITA_ID - CONT_OFF, jumptbl);

	for (int i = 0; i < CONT_SIZ; i++) {
		free(jumptbl[i]);
	}
	free(jumptbl);
}

void follow_and_mark(unsigned int idx, unsigned int **jumptbl) {
	unsigned int *jumps = jumptbl[idx];
	unsigned int sysidx = 0;

	CONT_FLAG[idx] = 1;
	for (int i = 0; i < 10; i++) {
		sysidx = jumps[i];
		if (sysidx == 0) continue;
		if (CONT_FLAG[sysidx] != 0) continue; // already visited

		// depth first means we should never have more stack frames than max hisec jump length; about 30-ish
		follow_and_mark(sysidx, jumptbl);
	}
}
