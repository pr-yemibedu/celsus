/*
* AnsiToUnicode converts the ANSI string pszA to a Unicode string
* and returns the Unicode string through ppszW. Space for the
* converted string is allocated by AnsiToUnicode.
* Allocated with malloc
*/ 

HRESULT AnsiToUnicode(LPCSTR pszA, LPOLESTR* ppszW);

/*
* UnicodeToAnsi converts the Unicode string pszW to an ANSI string
* and returns the ANSI string through ppszA. Space for the
* converted string is allocated by UnicodeToAnsi.
* Allocated with malloc
*/ 

HRESULT UnicodeToAnsi(LPCOLESTR pszW, LPSTR* ppszA);

HRESULT UnicodeToAnsiToBuffer(LPCOLESTR pszW, char* buffer, const int buf_len);
