#ifndef MAP_H
#define MAP_H

#include <sqlite3.h>

#define CONT_SIZ 50000
#define CONT_OFF 30000000
#define JITA_ID 30000142

unsigned int create_map(const char*, unsigned int, sqlite3*, FILE*);

void prepare_contiguous(sqlite3*);
void follow_and_mark(unsigned int, unsigned int**);

unsigned int create_mapSpace(const char*, unsigned int, sqlite3*, char);
unsigned int create_mapRSpace(const char*, unsigned int, sqlite3*);
unsigned int create_mapJumps(const char*, unsigned int, sqlite3*);
unsigned int create_mapDenorm(const char*, unsigned int, sqlite3*, char);
int create_mapDenorm_segment(const char*, unsigned int, sqlite3*, unsigned int, unsigned int, sqlite3_int64*, sqlite3_int64*, const char*, const char*, char);
int create_mapCelest_segment(const char*, unsigned int, sqlite3*, unsigned int, unsigned int, sqlite3_int64*, sqlite3_int64*, char);

#endif