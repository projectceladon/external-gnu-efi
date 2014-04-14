
/*
 *  Turn an CHAR16 * command line into a CHAR16 * argv, INT argc pair.
 *
 */


#include <efi.h>
#include <efilib.h>
#include <ParseCmdLine.h>



int ParseCmdLine(CHAR16 *argv[],  CHAR16 *LoadOptions, int LoadOptionSize)
{
    CHAR16 *buf = LoadOptions;
    int     len = LoadOptionSize;
    int	    i;	  // Index into buf

    int argc = 0;	// Count of argv entries
    argv[argc++] = LoadOptions ? LoadOptions: L"##NoName##";

    for ( i = 0;  len > 0;   len-= 2, i++)  {
	if ( buf[i] == L' ' ) {
	    // end of current argv entry, start next?
	    buf[i] = L'\0';
	    do {
	    	len -= 2;
		i++;
	    } while( (buf[i] == L' ') && (len > 0) );
	    argv[argc] = &buf[i];
	    if ( len > 0 ) argc++;
	}
    }
    argv[argc] = NULL;

    return argc;
}
