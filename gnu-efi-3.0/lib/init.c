/*++

Copyright (c) 1998  Intel Corporation

Module Name:


Abstract:




Revision History

--*/

#include "lib.h"

VOID
EFIDebugVariable (
    VOID
    );

VOID
InitializeLib (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
    )
/*++

Routine Description:

    Initializes EFI library for use
    
Arguments:

    Firmware's EFI system table
    
Returns:

    None

--*/ 
{
    EFI_LOADED_IMAGE        *LoadedImage;
    EFI_STATUS              Status;
    CHAR8                   *LangCode;

    if (!LibInitialized) {
        LibInitialized = TRUE;
        LibFwInstance = FALSE;

        //
        // Set up global pointer to the system table, boot services table,
        // and runtime services table
        //

        ST = SystemTable;
        BS = SystemTable->BootServices;
        RT = SystemTable->RuntimeServices;
//        ASSERT (CheckCrc(0, &ST->Hdr));
//        ASSERT (CheckCrc(0, &BS->Hdr));
//        ASSERT (CheckCrc(0, &RT->Hdr));


        //
        // Initialize pool allocation type
        //

        if (ImageHandle) {
            Status = uefi_call_wrapper(
			    BS->HandleProtocol,
				3,
                            ImageHandle, 
                            &LoadedImageProtocol,
                            (VOID*)&LoadedImage
                            );

            if (!EFI_ERROR(Status)) {
                PoolAllocationType = LoadedImage->ImageDataType;
            }
            
            EFIDebugVariable ();
        }

        //
        // Initialize Guid table
        //

        InitializeGuid();

        InitializeLibPlatform(ImageHandle,SystemTable);
    }

    //
    // 
    //

    if (ImageHandle && UnicodeInterface == &LibStubUnicodeInterface) {
        LangCode = LibGetVariable (VarLanguage, &EfiGlobalVariable);
        InitializeUnicodeSupport (LangCode);
        if (LangCode) {
            FreePool (LangCode);
        }
    }
}

VOID
InitializeUnicodeSupport (
    CHAR8 *LangCode
    )
{
    EFI_UNICODE_COLLATION_INTERFACE *Ui;
    EFI_STATUS                      Status;
    CHAR8                           *Languages;
    UINTN                           Index, Position, Length;
    UINTN                           NoHandles;
    EFI_HANDLE                      *Handles;

    //
    // If we don't know it, lookup the current language code
    //

    LibLocateHandle (ByProtocol, &UnicodeCollationProtocol, NULL, &NoHandles, &Handles);
    if (!LangCode || !NoHandles) {
        goto Done;
    }

    //
    // Check all driver's for a matching language code
    //

    for (Index=0; Index < NoHandles; Index++) {
        Status = uefi_call_wrapper(BS->HandleProtocol, 3, Handles[Index], &UnicodeCollationProtocol, (VOID*)&Ui);
        if (EFI_ERROR(Status)) {
            continue;
        }

        //
        // Check for a matching language code
        //

        Languages = Ui->SupportedLanguages;
        Length = strlena(Languages);
        for (Position=0; Position < Length; Position += ISO_639_2_ENTRY_SIZE) {

            //
            // If this code matches, use this driver
            //

            if (CompareMem (Languages+Position, LangCode, ISO_639_2_ENTRY_SIZE) == 0) {
                UnicodeInterface = Ui;
                goto Done;
            }
        }
    }

Done:
    //
    // Cleanup
    //

    if (Handles) {
        FreePool (Handles);
    }
}

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

VOID
EFIDebugVariable (
    VOID
    )
{
    EFI_STATUS      Status;
    UINT32          Attributes;
    UINTN           DataSize;
    UINTN           NewEFIDebug;

    DataSize = sizeof(EFIDebug);
    Status = uefi_call_wrapper(RT->GetVariable, 5, L"EFIDebug", &EfiGlobalVariable, &Attributes, &DataSize, &NewEFIDebug);
    if (!EFI_ERROR(Status)) {
        EFIDebug = NewEFIDebug;
    } 
}
