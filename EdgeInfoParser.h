#ifndef EDGEINFOPARSER_H
#define EDGEINFOPARSER_H

#include "EdgeInfoType.h"
#include <string>
#include <vector>
#include <stack>

using std::string;
using std::vector;

class EdgeInfoParser
{
private:
	int lineno;
	int n_term;
	int error_flag;
	const int ST_BASE = 0;
	const int ST_RULE_LABEL = 1;
	const int ST_POS = 2;
	const int ST_TEXT = 3;	
	struct ParserState{ int state; EdgeInfoType* edge_info_ptr; };
	std::stack<ParserState> states;
	EdgeInfoType* edge_ptr;

	void check_states(vector<EdgeInfoType*>& ret_vec);
	void add_alphanum(char ch);
	void add_text(char ch);
	void create_children();
	void complete_edge();
	void next_sibling();
public:
	EdgeInfoParser();
	int parse(const string& str, vector<EdgeInfoType*>& ret_vec);

};

#endif //EDGEINFOPARSER_H