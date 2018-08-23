#ifndef PARSERINITFIRSTTERMINAL_H
#define PARSERINITFIRSTTERMINAL_H

#include "ParserInitializer.h"

class ParserInitFirstTerminal: public ParserInitializer
{
public:
	ParserInitFirstTerminal(IParserInitializable* p) :
		ParserInitializer(*p){}

	virtual void initialize_parser() override
	{
		vector<EdgeType*> term_edges = parser.get_term_edges();

		if (term_edges.size() > 0){
			//! find all rules predicted by terminal symbols		
			parser.move_to_agenda(term_edges[0]);
		}
	}
};

#endif //PARSERINITFIRSTTERMINAL_H