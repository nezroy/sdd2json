#ifdef __GNUC__
#include <getopt.h>
#endif
#ifndef __GNUC__

#ifndef _W32GETOPT_H_
#define _W32GETOPT_H_

extern int opterr;
extern int optind;
extern int optopt;
extern char *optarg;
extern int getopt(int argc, char **argv, char *opts);

#endif
#endif
