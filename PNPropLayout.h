#ifndef PNPROPLAYOUT_H
#define PNPROPLAYOUT_H
#include <string>
#include <vector>
#include "DepRuleNode.h"
#include "PropNodeGraph.h"
#include <unordered_map>
#include "PNPropArg.h"

using namespace std;
typedef MatchedDepRule MRule;
typedef unordered_map<const MRule*, string> CPMRuleStrMap;
typedef vector<PNProp> PNPropVector;


class PNPropLayout
{
private:
	int			  s_counter;
	int			  c_counter;
	CPMRuleStrMap mrule_code_map;

	void		  initialize_rule_vec(const PropNodeGraph& graph, vector<const MRule*>& mrule_vec);
	void		  index_mrule_code(const vector<const MRule*>& mrule_vec);
	const string& get_code(const MRule* mrule_x);
	PropArg		  get_arg_form(const MRule* mrule_x);
	string		  get_lex_marker(const MRule* mrule_x);
	void transform_interseq(const MRule* mrule_x, PNPropVector& prop_vec);
	void transform_chunk(const MRule* mrule_x, PNPropVector& prop_vec);
public:
	PNPropLayout();
	void transform(const PropNodeGraph& graph, PNPropVector& prop_vec);
};

#endif //PNPROPLAYOUT_H
