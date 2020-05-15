#ifndef	_GETOPT_H_
#define	_GETOPT_H_

extern char *optarg;
extern int optopt;

#ifdef __cplusplus
extern "C" {
#endif

extern int getopt(int argc, char *const *argv, const char *optstring);

#ifdef __cplusplus
}
#endif


#endif	/* _UNISTD_H_ */
