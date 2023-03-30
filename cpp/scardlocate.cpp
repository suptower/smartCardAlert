// Copyright (C) Microsoft. All rights reserved. 
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#pragma comment(lib, "winscard.lib")

typedef ULONG_PTR SCARDCONTEXT;
typedef SCARDCONTEXT *PSCARDCONTEXT, *LPSCARDCONTEXT;

#if defined(_WIN64)
 typedef unsigned __int64 ULONG_PTR;
#else
 typedef unsigned long ULONG_PTR;
#endif

typedef struct {LPCTSTR szReader;LPVOID pvUserData;DWORD dwCurrentState;DWORD dwEventState;DWORD cbAtr;BYTE rgbAtr[36];} SCARD_READERSTATE, *PSCARD_READERSTATE, *LPSCARD_READERSTATE;


HRESULT __cdecl main()
{
HRESULT           hr = S_OK;
LPTSTR            szReaders, szRdr;
DWORD             cchReaders = SCARD_AUTOALLOCATE;
DWORD             dwI, dwRdrCount;
SCARD_READERSTATE rgscState[MAXIMUM_SMARTCARD_READERS];
TCHAR             szCard[MAX_PATH];
SCARDCONTEXT      hSC;
LONG              lReturn;

// Establish the card to watch for.
// Multiple cards can be looked for, but
// this sample looks for only one card.
_tcscat_s ( szCard, MAX_PATH * sizeof(TCHAR), TEXT("GemSAFE"));
szCard[lstrlen(szCard) + 1] = 0;  // Double trailing zero.

// Establish a context.
lReturn = SCardEstablishContext(SCARD_SCOPE_USER,
                                NULL,
                                NULL,
                                &hSC );
if ( SCARD_S_SUCCESS != lReturn )
{
    printf("Failed SCardEstablishContext\n");
    exit(1);
}

// Determine which readers are available.
lReturn = SCardListReaders(hSC,
                           NULL,
                           (LPTSTR)&szReaders,
                           &cchReaders );
if ( SCARD_S_SUCCESS != lReturn )
{
    printf("Failed SCardListReaders\n");
    exit(1);
}
// Place the readers into the state array.
szRdr = szReaders;
for ( dwI = 0; dwI < MAXIMUM_SMARTCARD_READERS; dwI++ )
{
    if ( 0 == *szRdr )
        break;
    rgscState[dwI].szReader = szRdr;
    rgscState[dwI].dwCurrentState = SCARD_STATE_UNAWARE;
    szRdr += lstrlen(szRdr) + 1;
}
dwRdrCount = dwI;

// If any readers are available, proceed.
if ( 0 != dwRdrCount )
{
  for (;;)
  { 
    // Look for the card.
    lReturn = SCardLocateCards(hSC,
                               szCard,
                               rgscState,
                               dwRdrCount );
    if ( SCARD_S_SUCCESS != lReturn )
    {
        printf("Failed SCardLocateCards\n");
        exit(1);
    }

    // Look through the array of readers.
    for ( dwI=0; dwI < dwRdrCount; dwI++)
    {
        if ( 0 != ( SCARD_STATE_ATRMATCH & 
                    rgscState[dwI].dwEventState))
        {
           _tprintf( TEXT("Card '%s' found in reader '%s'.\n"),
                     szCard,
                     rgscState[dwI].szReader );
            SCardFreeMemory( hSC,
                             szReaders );
            return 0;  // Context will be release automatically.
        }
        // Update the state.
        rgscState[dwI].dwCurrentState = rgscState[dwI].dwEventState;
    }

  // Card not found yet; wait until there is a change.
  lReturn = SCardGetStatusChange(hSC,
                                 INFINITE, // infinite wait
                                 rgscState,
                                 dwRdrCount );
  if ( SCARD_S_SUCCESS != lReturn )
  {
    printf("Failed SCardGetStatusChange\n");
    exit(1);
  }
 }  // for (;;)
}
else
    printf("No readers available\n");

// Release the context.
lReturn = SCardReleaseContext(hSC);
if ( SCARD_S_SUCCESS != lReturn )
{
    printf("Failed SCardReleaseContext\n");
    exit(1);
}

SCardFreeMemory( hSC,
                 szReaders );

return hr;
}