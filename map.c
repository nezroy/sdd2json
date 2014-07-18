#include <stdio.h>
#include <string.h>
#include "compat.h"

#include <sqlite3.h>

#include "sdd2json.h"
#include "map.h"

unsigned short CONT_FLAG[CONT_SIZ];

unsigned int create_map(const char *outdir, unsigned int schema, sqlite3 *db, FILE *mf) {
	int rc = 0;

	prepare_contiguous(db);

	fprintf(mf, "\"mapLandmarks\":{\n");
	fprintf(mf, "\"j\":\"mapKSpace\",\n");
	fprintf(mf, "\"c\":[\"landmarkName\",\"description\",\"locationID\",\"x\",\"y\",\"z\",\"iconID\"],\n");
	fprintf(mf, "\"k\":\"landmarkID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapKRegions\":{\n");
	fprintf(mf, "\"j\":\"mapKSpace\",\n");
	fprintf(mf, "\"c\":[\"regionName\",\"x\",\"y\",\"z\",\"xMin\",\"xMax\",\"yMin\",\"yMax\",\"zMin\",\"zMax\",\"factionID\",\"radius\",\"graphicID\",\"wormholeClassID\"],\n");
	fprintf(mf, "\"k\":\"regionID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapKConstellations\":{\n");
	fprintf(mf, "\"j\":\"mapKSpace\",\n");
	fprintf(mf, "\"c\":[\"regionID\",\"constellationName\",\"x\",\"y\",\"z\",\"xMin\",\"xMax\",\"yMin\",\"yMax\",\"zMin\",\"zMax\",\"factionID\",\"radius\",\"wormholeClassID\"],\n");
	fprintf(mf, "\"k\":\"constellationID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapKSolarSystemJumps\":{\n");
	fprintf(mf, "\"j\":\"mapKSpace\",\n");
	fprintf(mf, "\"c\":[\"fromRegionID\",\"fromConstellationID\",\"toConstellationID\",\"toRegionID\"],\n");
	fprintf(mf, "\"k\":\"fromSolarSystemID:toSolarSystemID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapKSolarSystems\":{\n");
	fprintf(mf, "\"j\":\"mapKSpace\",\n");
	fprintf(mf, "\"c\":[\"regionID\",\"constellationID\",\"solarSystemName\",\"x\",\"y\",\"z\",\"xMin\",\"xMax\",\"yMin\",\"yMax\",\"zMin\",\"zMax\",\"luminosity\",\"border\",\"fringe\",\"corridor\",\"hub\",\"international\",\"regional\",\"constellation\",\"contiguous\",\"security\",\"factionID\",\"radius\",\"sunTypeID\",\"securityClass\",\"wormholeClassID\",\"stationCount\"],\n");
	fprintf(mf, "\"k\":\"solarSystemID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	rc = create_mapSpace(outdir, schema, db, 'K');
	if (0 != rc) return rc;
	rc = post_file(outdir, "mapKSpace");
	if (0 != rc) return rc;

	fprintf(mf, "\"mapJRegions\":{\n");
	fprintf(mf, "\"j\":\"mapJSpace\",\n");
	fprintf(mf, "\"c\":[\"regionName\",\"x\",\"y\",\"z\",\"xMin\",\"xMax\",\"yMin\",\"yMax\",\"zMin\",\"zMax\",\"factionID\",\"radius\",\"graphicID\",\"wormholeClassID\"],\n");
	fprintf(mf, "\"k\":\"regionID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapJConstellations\":{\n");
	fprintf(mf, "\"j\":\"mapJSpace\",\n");
	fprintf(mf, "\"c\":[\"regionID\",\"constellationName\",\"x\",\"y\",\"z\",\"xMin\",\"xMax\",\"yMin\",\"yMax\",\"zMin\",\"zMax\",\"factionID\",\"radius\",\"wormholeClassID\"],\n");
	fprintf(mf, "\"k\":\"constellationID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapJSolarSystemJumps\":{\n");
	fprintf(mf, "\"j\":\"mapJSpace\",\n");
	fprintf(mf, "\"c\":[\"fromRegionID\",\"fromConstellationID\",\"toConstellationID\",\"toRegionID\"],\n");
	fprintf(mf, "\"k\":\"fromSolarSystemID:toSolarSystemID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapJSolarSystems\":{\n");
	fprintf(mf, "\"j\":\"mapJSpace\",\n");
	fprintf(mf, "\"c\":[\"regionID\",\"constellationID\",\"solarSystemName\",\"x\",\"y\",\"z\",\"xMin\",\"xMax\",\"yMin\",\"yMax\",\"zMin\",\"zMax\",\"luminosity\",\"border\",\"fringe\",\"corridor\",\"hub\",\"international\",\"regional\",\"constellation\",\"contiguous\",\"security\",\"factionID\",\"radius\",\"sunTypeID\",\"securityClass\",\"wormholeClassID\",\"stationCount\"],\n");
	fprintf(mf, "\"k\":\"solarSystemID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	rc = create_mapSpace(outdir, schema, db, 'J');
	if (0 != rc) return rc;
	rc = post_file(outdir, "mapJSpace");
	if (0 != rc) return rc;

	fprintf(mf, "\"mapWRegions\":{\n");
	fprintf(mf, "\"j\":\"mapWSpace\",\n");
	fprintf(mf, "\"c\":[\"regionName\",\"x\",\"y\",\"z\",\"xMin\",\"xMax\",\"yMin\",\"yMax\",\"zMin\",\"zMax\",\"factionID\",\"radius\",\"graphicID\",\"wormholeClassID\"],\n");
	fprintf(mf, "\"k\":\"regionID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapWConstellations\":{\n");
	fprintf(mf, "\"j\":\"mapWSpace\",\n");
	fprintf(mf, "\"c\":[\"regionID\",\"constellationName\",\"x\",\"y\",\"z\",\"xMin\",\"xMax\",\"yMin\",\"yMax\",\"zMin\",\"zMax\",\"factionID\",\"radius\",\"wormholeClassID\"],\n");
	fprintf(mf, "\"k\":\"constellationID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapWSolarSystems\":{\n");
	fprintf(mf, "\"j\":\"mapWSpace\",\n");
	fprintf(mf, "\"c\":[\"regionID\",\"constellationID\",\"solarSystemName\",\"x\",\"y\",\"z\",\"xMin\",\"xMax\",\"yMin\",\"yMax\",\"zMin\",\"zMax\",\"luminosity\",\"border\",\"fringe\",\"corridor\",\"hub\",\"international\",\"regional\",\"constellation\",\"contiguous\",\"security\",\"factionID\",\"radius\",\"sunTypeID\",\"securityClass\",\"wormholeClassID\",\"stationCount\"],\n");
	fprintf(mf, "\"k\":\"solarSystemID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	rc = create_mapSpace(outdir, schema, db, 'W');
	if (0 != rc) return rc;
	rc = post_file(outdir, "mapWSpace");
	if (0 != rc) return rc;

	fprintf(mf, "\"mapRegionJumps\":{\n");
	fprintf(mf, "\"j\":\"mapJumps\",\n");
	fprintf(mf, "\"c\":[],\n");
	fprintf(mf, "\"k\":\"fromRegionID:toRegionID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapConstellationJumps\":{\n");
	fprintf(mf, "\"j\":\"mapJumps\",\n");
	fprintf(mf, "\"c\":[\"fromRegionID\",\"toRegionID\"],\n");
	fprintf(mf, "\"k\":\"fromConstellationID:toConstellationID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapJumps\":{\n");
	fprintf(mf, "\"j\":\"mapJumps\",\n");
	fprintf(mf, "\"c\":[\"destinationID\"],\n");
	fprintf(mf, "\"k\":\"stargateID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	rc = create_mapJumps(outdir, schema, db);
	if (0 != rc) return rc;
	rc = post_file(outdir, "mapJumps");
	if (0 != rc) return rc;

	fprintf(mf, "\"mapKPlanets\":{\n");
	fprintf(mf, "\"j\":\"mapKCelestials\",\n");
	fprintf(mf, "\"c\":[\"typeID\",\"groupID\",\"solarSystemID\",\"constellationID\",\"regionID\",\"orbitID\",\"x\",\"y\",\"z\",\"radius\",\"itemName\",\"security\",\"celestialIndex\",\"orbitIndex\"],\n");
	fprintf(mf, "\"k\":\"itemID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapKBelts\":{\n");
	fprintf(mf, "\"j\":\"mapKCelestials\",\n");
	fprintf(mf, "\"c\":[\"typeID\",\"groupID\",\"solarSystemID\",\"constellationID\",\"regionID\",\"orbitID\",\"x\",\"y\",\"z\",\"radius\",\"itemName\",\"security\",\"celestialIndex\",\"orbitIndex\"],\n");
	fprintf(mf, "\"k\":\"itemID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapKMoons\":{\n");
	fprintf(mf, "\"j\":\"mapKCelestials\",\n");
	fprintf(mf, "\"c\":[\"typeID\",\"groupID\",\"solarSystemID\",\"constellationID\",\"regionID\",\"orbitID\",\"x\",\"y\",\"z\",\"radius\",\"itemName\",\"security\",\"celestialIndex\",\"orbitIndex\"],\n");
	fprintf(mf, "\"k\":\"itemID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapKGates\":{\n");
	fprintf(mf, "\"j\":\"mapKCelestials\",\n");
	fprintf(mf, "\"c\":[\"typeID\",\"groupID\",\"solarSystemID\",\"constellationID\",\"regionID\",\"orbitID\",\"x\",\"y\",\"z\",\"radius\",\"itemName\",\"security\",\"celestialIndex\",\"orbitIndex\"],\n");
	fprintf(mf, "\"k\":\"itemID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapKCelestials\":{\n");
	fprintf(mf, "\"j\":\"mapKCelestials\",\n");
	fprintf(mf, "\"c\":[\"typeID\",\"groupID\",\"solarSystemID\",\"constellationID\",\"regionID\",\"orbitID\",\"x\",\"y\",\"z\",\"radius\",\"itemName\",\"security\",\"celestialIndex\",\"orbitIndex\"],\n");
	fprintf(mf, "\"k\":\"itemID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapKCelestialStatistics\":{\n");
	fprintf(mf, "\"j\":\"mapKCelestials\",\n");
	fprintf(mf, "\"c\":[\"temperature\",\"spectralClass\",\"luminosity\",\"age\",\"life\",\"orbitRadius\",\"eccentricity\",\"massDust\",\"massGas\",\"fragmented\",\"density\",\"surfaceGravity\",\"escapeVelocity\",\"orbitPeriod\",\"rotationRate\",\"locked\",\"pressure\",\"radius\",\"mass\"],\n");
	fprintf(mf, "\"k\":\"celestialID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},");
	rc = create_mapDenorm(outdir, schema, db, 'K');
	if (0 != rc) return rc;
	rc = post_file(outdir, "mapKCelestials");
	if (0 != rc) return rc;

	fprintf(mf, "\"mapJPlanets\":{\n");
	fprintf(mf, "\"j\":\"mapJCelestials\",\n");
	fprintf(mf, "\"c\":[\"typeID\",\"groupID\",\"solarSystemID\",\"constellationID\",\"regionID\",\"orbitID\",\"x\",\"y\",\"z\",\"radius\",\"itemName\",\"security\",\"celestialIndex\",\"orbitIndex\"],\n");
	fprintf(mf, "\"k\":\"itemID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapJBelts\":{\n");
	fprintf(mf, "\"j\":\"mapJCelestials\",\n");
	fprintf(mf, "\"c\":[\"typeID\",\"groupID\",\"solarSystemID\",\"constellationID\",\"regionID\",\"orbitID\",\"x\",\"y\",\"z\",\"radius\",\"itemName\",\"security\",\"celestialIndex\",\"orbitIndex\"],\n");
	fprintf(mf, "\"k\":\"itemID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapJMoons\":{\n");
	fprintf(mf, "\"j\":\"mapJCelestials\",\n");
	fprintf(mf, "\"c\":[\"typeID\",\"groupID\",\"solarSystemID\",\"constellationID\",\"regionID\",\"orbitID\",\"x\",\"y\",\"z\",\"radius\",\"itemName\",\"security\",\"celestialIndex\",\"orbitIndex\"],\n");
	fprintf(mf, "\"k\":\"itemID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapJGates\":{\n");
	fprintf(mf, "\"j\":\"mapJCelestials\",\n");
	fprintf(mf, "\"c\":[\"typeID\",\"groupID\",\"solarSystemID\",\"constellationID\",\"regionID\",\"orbitID\",\"x\",\"y\",\"z\",\"radius\",\"itemName\",\"security\",\"celestialIndex\",\"orbitIndex\"],\n");
	fprintf(mf, "\"k\":\"itemID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapJCelestials\":{\n");
	fprintf(mf, "\"j\":\"mapJCelestials\",\n");
	fprintf(mf, "\"c\":[\"typeID\",\"groupID\",\"solarSystemID\",\"constellationID\",\"regionID\",\"orbitID\",\"x\",\"y\",\"z\",\"radius\",\"itemName\",\"security\",\"celestialIndex\",\"orbitIndex\"],\n");
	fprintf(mf, "\"k\":\"itemID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapJCelestialStatistics\":{\n");
	fprintf(mf, "\"j\":\"mapJCelestials\",\n");
	fprintf(mf, "\"c\":[\"temperature\",\"spectralClass\",\"luminosity\",\"age\",\"life\",\"orbitRadius\",\"eccentricity\",\"massDust\",\"massGas\",\"fragmented\",\"density\",\"surfaceGravity\",\"escapeVelocity\",\"orbitPeriod\",\"rotationRate\",\"locked\",\"pressure\",\"radius\",\"mass\"],\n");
	fprintf(mf, "\"k\":\"celestialID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},");
	rc = create_mapDenorm(outdir, schema, db, 'J');
	if (0 != rc) return rc;
	rc = post_file(outdir, "mapJCelestials");
	if (0 != rc) return rc;

	fprintf(mf, "\"mapWPlanets\":{\n");
	fprintf(mf, "\"j\":\"mapWCelestials\",\n");
	fprintf(mf, "\"c\":[\"typeID\",\"groupID\",\"solarSystemID\",\"constellationID\",\"regionID\",\"orbitID\",\"x\",\"y\",\"z\",\"radius\",\"itemName\",\"security\",\"celestialIndex\",\"orbitIndex\"],\n");
	fprintf(mf, "\"k\":\"itemID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapWMoons\":{\n");
	fprintf(mf, "\"j\":\"mapWCelestials\",\n");
	fprintf(mf, "\"c\":[\"typeID\",\"groupID\",\"solarSystemID\",\"constellationID\",\"regionID\",\"orbitID\",\"x\",\"y\",\"z\",\"radius\",\"itemName\",\"security\",\"celestialIndex\",\"orbitIndex\"],\n");
	fprintf(mf, "\"k\":\"itemID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapWCelestials\":{\n");
	fprintf(mf, "\"j\":\"mapWCelestials\",\n");
	fprintf(mf, "\"c\":[\"typeID\",\"groupID\",\"solarSystemID\",\"constellationID\",\"regionID\",\"orbitID\",\"x\",\"y\",\"z\",\"radius\",\"itemName\",\"security\",\"celestialIndex\",\"orbitIndex\"],\n");
	fprintf(mf, "\"k\":\"itemID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "},\n");
	fprintf(mf, "\"mapWCelestialStatistics\":{\n");
	fprintf(mf, "\"j\":\"mapWCelestials\",\n");
	fprintf(mf, "\"c\":[\"temperature\",\"spectralClass\",\"luminosity\",\"age\",\"life\",\"orbitRadius\",\"eccentricity\",\"massDust\",\"massGas\",\"fragmented\",\"density\",\"surfaceGravity\",\"escapeVelocity\",\"orbitPeriod\",\"rotationRate\",\"locked\",\"pressure\",\"radius\",\"mass\"],\n");
	fprintf(mf, "\"k\":\"celestialID\",\n");
	fprintf(mf, "\"t\":\"db3\"\n");
	fprintf(mf, "}");
	rc = create_mapDenorm(outdir, schema, db, 'W');
	if (0 != rc) return rc;
	rc = post_file(outdir, "mapWCelestials");
	if (0 != rc) return rc;

	return 0;
}

unsigned int create_mapSpace(const char *outdir, unsigned int schema, sqlite3 *db, char space) {
	char jsonfile[BUFLEN] = NULLSTR;
	char *sql = NULL;
	char txtbuf[101] = NULLSTR;
	char descbuf[7001] = NULLSTR;
	sqlite3_stmt *stmt = NULL;
	int rc = 0;
	sqlite3_int64 prev_from = 0;
	sqlite3_int64 fromID = 0;
	sqlite3_int64 toID = 0;
	FILE *f = NULL;
	if (space != 'K' && space != 'W' && space != 'J') {
		printf("err\n");
		fprintf(stderr, "unsupported space type");
		return 1;
	}

	// create mapSpace
	SNPRINTF(jsonfile, BUFLEN, "%smap%cSpace.json\0", outdir, space);
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

	if (space == 'K') {
		// mapLandmarks
		fprintf(f, "\"mapLandmarks\":{\n");
		fprintf(f, "\"d\":{\n");
		rc = sqlite3_prepare_v2(db, "SELECT * FROM mapLandmarks ORDER BY landmarkID", -1, &stmt, NULL);
		if (SQLITE_OK != rc) return 2;
		rc = sqlite3_step(stmt);
		while (SQLITE_ROW == rc) {
			fprintf(f, "\"%lld\":[", sqlite3_column_int64(stmt, 0));
			db3_column_text(txtbuf, stmt, 1, 101);
			fprintf(f, "\"%s\",", txtbuf);
			db3_column_text(descbuf, stmt, 2, 7001);
			fprintf(f, "\"%s\",", descbuf);
			fprintf(f, "%lld,", sqlite3_column_int64(stmt, 3));
			fprintf(f, "%Lg,", sqlite3_column_double(stmt, 4));
			fprintf(f, "%Lg,", sqlite3_column_double(stmt, 5));
			fprintf(f, "%Lg,", sqlite3_column_double(stmt, 6));
			fprintf(f, "%lld", sqlite3_column_int64(stmt, 7));
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc) fprintf(f, "],\n");
			else fprintf(f, "]\n");
		}
		rc = sqlite3_finalize(stmt);
		fprintf(f, "}\n");
		fprintf(f, "},\n"); // end of mapLandmarks
		fflush(f);
	}

	// mapRegions
	SNPRINTF(jsonfile, BUFLEN, "map%cRegions\0", space);
	fprintf(f, "\"%s\":{\n", jsonfile);
	fprintf(f, "\"d\":{\n");
	if (space == 'K') sql = "SELECT r.*, s.graphicID, w.wormholeClassID FROM mapRegions AS r LEFT OUTER JOIN mapLocationScenes AS s ON r.regionID = s.locationID LEFT OUTER JOIN mapLocationWormholeClasses AS w ON r.regionID = w.locationID WHERE r.regionID < 11000000 AND r.regionID != 10000004 AND r.regionID != 10000017 AND r.regionID != 10000019 ORDER BY r.regionID";
	else if (space == 'J') sql = "SELECT r.*, s.graphicID, w.wormholeClassID FROM mapRegions AS r LEFT OUTER JOIN mapLocationScenes AS s ON r.regionID = s.locationID LEFT OUTER JOIN mapLocationWormholeClasses AS w ON r.regionID = w.locationID WHERE r.regionID = 10000004 OR r.regionID = 10000017 OR r.regionID = 10000019 ORDER BY r.regionID";
	else if (space == 'W') sql = "SELECT r.*, s.graphicID, w.wormholeClassID FROM mapRegions AS r LEFT OUTER JOIN mapLocationScenes AS s ON r.regionID = s.locationID LEFT OUTER JOIN mapLocationWormholeClasses AS w ON r.regionID = w.locationID WHERE r.regionID >= 11000000 ORDER BY r.regionID";
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (SQLITE_OK != rc) return 2;
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
	}
	rc = sqlite3_finalize(stmt);
	fprintf(f, "}\n");
	fprintf(f, "},\n"); // end of mapRegions
	fflush(f);

	// mapConstellations
	SNPRINTF(jsonfile, BUFLEN, "map%cConstellations\0", space);
	fprintf(f, "\"%s\":{\n", jsonfile);
	fprintf(f, "\"d\":{\n");
	if (space == 'K') sql = "SELECT r.*, w.wormholeClassID FROM mapConstellations AS r LEFT OUTER JOIN mapLocationWormholeClasses AS w ON r.constellationID = w.locationID WHERE r.regionID < 11000000 AND r.regionID != 10000004 AND r.regionID != 10000017 AND r.regionID != 10000019 ORDER BY r.constellationID";
	else if (space == 'J') sql = "SELECT r.*, w.wormholeClassID FROM mapConstellations AS r LEFT OUTER JOIN mapLocationWormholeClasses AS w ON r.constellationID = w.locationID WHERE r.regionID = 10000004 OR r.regionID = 10000017 OR r.regionID = 10000019 ORDER BY r.constellationID";
	else if (space == 'W') sql = "SELECT r.*, w.wormholeClassID FROM mapConstellations AS r LEFT OUTER JOIN mapLocationWormholeClasses AS w ON r.constellationID = w.locationID WHERE r.regionID >= 11000000 ORDER BY r.constellationID";
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (SQLITE_OK != rc) return 2;
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
	}
	rc = sqlite3_finalize(stmt);
	fprintf(f, "}\n");
	fprintf(f, "},\n"); // end of mapConstellations
	fflush(f);

	// mapSolarSystemJumps
	if (space != 'W') {
		SNPRINTF(jsonfile, BUFLEN, "map%cSolarSystemJumps\0", space);
		fprintf(f, "\"%s\":{\n", jsonfile);
		fprintf(f, "\"d\":{\n");
		rc = sqlite3_prepare_v2(db, "SELECT * FROM mapSolarSystemJumps ORDER BY fromSolarSystemID, toSolarSystemID", -1, &stmt, NULL);
		if (SQLITE_OK != rc) return 2;
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
		fprintf(f, "}\n");
		fprintf(f, "},\n"); // end of mapSolarSystemJumps
		fflush(f);
	}

	// mapSolarSystems
	SNPRINTF(jsonfile, BUFLEN, "map%cSolarSystems\0", space);
	fprintf(f, "\"%s\":{\n", jsonfile);
	fprintf(f, "\"d\":{\n");
	if (space == 'K') sql = "SELECT r.*, w.wormholeClassID, dd.stationCount FROM mapSolarSystems AS r LEFT OUTER JOIN mapLocationWormholeClasses AS w ON r.solarSystemID = w.locationID LEFT OUTER JOIN (SELECT d.solarSystemID AS stationSolarSystemID, count(*) AS stationCount FROM mapDenormalize AS d WHERE groupID = 15 GROUP BY d.solarSystemID) AS dd ON dd.stationSolarSystemID = r.solarSystemID WHERE r.regionID < 11000000 AND r.regionID != 10000004 AND r.regionID != 10000017 AND r.regionID != 10000019 ORDER BY r.solarSystemID";
	else if (space == 'J') sql = "SELECT r.*, w.wormholeClassID, dd.stationCount FROM mapSolarSystems AS r LEFT OUTER JOIN mapLocationWormholeClasses AS w ON r.solarSystemID = w.locationID LEFT OUTER JOIN (SELECT d.solarSystemID AS stationSolarSystemID, count(*) AS stationCount FROM mapDenormalize AS d WHERE groupID = 15 GROUP BY d.solarSystemID) AS dd ON dd.stationSolarSystemID = r.solarSystemID WHERE r.regionID = 10000004 OR r.regionID = 10000017 OR r.regionID = 10000019 ORDER BY r.solarSystemID";
	else if (space == 'W') sql = "SELECT r.*, w.wormholeClassID, 0 FROM mapSolarSystems AS r LEFT OUTER JOIN mapLocationWormholeClasses AS w ON r.solarSystemID = w.locationID WHERE r.regionID >= 11000000 ORDER BY r.solarSystemID";
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (SQLITE_OK != rc) return 2;
	rc = sqlite3_step(stmt);
	while (SQLITE_ROW == rc) {
		sqlite3_int64 solarSystemID = sqlite3_column_int64(stmt, 2);
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
	}
	rc = sqlite3_finalize(stmt);
	fprintf(f, "}\n");
	fprintf(f, "}\n"); // end of mapSolarSystems

	fprintf(f, "}\n");
	fclose(f);

	printf("OK\n");

	return 0;
}

unsigned int create_mapJumps(const char *outdir, unsigned int schema, sqlite3 *db) {
	char jsonfile[BUFLEN] = NULLSTR;
	char txtbuf[101] = NULLSTR;
	char descbuf[7001] = NULLSTR;
	sqlite3_stmt *stmt = NULL;
	int rc = 0;
	FILE *f = NULL;
	sqlite3_int64 prev_from = 0;
	sqlite3_int64 fromID = 0;
	sqlite3_int64 toID = 0;

	// create mapJumps
	strlcpy(jsonfile, outdir, BUFLEN);
	strlcat(jsonfile, "mapJumps.json", BUFLEN);
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

	// mapRegionJumps
	fprintf(f, "\"mapRegionJumps\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = sqlite3_prepare_v2(db, "SELECT * FROM mapRegionJumps ORDER BY fromRegionID, toRegionID", -1, &stmt, NULL);
	if (SQLITE_OK != rc) return 2;
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
		}
		else {
			fprintf(f, ",\n\"%lld\":[", toID);
		}
		rc = sqlite3_step(stmt);
		if (SQLITE_ROW == rc) fprintf(f, "]");
		else fprintf(f, "]\n}\n");
	}
	rc = sqlite3_finalize(stmt);
	fprintf(f, "}\n");
	fprintf(f, "},\n"); // end of mapRegionJumps
	fflush(f);

	// mapConstellationJumps
	fprintf(f, "\"mapConstellationJumps\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = sqlite3_prepare_v2(db, "SELECT * FROM mapConstellationJumps ORDER BY fromConstellationID, toConstellationID", -1, &stmt, NULL);
	if (SQLITE_OK != rc) return 2;
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
	fprintf(f, "}\n");
	fprintf(f, "},\n"); // end of mapConstellationJumps
	fflush(f);

	// mapJumps
	fprintf(f, "\"mapJumps\":{\n");
	fprintf(f, "\"d\":{\n");
	rc = sqlite3_prepare_v2(db, "SELECT * FROM mapJumps ORDER BY stargateID", -1, &stmt, NULL);
	if (SQLITE_OK != rc) return 2;
	rc = sqlite3_step(stmt);
	while (SQLITE_ROW == rc) {
		fprintf(f, "\"%lld\":[", sqlite3_column_int64(stmt, 0));
		fprintf(f, "%lld", sqlite3_column_int64(stmt, 1));
		rc = sqlite3_step(stmt);
		if (SQLITE_ROW == rc) fprintf(f, "],\n");
		else fprintf(f, "]\n");
	}
	rc = sqlite3_finalize(stmt);
	fprintf(f, "}\n");
	fprintf(f, "}\n"); // end of mapJumps

	fprintf(f, "}\n");
	fclose(f);

	printf("OK\n");

	return 0;
}

unsigned int create_mapDenorm(const char *outdir, unsigned int schema, sqlite3 *db, char space) {
	char jsonfile[BUFLEN] = NULLSTR;
	char *sql = NULL;
	char txtbuf[101] = NULLSTR;
	sqlite3_stmt *stmt = NULL;
	int rc = 0;
	int count = 0;
	FILE *f = NULL;

	// create mapCelestials
	SNPRINTF(jsonfile, BUFLEN, "%smap%cCelestials.json\0", outdir, space);
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

	// mapCelestials; exclude regions, constellations, systems, moons, planets, belts, and gates
	SNPRINTF(jsonfile, BUFLEN, "map%cCelestials\0", space);
	fprintf(f, "\"%s\":{\n", jsonfile);
	fprintf(f, "\"d\":{\n");
	if (space == 'K') sql = "SELECT * FROM mapDenormalize WHERE (groupID = 6 OR groupID > 10) AND ((regionID < 11000000 AND regionID != 10000004 AND regionID != 10000017 AND regionID != 10000019) OR (regionID IS NULL AND itemID < 11000000 AND itemID != 10000004 AND itemID != 10000017 AND itemID != 10000019)) ORDER BY itemID";
	else if (space == 'J') sql = "SELECT * FROM mapDenormalize WHERE (groupID = 6 OR groupID > 10) AND (regionID = 10000004 OR regionID = 10000017 OR regionID = 10000019 OR (regionID IS NULL AND (itemID = 10000004 OR itemID = 10000017 OR itemID = 10000019))) ORDER BY itemID";
	else if (space == 'W') sql = "SELECT * FROM mapDenormalize WHERE (groupID = 6 OR groupID > 10) AND (regionID >= 11000000 OR (regionID IS NULL AND itemID >= 11000000)) ORDER BY itemID";
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (SQLITE_OK != rc) return 2;
	rc = sqlite3_step(stmt);
	count = 0;
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
		if (0 == (count++ % 1000)) {
			fflush(f);
		}
	}
	rc = sqlite3_finalize(stmt);
	fprintf(f, "}\n");
	fprintf(f, "},\n"); // end of mapCelestials

	fprintf(f, "}\n");
	fflush(f);

	// segment stuff
	printf("OK\n");
	unsigned int segment;
	unsigned int segsize;
	int segrc;
	sqlite3_int64 minID;
	sqlite3_int64 maxID;

	// mapPlanets
	SNPRINTF(jsonfile, BUFLEN, "map%cPlanets\0", space);
	printf("creating %s - segmented\n", jsonfile);
	fprintf(f, "\"%s\":{\n", jsonfile);
	fprintf(f, "\"s\":[\n");
	segment = 0;
	segsize = 15000;
	do {
		if (0 != segment) fprintf(f, ",\n");
		minID = -1;
		maxID = -2;
		segrc = create_mapDenorm_segment(outdir, schema, db, segment, segsize, &minID, &maxID, "Planets", "groupID = 7", space);
		fprintf(f, "[\"%02u\",%lld,%lld]", segment++, minID, maxID);
	} while (segrc == segsize);
	if (segrc < 0) return -segrc;
	fprintf(f, "\n]\n");
	fprintf(f, "},\n"); // end of mapPlanets
	fflush(f);

	// mapMoons
	SNPRINTF(jsonfile, BUFLEN, "map%cMoons\0", space);
	printf("creating %s - segmented\n", jsonfile);
	fprintf(f, "\"%s\":{\n", jsonfile);
	fprintf(f, "\"s\":[\n");
	segment = 0;
	segsize = 15000;
	do {
		if (0 != segment) fprintf(f, ",\n");
		minID = -1;
		maxID = -2;
		segrc = create_mapDenorm_segment(outdir, schema, db, segment, segsize, &minID, &maxID, "Moons", "groupID = 8", space);
		fprintf(f, "[\"%02u\",%lld,%lld]", segment++, minID, maxID);
	} while (segrc == segsize);
	if (segrc < 0) return -segrc;
	fprintf(f, "\n]\n");
	fprintf(f, "},\n"); // end of mapMoons
	fflush(f);

	if (space == 'K' || space == 'J') {
		// mapBelts
		SNPRINTF(jsonfile, BUFLEN, "map%cBelts\0", space);
		printf("creating %s - segmented\n", jsonfile);
		fprintf(f, "\"%s\":{\n", jsonfile);
		fprintf(f, "\"s\":[\n");
		segment = 0;
		segsize = 15000;
		do {
			if (0 != segment) fprintf(f, ",\n");
			minID = -1;
			maxID = -2;
			segrc = create_mapDenorm_segment(outdir, schema, db, segment, segsize, &minID, &maxID, "Belts", "groupID = 9", space);
			fprintf(f, "[\"%02u\",%lld,%lld]", segment++, minID, maxID);
		} while (segrc == segsize);
		if (segrc < 0) return -segrc;
		fprintf(f, "\n]\n");
		fprintf(f, "},\n"); // end of mapBelts
		fflush(f);

		// mapGates
		SNPRINTF(jsonfile, BUFLEN, "map%cGates\0", space);
		printf("creating %s - segmented\n", jsonfile);
		fprintf(f, "\"%s\":{\n", jsonfile);
		fprintf(f, "\"s\":[\n");
		segment = 0;
		segsize = 10000;
		do {
			if (0 != segment) fprintf(f, ",\n");
			minID = -1;
			maxID = -2;
			segrc = create_mapDenorm_segment(outdir, schema, db, segment, segsize, &minID, &maxID, "Gates", "groupID = 10", space);
			fprintf(f, "[\"%02u\",%lld,%lld]", segment++, minID, maxID);
		} while (segrc == segsize);
		if (segrc < 0) return -segrc;
		fprintf(f, "\n]\n");
		fprintf(f, "},\n"); // end of mapGates
		fflush(f);
	}

	// mapCelestialStatistics
	SNPRINTF(jsonfile, BUFLEN, "map%cCelestialStatistics\0", space);
	printf("creating %s - segmented\n", jsonfile);
	fprintf(f, "\"%s\":{\n", jsonfile);
	fprintf(f, "\"s\":[\n");
	segment = 0;
	segsize = 20000;
	do {
		if (0 != segment) fprintf(f, ",\n");
		minID = -1;
		maxID = -2;
		segrc = create_mapCelest_segment(outdir, schema, db, segment, segsize, &minID, &maxID, space);
		fprintf(f, "[\"%02u\",%lld,%lld]", segment++, minID, maxID);
	} while (segrc == segsize);
	if (segrc < 0) return -segrc;
	fprintf(f, "\n]\n");
	fprintf(f, "}\n"); // end of mapCelestialStatistics
	fflush(f);

	fclose(f);

	return 0;
}

int create_mapDenorm_segment(const char *outdir, unsigned int schema, sqlite3 *db, unsigned int segment, unsigned int segsize, sqlite3_int64 *minID, sqlite3_int64 *maxID, const char *tag, const char *tagw, char space) {
	char jsonfile[BUFLEN] = NULLSTR;
	char sql[2 * BUFLEN] = NULLSTR;
	char txtbuf[101] = NULLSTR;
	sqlite3_stmt *stmt = NULL;
	int rc = 0;
	FILE *f = NULL;

	// create map[Space][Tag]
	SNPRINTF(jsonfile, BUFLEN, "%smap%c%s_%02u.json", outdir, space, tag, segment);
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
	if (space == 'K') SNPRINTF(sql, 2 * BUFLEN, "SELECT * FROM mapDenormalize WHERE (%s) AND ((regionID < 11000000 AND regionID != 10000004 AND regionID != 10000017 AND regionID != 10000019) OR (regionID IS NULL AND itemID < 11000000 AND itemID != 10000004 AND itemID != 10000017 AND itemID != 10000019)) ORDER BY itemID LIMIT %u OFFSET %u\0", tagw, segsize, segment * segsize);
	else if (space == 'J') SNPRINTF(sql, 2 * BUFLEN, "SELECT * FROM mapDenormalize WHERE (%s) AND (regionID = 10000004 OR regionID = 10000017 OR regionID = 10000019 OR (regionID IS NULL AND (itemID = 10000004 OR itemID = 10000017 OR itemID = 10000019))) ORDER BY itemID LIMIT %u OFFSET %u\0", tagw, segsize, segment * segsize);
	else if (space == 'W') SNPRINTF(sql, 2 * BUFLEN, "SELECT * FROM mapDenormalize WHERE (%s) AND (regionID >= 11000000 OR (regionID IS NULL AND itemID >= 11000000)) ORDER BY itemID LIMIT %u OFFSET %u\0", tagw, segsize, segment * segsize);
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (SQLITE_OK != rc) return -2;
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
	fprintf(f, "}\n");

	fprintf(f, "}\n");
	fclose(f);

	printf("OK\n");
	SNPRINTF(jsonfile, BUFLEN, "map%c%s_%02u", space, tag, segment);
	post_file(outdir, jsonfile);

	return count;
}

int create_mapCelest_segment(const char *outdir, unsigned int schema, sqlite3 *db, unsigned int segment, unsigned int segsize, sqlite3_int64 *minID, sqlite3_int64 *maxID, char space) {
	char jsonfile[BUFLEN] = NULLSTR;
	char sql[2 * BUFLEN] = NULLSTR;
	char txtbuf[101] = NULLSTR;
	sqlite3_stmt *stmt = NULL;
	int rc = 0;
	FILE *f = NULL;

	// create mapCelestialStatistics
	SNPRINTF(jsonfile, BUFLEN, "%smap%cCelestialStatistics_%02u.json", outdir, space, segment);
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
	if (space == 'K') SNPRINTF(sql, 2 * BUFLEN, "SELECT m.* FROM mapCelestialStatistics AS m LEFT OUTER JOIN mapDenormalize AS d ON m.celestialID = d.itemID WHERE ((d.regionID < 11000000 AND d.regionID != 10000004 AND d.regionID != 10000017 AND d.regionID != 10000019) OR (d.regionID IS NULL AND d.itemID < 11000000 AND d.itemID != 10000004 AND d.itemID != 10000017 AND d.itemID != 10000019)) ORDER BY m.celestialID LIMIT %u OFFSET %u\0", segsize, segment * segsize);
	else if (space == 'J') SNPRINTF(sql, 2 * BUFLEN, "SELECT m.* FROM mapCelestialStatistics AS m LEFT OUTER JOIN mapDenormalize AS d ON m.celestialID = d.itemID WHERE (d.regionID = 10000004 OR d.regionID = 10000017 OR d.regionID = 10000019 OR (d.regionID IS NULL AND (d.itemID = 10000004 OR d.itemID = 10000017 OR d.itemID = 10000019))) ORDER BY m.celestialID LIMIT %u OFFSET %u\0", segsize, segment * segsize);
	else if (space == 'W') SNPRINTF(sql, 2 * BUFLEN, "SELECT m.* FROM mapCelestialStatistics AS m LEFT OUTER JOIN mapDenormalize AS d ON m.celestialID = d.itemID WHERE d.regionID >= 11000000 OR (d.regionID IS NULL AND d.itemID >= 11000000) ORDER BY m.celestialID LIMIT %u OFFSET %u\0", segsize, segment * segsize);
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (SQLITE_OK != rc) return -2;
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
	fprintf(f, "}\n");

	fprintf(f, "}\n");
	fclose(f);

	printf("OK\n");
	SNPRINTF(jsonfile, BUFLEN, "map%cCelestialStatistics_%02u", space, segment);
	post_file(outdir, jsonfile);

	return count;
}

void prepare_contiguous(sqlite3 *db) {
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
	int rc = sqlite3_prepare_v2(db, "SELECT j.fromSolarSystemID, j.toSolarSystemID FROM mapSolarSystemJumps AS j LEFT OUTER JOIN mapSolarSystems AS mF ON j.fromSolarSystemID = mF.solarSystemID LEFT OUTER JOIN mapSolarSystems AS mT ON j.toSolarSystemID = mT.solarSystemID WHERE mF.security >= 0.45 AND mT.security >= 0.45 ORDER BY j.fromSolarSystemID, j.toSolarSystemID", -1, &stmt, NULL);
	if (SQLITE_OK != rc) {
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
