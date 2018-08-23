#ifndef PARSERINITIALIZER_H
#define PARSERINITIALIZER_H
#include "IParserInitializable.h"


class ParserInitializer
{
protected:
	IParserInitializable& parser;

public:
	ParserInitializer(IParserInitializable& p) :
		parser(p){}
	virtual void initialize_parser() = 0;
};

#endif //PARSERINITIALIZER_H