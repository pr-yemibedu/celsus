#include <windows.h>
#include "UnicodeUtils.hpp"

/*
* AnsiToUnicode converts the ANSI string pszA to a Unicode string
* and returns the Unicode string through ppszW. Space for the
* the converted string is allocated by AnsiToUnicode.
* Allocated with malloc
*/ 

HRESULT AnsiToUnicode(LPCSTR pszA, LPOLESTR* ppszW)
{

  ULONG cCharacters;
  DWORD dwError;

  // If input is null then just return the same.
  if (NULL == pszA)
  {
    *ppszW = NULL;
    return NOERROR;
  }

  // Determine number of wide characters to be allocated for the
  // Unicode string.
  cCharacters =  strlen(pszA)+1;

  // Use of the OLE allocator is required if the resultant Unicode
  // string will be passed to another COM component and if that
  // component will free it. Otherwise you can use your own allocator.
  *ppszW = (LPOLESTR)malloc(cCharacters*2);
  if (NULL == *ppszW)
    return E_OUTOFMEMORY;

  // Covert to Unicode.
  if (0 == MultiByteToWideChar(CP_ACP, 0, pszA, cCharacters, *ppszW, cCharacters))
  {
    dwError = GetLastError();
    free(*ppszW);
    *ppszW = NULL;
    return HRESULT_FROM_WIN32(dwError);
  }

  return NOERROR;
}
/*
* UnicodeToAnsi converts the Unicode string pszW to an ANSI string
* and returns the ANSI string through ppszA. Space for the
* the converted string is allocated by UnicodeToAnsi.
* Allocated with malloc
*/ 

HRESULT UnicodeToAnsi(LPCOLESTR pszW, LPSTR* ppszA)
{

  ULONG cbAnsi, cCharacters;
  DWORD dwError;

  // If input is null then just return the same.
  if (pszW == NULL)
  {
    *ppszA = NULL;
    return NOERROR;
  }

  cCharacters = wcslen(pszW)+1;
  // Determine number of bytes to be allocated for ANSI string. An
  // ANSI string can have at most 2 bytes per character (for Double
  // Byte Character Strings.)
  cbAnsi = cCharacters*2;

  *ppszA = (LPSTR)malloc(cbAnsi);
  if (NULL == *ppszA)
    return E_OUTOFMEMORY;

  // Convert to ANSI.
  if (0 == WideCharToMultiByte(CP_ACP, 0, pszW, cCharacters, *ppszA, cbAnsi, NULL, NULL))
  {
    dwError = GetLastError();
    free(*ppszA);
    *ppszA = NULL;
    return HRESULT_FROM_WIN32(dwError);
  }
  return NOERROR;
}

HRESULT UnicodeToAnsiToBuffer(LPCOLESTR pszW, char* buffer, const int buf_len)
{

  ULONG cbAnsi, cCharacters;
  DWORD dwError;

  // If input is null then just return the same.
  if (pszW == NULL)
  {
    return NOERROR;
  }

  cCharacters = wcslen(pszW)+1;
  // Determine number of bytes to be allocated for ANSI string. An
  // ANSI string can have at most 2 bytes per character (for Double
  // Byte Character Strings.)
  cbAnsi = cCharacters*2;

  if ((int)cbAnsi > buf_len) {
    return E_OUTOFMEMORY;
  }

  // Convert to ANSI.
  if (0 == WideCharToMultiByte(CP_ACP, 0, pszW, cCharacters, buffer, cbAnsi, NULL, NULL))
  {
    dwError = GetLastError();
    return HRESULT_FROM_WIN32(dwError);
  }
  return NOERROR;
}
