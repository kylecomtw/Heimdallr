#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <exception>
#include <stdarg.h>
#include <unicode/ucnv.h>
using std::string;
extern FILE* debug_out;

struct PosTaggedType{
	string pos;
	string word;
};

struct DepToken{
	string pos;
	string word;
	int index;
};

inline std::string strtrim(const std::string& s){
	size_t b = s.find_first_not_of(' ');
	size_t e = s.find_last_not_of(' ');
	return s.substr(b, e - b + 1);
}

class PCFG_Exception:public std::exception{};

inline void dprintf(const char* format, ...)
{
#ifdef _DEBUG	
	const int buf_len = 1024;
	char raw[buf_len];	

	va_list arg;
	va_start(arg, format);
	vsprintf_s(raw, format, arg);
	va_end(arg);


	// char big5[buf_len];
	// UErrorCode err = U_ZERO_ERROR;		
	// ucnv_convert("cp950", "UTF-8", big5, buf_len, raw, buf_len, &err);
	// printf(big5);


	if(debug_out){
		fprintf(debug_out, raw);

	}
#endif //_DEBUG

}

inline void rprintf(const char* format, ...)
{
	const int buf_len = 1024;
	char big5[buf_len];
	char raw[buf_len];
	UErrorCode err = U_ZERO_ERROR;
	va_list arg;
	va_start(arg, format);
	vsprintf_s(raw, format, arg);
	va_end(arg);

	ucnv_convert("cp950", "UTF-8", big5, buf_len, raw, buf_len, &err);
	printf(big5);
}

#endif //COMMON_H