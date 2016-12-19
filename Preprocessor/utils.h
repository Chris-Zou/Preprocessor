#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>
#include <stdarg.h>

namespace utils {


	inline int32_t vsnprintf(char* _str, size_t _count, const char* _format, va_list _argList)
	{
#if BX_COMPILER_MSVC
		va_list argListCopy;
		va_copy(argListCopy, _argList);
		int32_t len = ::vsnprintf_s(_str, _count, size_t(-1), _format, argListCopy);
		va_end(argListCopy);
		return -1 == len ? ::_vscprintf(_format, _argList) : len;
#else
		return ::vsnprintf(_str, _count, _format, _argList);
#endif // BX_COMPILER_MSVC
	}

	inline void stringPrintfVargs(std::string& _out, const char* _format, va_list _argList)
	{
		char temp[2048];

		char* out = temp;
		int32_t len = vsnprintf(out, sizeof(temp), _format, _argList);
		if ((int32_t)sizeof(temp) < len)
		{
			out = (char*)alloca(len + 1);
			len = vsnprintf(out, len, _format, _argList);
		}
		out[len] = '\0';
		_out.append(out);
	}

	inline void eolLF(char* _out, size_t _size, const char* _str)
	{
		if (0 < _size)
		{
			char* end = _out + _size - 1;
			for (char ch = *_str++; ch != '\0' && _out < end; ch = *_str++)
			{
				if ('\r' != ch)
				{
					*_out++ = ch;
				}
			}

			*_out = '\0';
		}
	}

}

#endif
