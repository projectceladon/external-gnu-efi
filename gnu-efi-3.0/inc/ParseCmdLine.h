

/*
 * Turn an CHAR16 * command line into a CHAR16 * argv, INT argc pair.
 *
 */

#define MAX_ARGS 512

/* buf will be updated in-place */
INTN ParseCmdLine(CHAR16 *argv[MAX_ARGS],  CHAR16 *buf, UINTN len);
