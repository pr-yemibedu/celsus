#ifndef _ERROR2_HPP_
#define _ERROR2_HPP_

class ErrorObj
{
public:
	ErrorObj(const HRESULT hr) : _hr(hr)
	{
		if (FAILED(_hr)) {
			LPVOID lpMsgBuf;  
			const DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
			FormatMessage(flags,  NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
			OutputDebugStringA((const char*)lpMsgBuf);
			LocalFree( lpMsgBuf ); 
		}
	}
	operator HRESULT() { return _hr; }
private:
	HRESULT _hr;
};

inline ErrorObj make_error(const HRESULT hr) { return ErrorObj(hr); }

template<typename T> struct ErrorPredicate{};

template<> struct ErrorPredicate<bool>
{
	static bool test_bool(bool b)
	{
		return b;
	}

	static HRESULT test_hresult(bool b)
	{
		return b ? S_OK : E_FAIL;
	}
};

template<> struct ErrorPredicate<HRESULT>
{
	static bool test_bool(HRESULT hr)
	{
		return SUCCEEDED(hr);
	}

	static HRESULT test_hresult(HRESULT hr)
	{
		return hr;
	}
};

#define RETURN_ON_FAIL_HR(x, pred, log) { HRESULT hr = !pred::test_hresult(x); if (FAILED(hr) { log(#x); return hr; } }
#define RETURN_ON_FAIL_BOOL(x, pred, log) { bool b = !pred::test_bool(x); if (!b) { log(#x); return false; } }
#define RETURN_ON_FAIL_VOID(x, pred, log) { bool b = !pred::test_bool(x); if (!b) { log(#x); return; } }

#endif