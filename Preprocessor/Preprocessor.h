#ifndef _PREPROCESSOR_H_
#define _PREPROCESSOR_H_

#define MAX_TAGS 256

#include <stdlib.h>
#include <string>



class Preprocessor
{
public:
	Preprocessor(const char* _filePath, bool _essl, const char* _includeDir = NULL);
	void setDefine(const char* _define);
	void writef(const char* _format, ...);
	void addInclude(const char* _includeDir);
	void addDependency(const char* _fileName); 
	bool run(const char* _input);
	char* fgets(char* _buffer, int _size);


	static void fppDepends(char* _fileName, void* _userData);
	static char* fppInput(char* _buffer, int _size, void* _userData);
	static void fppOutput(int _ch, void* _userData);
	static void fppError(void* /*_userData*/, char* _format, va_list _vargs);
	char* scratch(const char* _str);


private:
	struct fcppTag {
		int tag;
		void *data;
	};
	fcppTag m_tags[MAX_TAGS];
	fcppTag* m_tagptr;

	std::string m_depends;
	std::string m_default;
	std::string m_input;
	std::string m_preprocessed;
	char m_scratch[16 << 10];
	uint32_t m_scratchPos;
	uint32_t m_fgetsPos;
};

#endif