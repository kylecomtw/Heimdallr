#include "EdgeInfoParser.h"

EdgeInfoParser::EdgeInfoParser()
{

}

int EdgeInfoParser::parse(const string& str, vector<EdgeInfoType*>& ret_vec)
{
	string::const_iterator iter_end = str.end();
	string::const_iterator iter;
	lineno = 1; n_term = 0;
	error_flag = 0;
	for (iter = str.begin(); iter != iter_end; ++iter){
		char ch = *iter;
		int ascii = (int)ch;

		if (ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') {
			if (ch == '\n') lineno += 1;
			continue;
			//! ignore it otherwise
		}

		check_states(ret_vec);

		if (ascii >= 65 && ascii <= 90){ // A-Z
			add_alphanum(ch);
		} else if (ascii >= 97 && ascii <= 122){ // a-z
			add_alphanum(ch);
		} else if (ascii >= 48 && ascii <= 57){ // 0-9
			add_alphanum(ch);
		} else if (ch == '_'){ // 0-9
			add_alphanum(ch);		
		} else if (ch == '\"') {
			//! toggle ST_TEXT
			ParserState& st = states.top();
			if (st.state == ST_TEXT) st.state = ST_BASE;
			else {
				n_term += 1;
				st.state = ST_TEXT;
			}
		} else if (ch == ':') {
			ParserState& st = states.top();
			if (st.state != ST_POS) st.state = ST_POS;
			else {
				printf("unexpected \":\" at line %d\n", lineno);
				error_flag = 1;
			}
		} else if (ch == '(') {
			create_children();
		} else if (ch == ')') {
			complete_edge();
		} else if (ch == '|') {
			next_sibling();
		} else if (ch == '*') {
			ParserState& st = states.top();
			st.edge_info_ptr->is_head = true;
			if (st.edge_info_ptr->parent){
				st.edge_info_ptr->parent->head_ptr = st.edge_info_ptr;
			}
		} else { // see as character in ideographs
			add_text(ch);
		}
	}

	while (!states.empty()){
		complete_edge();
	}
	return error_flag;
}

void EdgeInfoParser::check_states(vector<EdgeInfoType*>& ret_vec)
{
	if (states.empty()){
		ParserState st;
		st.state = ST_RULE_LABEL;
		st.edge_info_ptr = new EdgeInfoType();
		st.edge_info_ptr->start = n_term;
		ret_vec.push_back(st.edge_info_ptr);
		states.push(st);
	}
}

void EdgeInfoParser::add_alphanum(char ch)
{
	ParserState& st = states.top();
	EdgeInfoType* einfo = st.edge_info_ptr;
	if (st.state == ST_RULE_LABEL) einfo->rule_label.push_back(ch);
	else if (st.state == ST_POS) einfo->pos.push_back(ch);
	else{
		printf("Parsing state error at add_alphanum\n");
		error_flag = 1;
	}

}

void EdgeInfoParser::add_text(char ch)
{
	ParserState& st = states.top();
	EdgeInfoType* einfo = st.edge_info_ptr;
	if (st.state == ST_TEXT) einfo->word.push_back(ch);
	else {
		printf("unexpected text at line %d\n", lineno);
		error_flag = 1;
	}
}

void EdgeInfoParser::create_children()
{
	ParserState& st = states.top();
	EdgeInfoType* einfo = st.edge_info_ptr;
	EdgeInfoType* einfo_ch = new EdgeInfoType();
	einfo_ch->parent = einfo;
	einfo_ch->start = n_term;
	einfo->children.push_back(einfo_ch);

	ParserState st_ch;
	st_ch.state = ST_RULE_LABEL;
	st_ch.edge_info_ptr = einfo_ch;

	states.push(st_ch);
}

void EdgeInfoParser::complete_edge()
{
	ParserState& st = states.top();
	st.edge_info_ptr->end = n_term;	
	st.edge_info_ptr->dot_pos = st.edge_info_ptr->children.size();
	states.pop();
}

void EdgeInfoParser::next_sibling()
{	
	complete_edge();
	create_children();
}