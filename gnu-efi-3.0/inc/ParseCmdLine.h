

/*
 * Turn an CHAR16 * command line into a CHAR16 * argv, INT argc pair.
 *
 */

#define MAX_ARGS 512


int ParseCmdLine(CHAR16 **argv[],  CHAR16 *buf, int len);

