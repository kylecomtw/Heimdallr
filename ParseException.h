#ifndef PARSEEXCEPTION_H
#define PARSEEXCEPTION_H

#include <exception>
#include <cstdio>

class ParseException : public std::exception
{
private:
	int lineno;
	char buf[80];

public:
	ParseException(int ln):lineno(ln) {
		sprintf_s(buf, "Parse error in line %d", lineno);
	};	
	int get_position() { return lineno; }
	char const* what() {				
		return buf;
	}
};

#endif //PARSEEXCEPTION_H
