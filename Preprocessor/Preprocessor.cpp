#include "Preprocessor.h"
#include <stdarg.h>
#include "utils.h"

extern "C" {
#include "fpp.h"
}


Preprocessor::Preprocessor(const char* _filePath, bool _essl, const char* _includeDir)
	: m_tagptr(m_tags)
	, m_scratchPos(0)
	, m_fgetsPos(0)
{
	m_tagptr->tag = FPPTAG_USERDATA;
	m_tagptr->data = this;
	m_tagptr++;

	m_tagptr->tag = FPPTAG_DEPENDS;
	m_tagptr->data = (void*)fppDepends;
	m_tagptr++;

	m_tagptr->tag = FPPTAG_INPUT;
	m_tagptr->data = (void*)fppInput;
	m_tagptr++;

	m_tagptr->tag = FPPTAG_OUTPUT;
	m_tagptr->data = (void*)fppOutput;
	m_tagptr++;

	m_tagptr->tag = FPPTAG_ERROR;
	m_tagptr->data = (void*)fppError;
	m_tagptr++;

	m_tagptr->tag = FPPTAG_IGNOREVERSION;
	m_tagptr->data = (void*)0;
	m_tagptr++;

	m_tagptr->tag = FPPTAG_LINE;
	m_tagptr->data = (void*)0;
	m_tagptr++;

	m_tagptr->tag = FPPTAG_INPUT_NAME;
	m_tagptr->data = scratch(_filePath);
	m_tagptr++;

	if (NULL != _includeDir)
	{
		addInclude(_includeDir);
	}

	if (!_essl)
	{
		m_default = "#define lowp\n#define mediump\n#define highp\n";
	}
}

void Preprocessor::setDefine(const char* _define)
{
	m_tagptr->tag = FPPTAG_DEFINE;
	m_tagptr->data = scratch(_define);
	m_tagptr++;
}


void Preprocessor::writef(const char* _format, ...)
{
	va_list argList;
	va_start(argList, _format);
	utils::stringPrintfVargs(m_default, _format, argList);
	va_end(argList);
}

void Preprocessor::addInclude(const char* _includeDir)
{
	char* start = scratch(_includeDir);

	for (char* split = strchr(start, ';'); NULL != split; split = strchr(start, ';'))
	{
		*split = '\0';
		m_tagptr->tag = FPPTAG_INCLUDE_DIR;
		m_tagptr->data = start;
		m_tagptr++;
		start = split + 1;
	}

	m_tagptr->tag = FPPTAG_INCLUDE_DIR;
	m_tagptr->data = start;
	m_tagptr++;
}

void Preprocessor::addDependency(const char* _fileName)
{
	m_depends += " \\\n ";
	m_depends += _fileName;
}

bool Preprocessor::run(const char* _input)
{
	m_fgetsPos = 0;

	m_preprocessed.clear();
	m_input = m_default;
	m_input += "\n\n";

	size_t len = strlen(_input) + 1;
	char* temp = new char[len];
	utils::eolLF(temp, len, _input);
	m_input += temp;
	delete[] temp;

	fppTag* tagptr = reinterpret_cast<fppTag*>(m_tagptr);

	tagptr->tag = FPPTAG_END;
	tagptr->data = 0;
	tagptr++;

	int result = fppPreProcess(reinterpret_cast<fppTag*>(m_tags));

	return 0 == result;
}

char* Preprocessor::fgets(char* _buffer, int _size)
{
	int ii = 0;
	for (char ch = m_input[m_fgetsPos]; m_fgetsPos < m_input.size() && ii < _size - 1; ch = m_input[++m_fgetsPos])
	{
		_buffer[ii++] = ch;

		if (ch == '\n' || ii == _size)
		{
			_buffer[ii] = '\0';
			m_fgetsPos++;
			return _buffer;
		}
	}

	return NULL;
}

void Preprocessor::fppDepends(char* _fileName, void* _userData)
{
	Preprocessor* thisClass = (Preprocessor*)_userData;
	thisClass->addDependency(_fileName);
}

char* Preprocessor::fppInput(char* _buffer, int _size, void* _userData)
{
	Preprocessor* thisClass = (Preprocessor*)_userData;
	return thisClass->fgets(_buffer, _size);
}

void Preprocessor::fppOutput(int _ch, void* _userData)
{
	Preprocessor* thisClass = (Preprocessor*)_userData;
	thisClass->m_preprocessed += _ch;
}

void Preprocessor::fppError(void* /*_userData*/, char* _format, va_list _vargs)
{
	vfprintf(stderr, _format, _vargs);
}

char* Preprocessor::scratch(const char* _str)
{
	char* result = &m_scratch[m_scratchPos];
	strcpy(result, _str);
	m_scratchPos += (uint32_t)strlen(_str) + 1;

	return result;
}
