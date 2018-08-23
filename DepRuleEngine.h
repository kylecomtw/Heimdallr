#ifndef DEPRULESET_H
#define DEPRULESET_H

#include <array>
#include "common.h"
#include "DepRuleNode.h"
#include "DepData.h"
#include "MatchedDepRule.h"
#include <functional>

typedef std::function<bool(const DepRuleNode*)> Rule_Predicate;
typedef vector<DepRuleNode*> RuleList;
typedef vector<const DepRuleNode*> PC_RuleList;

class DepRuleEngine{
private:
	const string CAT_SEQDELIM = "SeqDelim";
	const string LAB_PREFIX_QUOTEND = "QuoteEnd";
	const string LAB_PREFIX_SEQDELIM = "SeqDelim";
	RuleList    rules_vec;	
	RuleList	black_rules;
	RuleList    precedence_rules;
	PC_RuleList	rules_vec_ctx;
	vector<string> lm_categories;
	bool has_parse_error;
	bool ctx_ready;
	int assign_default_info();
	int assign_rule_head(DepRuleNode* rule_x);	
	int assign_category(DepRuleNode* rule);	
	int split_word(DepRuleNode* rule, int child_pos, DepRuleNode* term);
	int expand_match_options(DepRuleNode* rule);
	int expand_optionals(DepRuleNode* rule);
	int expand_anchor(DepRuleNode* _rule);	
	DepRuleNode* replace_dummy_as_new_rule(DepRuleNode* parent, int child_pos, DepRuleNode* or_node);
	DepRuleNode* expand_dummyNode(DepRuleNode* parent, int child_pos, DepRuleNode* dummy_node);
	int initialize_blacklist();
	int initialize_precedences();
	int is_any_children_blacklisted(const vector<const MatchedDepRule*>& mrules) const;	

public:
	DepRuleEngine();
	~DepRuleEngine();
	PC_RuleList predict_td(const MatchedDepRule& drule) const;
	PC_RuleList predict_bu(const MatchedDepRule& drule) const;
	PC_RuleList predict(const DepToken& token) const;
	
	void reset();
	int split_and_expand_dummy(DepRuleNode* rule);
	int save(const string& fpath);
	bool parse_error() { return has_parse_error; }	
	void set_context(const vector<DepToken>& seq_ctx);
	void set_context(const vector<vector<DepInfo>>& dep_data);
	void set_context_to_all();
	void add_lexical_marker_category(const string& cat);
	void reset_context();
	void switch_mode(bool use_inter_seq_mode);
	void enable_category(RuleCategory);
	int parse_rule_text(const string& text);
	int import_dep_rules(const string& fpath);		
	int post_process_rules();
	const DepRuleNode* find_rule(const string& rule_label) const;
	const DepRuleNode* find_rule_dynamic(const string& rule_class, const string& rule_label, int n_slot);
	const DepRuleNode* create_seq_quot_end_rule(int n_slot, const string& rlabel);
	const DepRuleNode* create_seq_delim_rule(int n_slot, const string& rlabel, bool has_delim);
	const DepRuleNode* create_container_rule(const vector<const MatchedDepRule*>& mrule_vec);
	const DepRuleNode* create_container_rule(int n_slot, const string& rclass, const string& rlabel);
	const DepRuleNode* is_blacklisted(const MatchedDepRule* mrule) const;
	bool check_precedence_rules(const MatchedDepRule* mrule) const;
	PC_RuleList find_rules(vector<const DepRuleNode*>) const;
	PC_RuleList find_rules(const DepRuleNode*) const;
	int rule_count() const { return rules_vec.size(); }
	DepRuleNodeList get_rules_ptr(RuleCategory categ);
	RuleList filter_rules(Rule_Predicate func);
	RuleList get_rules_vec();
	
};

#endif //DEPRULESET_H