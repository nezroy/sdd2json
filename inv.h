#ifndef INV_H
#define INV_H
#include <sql.h>

unsigned int create_inv(const char*, unsigned int, SQLHDBC, FILE*);

unsigned int create_invMeta(const char*, unsigned int, SQLHDBC);

int create_invTypes_segment(const char*, unsigned int, SQLHDBC, unsigned int, unsigned int, SQLINTEGER*, SQLINTEGER*);
int create_invItems_segment(const char*, unsigned int, SQLHDBC, unsigned int, unsigned int, SQLINTEGER*, SQLINTEGER*);
/*
unsigned int create_mapJumps(const char*, unsigned int, SQLHDBC);
unsigned int create_mapDenormOther(const char*, unsigned int, SQLHDBC);
unsigned int create_mapDenorm(const char*, unsigned int, SQLHDBC);
unsigned int create_mapDenorm_segment(const char*, unsigned int, SQLHDBC, const char*, const char*, const char*, int start, unsigned int seg_size);
unsigned int create_mapCelestials_segment(const char*, unsigned int, SQLHDBC, const char*, int, unsigned int);
*/
#endif