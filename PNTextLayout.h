#ifndef PNTEXTLAYOUT_H
#define PNTEXTLAYOUT_H

#include "PropNodeGraph.h"
#include <unordered_map>

typedef MatchedDepRule MRule;
typedef std::unordered_map<const MRule*, string> CPMRuleStrMap;

class PNTextLayout
{
private:
	int			  s_counter;
	int			  c_counter;
	CPMRuleStrMap mrule_code_map;

	void		  initialize_rule_vec(const PropNodeGraph& graph, vector<const MRule*>& mrule_vec);
	void		  index_mrule_code(const vector<const MRule*>& mrule_vec);
	const string& get_code(const MRule* mrule_x);
	string		  get_arg_form(const MRule* mrule_x);
	string		  get_lex_marker(const MRule* mrule_x);
	void transform_interseq(const MRule* mrule_x, std::ofstream& ofs);
	void transform_chunk(const MRule* mrule_x, std::ofstream& ofs);
public:
	PNTextLayout();
	void transform(const PropNodeGraph& graph, string fpath);
};

#endif //PNTEXTLAYOUT_H