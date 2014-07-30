
/*
 *  Turn an CHAR16 * command line into a CHAR16 * argv, INT argc pair.
 *
 */


#include <efi.h>
#include <efilib.h>
#include <ParseCmdLine.h>



INTN ParseCmdLine(CHAR16 *argv[MAX_ARGS], CHAR16 *buf, UINTN len)
{
    CHAR16 *arg_start, *c;
    INTN argc = 0;

    if (len < 2)
      return -1;

    buf[(len / 2) - 1] = L'\0';
    for (c = arg_start = buf ; *c ; ++c) {
      if (*c == L' ') {
        *c = L'\0';
        if (argc < MAX_ARGS) argv[argc++] = arg_start;
        arg_start = c + 1;
      }
    }

    if ((*arg_start != L'\0') && (argc < MAX_ARGS))
      argv[argc++] = arg_start;

    return argc;
}
