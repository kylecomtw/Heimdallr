#include "../DepRuleNode.h"
#include "../DepRuleEngine.h"
#define EXPORT extern"C" __declspec(dllexport)

typedef DepRuleNode* DepRuleNodePtr;

const char* get_c_str(const string& s);
const char* get_cons_cstr(DepRuleNode* r, NodeConstraint nc);


EXPORT void			free_rule(DepRuleNode* rn);
EXPORT DepRuleNode* drn_parent(DepRuleNode* rn);
EXPORT const char*  drn_rule_class(DepRuleNode* rn);
EXPORT const char*  drn_rule_label(DepRuleNode* rn);
EXPORT const char*  drn_pos(DepRuleNode* rn);
EXPORT const char*  drn_word(DepRuleNode* rn);
EXPORT const char*	drn_comment(DepRuleNode* rn);

EXPORT bool			drn_head_flag(DepRuleNode* rn);
EXPORT bool			drn_optional(DepRuleNode* rn);
EXPORT int			drn_src_line(DepRuleNode* rn);
EXPORT const char*	drn_src_file(DepRuleNode* rn);

EXPORT const char*  drn_cons_class(DepRuleNode* rn);
EXPORT const char*  drn_cons_label(DepRuleNode* rn);
EXPORT const char*  drn_cons_m_class(DepRuleNode* rn);
EXPORT const char*  drn_cons_m_label(DepRuleNode* rn);

EXPORT int          drn_category(DepRuleNode* rn);
EXPORT int			drn_node_type(DepRuleNode* rn);
EXPORT int			drn_match_type(DepRuleNode* rn);
EXPORT int			drn_conn_relation(DepRuleNode* rn);
EXPORT int			drn_anchor(DepRuleNode* rn);

EXPORT int			drn_children_count(DepRuleNode* rn);
EXPORT void			drn_children(DepRuleNode* rn, int n, DepRuleNode** rn_arr);
EXPORT const char*  drn_to_node_string(DepRuleNode* rn, string** node_str);
EXPORT void			drn_free_node_string(string** node_str);
EXPORT DepRuleNode* parse_rule(const char*);

//! DepRuleEngine interface
EXPORT DepRuleEngine*	init_rule_engine();
EXPORT void			reset_rule_engine(DepRuleEngine* ptr);
EXPORT void			free_rule_engine(DepRuleEngine* ptr);
EXPORT bool			get_parse_error_state(DepRuleEngine* ptr);
EXPORT void			engine_post_proc(DepRuleEngine* ptr);
EXPORT void			parse_file(DepRuleEngine* engine_ptr, const char* in_path);
EXPORT int			parse_rule_text(DepRuleEngine* engine_ptr, const char* text);
EXPORT int			get_nRule(DepRuleEngine* engine_ptr);
EXPORT void			get_rule_list(DepRuleEngine* engine_ptr, DepRuleNodePtr** node_list, int n);
EXPORT void			dre_set_context_to_all(DepRuleEngine* engine_ptr);
EXPORT PC_RuleList*	dre_query_bu(DepRuleEngine* engine_ptr, DepRuleNode* drn);
EXPORT PC_RuleList*	dre_query_td(DepRuleEngine* engine_ptr, DepRuleNode* drn);
EXPORT PC_RuleList*	dre_find_rules(DepRuleEngine* engine_ptr, DepRuleNode* drn);
EXPORT int			dre_PC_RuleList_count(PC_RuleList* rlist);
EXPORT void			dre_get_PC_RuleList(PC_RuleList* rlist, int n, DepRuleNode** drn_arr);
EXPORT void			dre_free_PC_RuleList(PC_RuleList* rlist);
