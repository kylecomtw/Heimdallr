#ifndef DEPPARSERSTATE_H
#define DEPPARSERSTATE_H

#include "DepParser.h"
#include <string>
#include <vector>
using std::string;
using std::vector;

class DepParserState
{
private:
	const DepParser& parser;
	vector<DepToken>* input;
	vector<int> head_vec;
	vector<int> dep_vec;
	vector<int> lc_vec;
	vector<int> rc_vec;
	vector<int> ls_vec;
	vector<int> rs_vec;
	
public:
	DepParserState(const DepParser& _parser);
	~DepParserState();

	const vector<DepToken>* get_input() const;
	int get_input_cursor() const;
	const std::stack<DepToken> get_stack() const;	
	const vector<int>& get_head_vec() const;
	const vector<int>& get_dep_vec() const;
	const vector<int>& get_lc_vec() const;
	const vector<int>& get_rc_vec() const;
	const vector<int>& get_ls_vec() const;
	const vector<int>& get_rs_vec() const;

	void sync_callback();
	
};

#endif //DEPPARSERSTATE_H