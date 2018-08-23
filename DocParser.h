#ifndef DOCPARSER_H
#define DOCPARSER_H
#include "PropNodeGraph.h"


class DocParser
{
private:
	vector<string>	rule_path_vec;
	vector<string>	lex_marker_strvec;
	void parse_data(vector<vector<DepInfo>>& dep_data, PropNodeGraph& pn_graph);
	void parse_seq(vector<DepInfo>& dep_seq, PropNodeGraph& pn_graph);
	int parse_iter_max = 5000;

public:
	void parse_file(const string& seg_path, PropNodeGraph& pn_graph);
	void parse_text(const string& text, PropNodeGraph& pn_graph);
	void build_graph(PropNodeGraph& pn_graph);
	void import_rule_file(const string& rule_path);
	void add_lexical_marker_target(const string& s);
	void set_iter_max(int v) { parse_iter_max = v; }

};

#endif //DOCPARSER_H
