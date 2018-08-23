

#include <string>
#include <iostream>
#include "c_interface.h"
#include "../DepRuleNode.h"

using std::string;

// int bison_parse_dep_rule(const char* buf);
extern DepRuleNodeSet* dpr_root;
typedef struct yy_buffer_state *YY_BUFFER_STATE;
YY_BUFFER_STATE yy_scan_string(const char *yy_str);
void yy_delete_buffer(YY_BUFFER_STATE b);
extern "C" int yyparse();
extern int yydebug;



const char* get_c_str(const string& s) {
	if (s.empty()) {
		return nullptr;
	} else {
		return s.c_str();
	}
}

const char* get_cons_cstr(DepRuleNode* r, NodeConstraint nc)
{
	const auto& cmap = r->get_node_constraints();
	if (cmap.count(nc)) {
		return cmap.at(nc).c_str();
	} else {
		return nullptr;
	}
}

void free_rule(DepRuleNode* rn)
{
	delete rn;
}

DepRuleNode* drn_parent(DepRuleNode* rn) { return rn->get_parent(); }
const char*  drn_rule_class(DepRuleNode* rn) { return rn->get_rule_class().c_str(); }
const char*  drn_rule_label(DepRuleNode* rn) { return rn->get_rule_label().c_str(); };
const char*  drn_pos(DepRuleNode* rn) { return rn->get_pos().c_str(); }
const char*  drn_word(DepRuleNode* rn) { return rn->get_word().c_str(); }
const char*  drn_comment(DepRuleNode* rn) { return rn->get_comment().c_str(); }

bool		 drn_head_flag(DepRuleNode* rn) { return rn->get_head_flag(); }
bool		 drn_optional(DepRuleNode* rn) { return rn->is_optional(); }
int			 drn_src_line(DepRuleNode* rn) { return rn->get_src_line(); }
const char*	 drn_src_file(DepRuleNode* rn) { return rn->get_src_file().c_str(); }

const char*  drn_cons_class(DepRuleNode* rn) { return get_cons_cstr(rn, NodeConstraint::RuleClass); }
const char*  drn_cons_label(DepRuleNode* rn) { return get_cons_cstr(rn, NodeConstraint::RuleLabel); }
const char*  drn_cons_m_class(DepRuleNode* rn) { return get_cons_cstr(rn, NodeConstraint::MatcherClassCons); }
const char*  drn_cons_m_label(DepRuleNode* rn) { return get_cons_cstr(rn, NodeConstraint::MatcherLabelCons); }

int          drn_category(DepRuleNode* rn) { return (int)rn->get_category(); }
int			 drn_node_type(DepRuleNode* rn) { return (int)rn->get_node_type(); }
int 		 drn_match_type(DepRuleNode* rn) { return (int)rn->get_match_type(); }
int	 		 drn_conn_relation(DepRuleNode* rn) { return (int)rn->get_node_connection(); }
int	   		 drn_anchor(DepRuleNode* rn) { return (int)rn->get_anchor(); }

int 		 drn_children_count(DepRuleNode* rn) { return rn->get_children_count(); }

void drn_children(DepRuleNode* rn, int n, DepRuleNode** rn_arr)
{
	int n_child = rn->get_children_count();
	for (int i = 0; i < n; ++i) {
		if (i >= n_child) break;
		rn_arr[i] = rn->get_children(i);		
	}
}

const char* drn_to_node_string(DepRuleNode* rn, string** node_str)
{
	string str = rn->to_node_string();
	*node_str = new string(str);
	const char* ret = (*node_str)->c_str();
	return ret;
}

void drn_free_node_string(string** node_str)
{
	delete *node_str;
}

DepRuleNode* parse_rule(const char* rbuf)
{
	yydebug = 0;
	YY_BUFFER_STATE ybuf = yy_scan_string(rbuf);
	yyparse();
	yy_delete_buffer(ybuf);

	DepRuleNodeList* node = dpr_root->vec;
	if (node->size() > 0){
		std::cout << "Parsing success" << std::endl;
		return (*node)[0];
		// std::cout << (*node)[0]->to_detail_str() << std::endl;
	} else {
		return nullptr;
	}
	
}

DepRuleEngine* init_rule_engine()
{
	DepRuleEngine* rule_engine = new DepRuleEngine();
	return rule_engine;
}

void reset_rule_engine(DepRuleEngine* ptr)
{
	ptr->reset();
}

void free_rule_engine(DepRuleEngine* ptr)
{
	delete ptr;
}

bool get_parse_error_state(DepRuleEngine* ptr)
{
	return ptr->parse_error();
}

void engine_post_proc(DepRuleEngine* ptr)
{
	ptr->post_process_rules();
}

void parse_file(DepRuleEngine* engine_ptr, const char* in_path)
{
	engine_ptr->import_dep_rules(string(in_path));
}

int	parse_rule_text(DepRuleEngine* engine_ptr, const char* text)
{
	return engine_ptr->parse_rule_text(text);
}

int get_nRule(DepRuleEngine* engine_ptr)
{
	return engine_ptr->get_rules_vec().size();
}

void get_rule_list(DepRuleEngine* engine_ptr, DepRuleNodePtr** node_list_ptr, int n)
{
	const RuleList& rlist = engine_ptr->get_rules_vec();	
	if (n != rlist.size()) return;

	DepRuleNodePtr* node_list = *node_list_ptr;
	for (int i = 0; i < n; ++i) {		
		node_list[i] = rlist[i];
	}		
	
}

void free_rule_list(DepRuleNode** ptr)
{
	delete[] ptr;
}

void dre_set_context_to_all(DepRuleEngine* engine_ptr)
{
	engine_ptr->set_context_to_all();
}

PC_RuleList* dre_query_bu(DepRuleEngine* engine_ptr, DepRuleNode* drn)
{
	const ConstraintMapType& nc = drn->get_node_constraints();
	string rc, rl;
	if (nc.count(NodeConstraint::RuleClass) > 0) {
		rc = nc.at(NodeConstraint::RuleClass);
	}

	if (nc.count(NodeConstraint::RuleLabel) > 0) {		
		rl = nc.at(NodeConstraint::RuleLabel);
	}

	drn->set_info(0, rc, rl);
	MatchedDepRule* mrule = MatchedDepRule::make_matched_node(drn);
	PC_RuleList rlist = engine_ptr->predict_bu(*mrule);
	delete mrule;

	drn->set_info(0, "", "");
	return new PC_RuleList(rlist);
	
	
}

PC_RuleList* dre_query_td(DepRuleEngine* engine_ptr, DepRuleNode* drn)
{
	
	MatchedDepRule* mrule = MatchedDepRule::make_matched_node(drn);
	PC_RuleList rlist = engine_ptr->predict_td(*mrule);
	delete mrule;
	return new PC_RuleList(rlist);

}

PC_RuleList* dre_find_rules(DepRuleEngine* engine_ptr, DepRuleNode* drn)
{

	PC_RuleList rlist = engine_ptr->find_rules(drn);
	return new PC_RuleList(rlist);
}

int	dre_PC_RuleList_count(PC_RuleList* rlist)
{
	return rlist->size();
}

void dre_get_PC_RuleList(PC_RuleList* rlist, int n, DepRuleNode** drn_arr)
{
	int n_list = rlist->size();
	for (int i = 0; i < n; ++i) {
		if (i >= n_list) break;
		drn_arr[i] = const_cast<DepRuleNode*>(rlist->at(i));
	}
}

void dre_free_PC_RuleList(PC_RuleList* rlist)
{
	delete(rlist);
}




