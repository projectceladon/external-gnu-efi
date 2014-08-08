#include "lib.h"

#ifndef MAX_ARGV_CONTENTS_SIZE
# define MAX_CMDLINE_SIZE 1024
#endif
#ifndef MAX_ARGC
# define MAX_CMDLINE_ARGC 32
#endif

INTN
GetShellArgcArgv(
    EFI_HANDLE ImageHandle,
    CHAR16 **ResultArgv[]
    )
{
  EFI_STATUS Status;
  EFI_LOADED_IMAGE *LoadedImage = NULL;
  static CHAR16 ArgvContents[MAX_CMDLINE_SIZE];
  static CHAR16 *Argv[MAX_CMDLINE_ARGC], *ArgStart, *c;
  UINTN Argc = 0, BufLen;

  Status = uefi_call_wrapper(BS->HandleProtocol, 3,
                             ImageHandle, &LoadedImageProtocol, &LoadedImage);
  if (EFI_ERROR(Status))
    return -1;

  BufLen = LoadedImage->LoadOptionsSize;
  if (BufLen < 2)  /* We are expecting at least a \0 */
    return -1;
  else if (BufLen > sizeof(ArgvContents))
    BufLen = sizeof(ArgvContents);

  CopyMem(ArgvContents, LoadedImage->LoadOptions, BufLen);
  ArgvContents[MAX_CMDLINE_SIZE - 1] = L'\0';

  for (c = ArgStart = ArgvContents ; *c != L'\0' ; ++c) {
    if (*c == L' ') {
      *c = L'\0';
      if (Argc < MAX_CMDLINE_ARGC) Argv[Argc++] = ArgStart;
      ArgStart = c + 1;
    }
  }

  if ((*ArgStart != L'\0') && (Argc < MAX_CMDLINE_ARGC))
    Argv[Argc++] = ArgStart;

  *ResultArgv = Argv;
  return Argc;
}
