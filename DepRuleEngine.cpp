#include "DepRuleEngine.h"
#include "DepRuleNode.h"
#include "ParseException.h"
#include <regex>
#include <vector>
#include <unordered_map>
#include <stack>
#include <sstream>

using std::vector;
extern DepRuleNodeSet* dpr_root;
int bison_parse_dep_rule_file(const char* fpath);
typedef struct yy_buffer_state *YY_BUFFER_STATE;
YY_BUFFER_STATE yy_scan_string(const char *yy_str);
void yy_delete_buffer(YY_BUFFER_STATE b);
void yy_reset_buffer();
extern "C" int yyparse();
extern int yydebug;
extern int yylineno;
string g_rule_file_name = "";

DepRuleEngine::DepRuleEngine()
{
	ctx_ready = false;
	lm_categories.push_back("LexicalMarker");
	lm_categories.push_back("FrameRule");
	lm_categories.push_back("PUNC");
}

DepRuleEngine::~DepRuleEngine()
{
	reset();
}

PC_RuleList DepRuleEngine::predict(const DepToken& token) const
{
	_ASSERT(false);
	PC_RuleList buf;
	if (!ctx_ready){
		printf("[WARNING] context not set in DepRuleEngine. call set_context first\n");
	}
	for (const DepRuleNode* rule_x : rules_vec_ctx){
		const DepRuleNode* first_term = rule_x->get_children(0);
		_ASSERT(first_term->get_children_count() < 10);
		const string& word = first_term->get_word();
		const string& pos = first_term->get_pos();
		if ((word.size() == 0 || word == token.word) && 
			(pos.size() == 0 || pos == token.pos)){			
			buf.push_back(rule_x);
		}
	}

	return buf;
}

PC_RuleList DepRuleEngine::predict_td(const MatchedDepRule& mrule) const
{
	PC_RuleList buf;
	if (!ctx_ready){
		printf("[WARNING] context not set in DepRuleEngine. call set_context first\n");
	}

	const DepRuleNode* to_complete = mrule.next_rule_node();

	// do not forward prediction with a passive node
	if (to_complete->get_passive() == true) return buf;

	// given a drule: A -> a . B c
	// found rules that can generate a B (to_complete)
	for (const DepRuleNode* rule_x : rules_vec_ctx){
		if (!rule_x->get_enabled()) continue;
		if (rule_x->can_generate(to_complete) && 
			!rule_x->is_start_passive() &&
			rule_x->match_position(mrule.end_pos(), -1) &&
			rule_x->match_anchor(to_complete->get_anchor())){
			buf.push_back(rule_x);
		}
	}

	//! extra add-on for precedence rules
	//! (is there a better place to do this?)
	for (DepRuleNode* drule : precedence_rules) {
		if (drule->get_start_pos_hint() >= 0 &&
			drule->get_start_pos_hint() == mrule.end_pos()) {
			buf.push_back(drule);
		}
	}

	return buf;
}

PC_RuleList DepRuleEngine::predict_bu(const MatchedDepRule& mrule) const 
{
	PC_RuleList buf;
	if (!ctx_ready){
		printf("[WARNING] context not set in DepRuleEngine. call set_context first\n");
	}

	const DepRuleNode* completed = &mrule.get_rule_node();

	//debug
	// int aa = drule.start_pos(); int bb = drule.end_pos();

	// given a drule (completed): B -> r .
	// found rules in which B generates starting node, i.e. A -> B c
	for (const DepRuleNode* rule_x : rules_vec_ctx){
		if (!rule_x->get_enabled()) continue;
		// check if to_complete match the first leaf node in rule_x
		string cc = rule_x->get_rule_label();	

		const DepRuleNode* rule_start_node = rule_x->get_children(0);
		if (completed->can_generate(rule_start_node) &&
			rule_start_node->match_position(mrule.start_pos(), mrule.end_pos()) && 
			rule_start_node->match_anchor(mrule.matched_anchor())) {

			//! passive tree does not self-procreate
			if (rule_x->is_start_passive() == true && 
				completed->get_rule_label() == rule_x->get_rule_label()) continue;

			// edge_x contain nodes match to_complete node in edge
			buf.push_back(rule_x);
		}
	}

	//! extra add-on for precedence rules
	//! (is there a better place to do this?)
	for (DepRuleNode* drule : precedence_rules) {
		if (drule->get_start_pos_hint() >= 0 && 
			drule->get_start_pos_hint() == mrule.start_pos()) {
			buf.push_back(drule);
		}
	}

	return buf;
}

int DepRuleEngine::is_any_children_blacklisted(const vector<const MatchedDepRule*>& mrules) const
{

	for (int i = 0; i < mrules.size(); ++i) {		
		auto mr = mrules.at(i);

		if (mr == nullptr) continue;
		if (is_blacklisted(mr)) return i;
	}

	return -1;
}

const DepRuleNode* DepRuleEngine::is_blacklisted(const MatchedDepRule* mrule) const
{	
	//! we only check an inactive mrule here
	if (mrule->is_active()) return nullptr;
	if (mrule->start_pos() == 5 && mrule->end_pos() == 10 && mrule->matched_rule_class() == "Sen") {
		int a = 1;
	}
	for (DepRuleNode* dn : black_rules) {
		bool is_matched = true;

		//! check mrule itself is a blacklisted rule
		if (mrule->is_realization_of(dn)) {
			dprintf("%s is blacklisted by %s\n", ((string)*mrule).c_str(), dn->get_rule_label());
			return dn;
		}

		//! check if mrule contains the same components as the blacklisted rule
		int dn_nchild = dn->get_children_count();
		if (dn_nchild != mrule->get_children_count()) continue;
		for (int i = 0; i < dn_nchild; ++i) {
			const DepRuleNode* dn_ch = dn->get_children(i);
			const MatchedDepRule* mrule_ch = mrule->get_matched_children(i);
			if (!mrule_ch->is_realization_of(dn_ch)) {
				is_matched = false;
				break;
			}
		}

		if (is_matched) {
			dprintf("%s is blacklisted by %s\n", ((string)*mrule).c_str(), dn->get_rule_label());
			return dn;
		}
	}

	return nullptr;
}

bool DepRuleEngine::check_precedence_rules(const MatchedDepRule* mrule) const
{
	//! I. range check: mrule does not overlap with precedence rules are pass by default
	bool is_overlap = false;	
	for (auto pdn : precedence_rules) {
		int pdn_start = pdn->get_start_pos_hint();
		int pdn_end = pdn->get_end_pos_hint();
		if ((pdn_start >= mrule->start_pos() && pdn_start < mrule->end_pos()) ||
			(pdn_end > mrule->start_pos() && pdn_end <= mrule->end_pos()) ||
			(pdn_start >= mrule->start_pos() && pdn_end <= mrule->end_pos()))
		{			
			is_overlap = true; break;
		}
	}

	if (!is_overlap) return true;

	//! II. Enclosing check: mrule whose range overlapped with precedence rules should contains precedence rules
	vector<const MatchedDepRule*> buf;
	buf.push_back(mrule);
	RuleList prec_copy(precedence_rules);

	while (buf.size() > 0) {
		const MatchedDepRule* mrule_x = buf.back();
		buf.pop_back();
		for (auto m_ch : mrule_x->get_matched_children()) {
			buf.push_back(m_ch);
		}

		//! check if prec_iter is used in mrule_x.
		//! delete prec_iter if it appeared in mrule_x
		if (prec_copy.size() == 0) break;
		auto prec_iter = prec_copy.cbegin();		
		while (prec_iter != prec_copy.cend()) {
			if (mrule_x->get_rule_node().get_rule_label() ==
				(*prec_iter)->get_rule_label()) {
				prec_iter = prec_copy.erase(prec_iter);
			} else {
				++prec_iter;
			}
		}

	}

	return prec_copy.size() == 0;
}

void DepRuleEngine::add_lexical_marker_category(const string& cat)
{
	lm_categories.push_back(cat);
}

void DepRuleEngine::set_context_to_all()
{
	rules_vec_ctx.clear();
	for (const DepRuleNode* rn : rules_vec){
		rules_vec_ctx.push_back(rn);
	}

	ctx_ready = true;
}

void DepRuleEngine::set_context(const vector<vector<DepInfo>>& dep_data)
{	
	vector<DepToken> dep_vec;
	
	for (int j = 0; j < dep_data.size(); ++j){
		const vector<DepInfo>& dep_info_vec = dep_data.at(j);
		for (int i = 0; i < dep_info_vec.size(); ++i){
			DepToken dt;
			dt.index = i; dt.word = dep_info_vec.at(i).word;
			dt.pos = dep_info_vec.at(i).pos;
			dep_vec.push_back(std::move(dt));
		}
	}

	set_context(dep_vec);
}

void DepRuleEngine::set_context(const vector<DepToken>& seq_ctx)
{
	reset_context();
	for (int j = 0; j < rules_vec.size(); ++j){
		const DepRuleNode* rnode = rules_vec.at(j);
		bool in_context = true;

		for (int k = 0; k < rnode->get_children_count(); ++k){
			const DepRuleNode* ch_rnode = rnode->get_children(k);
			if (ch_rnode->get_node_type() != NodeType::NdTerminal) continue;

			const DepRuleNode* term_rnode = ch_rnode;
			
			bool found_in_ctx = false;
			for (int i = 0; i < seq_ctx.size(); ++i){
				const DepToken& di = seq_ctx.at(i);
				found_in_ctx |= (term_rnode->get_word().size() == 0 || di.word == term_rnode->get_word()) &&
								(term_rnode->get_pos().size() == 0 || di.pos == term_rnode->get_pos());
				if (found_in_ctx) break;
			}				

			// dprintf("%s: found in context [%d]\n", term_rnode->to_node_string().c_str(), found_in_ctx);
			in_context &= found_in_ctx;
			if (!in_context) break;
		}

		if (in_context){
			rules_vec_ctx.push_back(rnode);
		}
	}

	ctx_ready = true;
	dprintf("Select %d rules in context from %d rules\n", rules_vec_ctx.size(), rules_vec.size());
}

void DepRuleEngine::switch_mode(bool use_inter_seq_mode)
{
	if (use_inter_seq_mode) {
		for (auto x : rules_vec) {
			if (x->get_category() != RuleCategory::InterSeq)
				x->set_enabled(false);
			else
				x->set_enabled(true);
		}
	} else {
		for (auto x : rules_vec) {
			if (x->get_category() == RuleCategory::InterSeq)
				x->set_enabled(false);
			else
				x->set_enabled(true);
		}
	}
}

void DepRuleEngine::enable_category(RuleCategory rc)
{
	for (auto x : rules_vec) {
		if (x->get_category() == rc) {
			x->set_enabled(true);
		}
	}
}

void DepRuleEngine::reset_context()
{
	rules_vec_ctx.clear();
	for (auto x : rules_vec) x->set_enabled(true);
	ctx_ready = false;
}

int DepRuleEngine::save(const string& fpath)
{
	return 0;
}

void DepRuleEngine::reset()
{
	for (auto x : rules_vec) delete x;
	for (auto x : black_rules) delete x;
	rules_vec.clear();
	black_rules.clear();
}

int DepRuleEngine::parse_rule_text(const string& rbuf)
{
	if (rbuf.size() == 0) {
		return 0;
	}

	yydebug = 0;
	int ret = 0;
	g_rule_file_name = "";

	YY_BUFFER_STATE ybuf = yy_scan_string(rbuf.c_str());
	has_parse_error = false;
	try {
		yylineno = 1;
		yyparse();

		if (dpr_root != NULL) {
			DepRuleNodeList* nodes = dpr_root->vec;
			if (nodes->size() > 0) {
				rules_vec.insert(rules_vec.end(), nodes->begin(), nodes->end());
				// std::cout << (*node)[0]->to_detail_str() << std::endl;
			}
		} else {
			throw ParseException(0);
		}
		
	} catch (ParseException& ex) {		
		ret = ex.get_position();
		yy_reset_buffer();
		yy_delete_buffer(ybuf);
		ybuf = NULL;
		has_parse_error = true;
		// delete dpr_root;
		return ret;
	}

	yy_delete_buffer(ybuf);
	ybuf = NULL;
	delete dpr_root;
	return -1;
	
}

int DepRuleEngine::import_dep_rules(const string& fpath)
{		
	const char* fpath_cstr = fpath.c_str();
	has_parse_error = false;

	int substr_idx = fpath.find_last_of("/");
	if (substr_idx != string::npos) {
		g_rule_file_name = fpath.substr(substr_idx+1);
	} else {
		g_rule_file_name = fpath;
	}

	yy_reset_buffer();
	try{
		int ret = bison_parse_dep_rule_file(fpath_cstr);
		if (ret < 0) {
			return -1;
		}
	} catch (ParseException& ex){
		has_parse_error = true;
		yy_reset_buffer();
		// delete dpr_root;
		return ex.get_position();
	}
	
	dprintf("Parsed %d rules from %s\n", dpr_root->vec->size(), fpath.c_str());
	
	DepRuleNodeList& rlist = *dpr_root->vec;	
	rules_vec.insert(rules_vec.end(), rlist.begin(), rlist.end());
	delete dpr_root;

	return -1;
}

RuleList DepRuleEngine::get_rules_vec()
{	
	return rules_vec;
}

RuleList DepRuleEngine::filter_rules(Rule_Predicate func)
{
	RuleList ret_list;
	std::copy_if(rules_vec.begin(), rules_vec.end(), 
		std::back_inserter(ret_list), func);
	return ret_list;
}

DepRuleNodeList DepRuleEngine::get_rules_ptr(RuleCategory categ)
{
	DepRuleNodeList ret_vec;
	std::copy_if(rules_vec.begin(), rules_vec.end(), std::back_inserter(ret_vec),
		[categ](DepRuleNode* rptr){return rptr->get_category() == categ; });
	return ret_vec;
}

int DepRuleEngine::initialize_blacklist()
{
	auto iter = rules_vec.cbegin();
	while (iter != rules_vec.cend()) {
		if ((*iter)->get_priority() < 0) {
			black_rules.push_back(*iter);
			iter = rules_vec.erase(iter);
		} else {
			++iter;
		}
	}

	return 0;
}

int DepRuleEngine::initialize_precedences()
{
	auto iter = rules_vec.cbegin();
	while (iter != rules_vec.cend()) {
		const string& label = (*iter)->get_rule_label();
		if (label.size() >= 3 &&
			label.substr(label.size() - 3) == "_PR") 
		{
				precedence_rules.push_back(*iter);
		}
		
		++iter;		
	}

	return 0;
}

int DepRuleEngine::post_process_rules()
{			
	printf("Normal Rules: %d\n", rules_vec.size());
	//! I. assign default properties
	//! -----------------------------

	assign_default_info();
	
	//! II. normalize the rules
	//! ------------------------

	RuleList rule_buf(rules_vec);
	for (auto& rule_x : rule_buf)
	{
		dprintf("Raw: %s\n", ((string)*rule_x).c_str());
		expand_anchor(rule_x);
		split_and_expand_dummy(rule_x);
	}
	
	//! III. Post processing: 
	//!    a. assign rule head
	//! ------------------------
	for (auto& rule_x : rules_vec){
		assign_rule_head(rule_x);
	}

	// there might be a better options, 
	// but each step in normalization may add or remove entries in rules_vec_ptr
	// so, copy to rule_buf each time when commencing next step of normalization
	rule_buf = RuleList(rules_vec);
	for (auto& rule_x : rule_buf){
		expand_match_options(rule_x);
	}

	rule_buf = RuleList(rules_vec);
	for (auto& rule_x : rule_buf){
		expand_optionals(rule_x);
	}

	// Debug print
	printf("Normalized Rules: %d\n", rules_vec.size());
	for (auto& rule_x : rules_vec)
	{
		dprintf("Norm: %s\n", ((string)*rule_x).c_str());
	}

	initialize_blacklist(); 
	if(black_rules.size() > 0) printf("Blacklist rules: %d\n", black_rules.size());
	initialize_precedences();
	if (precedence_rules.size() > 0) printf("Precedence rules: %d\n", precedence_rules.size());

	std::sort(rules_vec.begin(), rules_vec.end(), [](DepRuleNode* a, DepRuleNode* b){		
		if (a->from_normalized == b->from_normalized){
			if (a->get_rule_class() == b->get_rule_class()){
				if (a->get_rule_label() == b->get_rule_label()){
					return false;
				} else {
					return a->get_rule_label() < b->get_rule_label();
				}
			} else {
				return a->get_rule_class() < b->get_rule_class();
			}
		} else {
			return a->from_normalized < b->from_normalized;
		}
	});

	return 0;
}

int DepRuleEngine::split_and_expand_dummy(DepRuleNode* _rule)
{
	std::stack<DepRuleNode*> rule_stack;
	rule_stack.push(_rule);
	int counter = 0;
	while (rule_stack.size() > 0){
		DepRuleNode* rule = rule_stack.top();
		rule_stack.pop();


		for (int i = 0; i < rule->get_children_count(); ++i){
			DepRuleNode* rule_x = rule->get_children(i);

			// ----------------------
			// expand the concatenation into consecutive nodes
			// ----------------------
			split_word(rule, i, rule_x);

			// --------------------
			// expand dummy node into another rule
			// --------------------			
			if (rule_x->get_node_type() == NodeType::NdOrNode){
				DepRuleNode* rule_new = replace_dummy_as_new_rule(rule, i, rule_x);
				//! rule_x is replaced by a new node in rule, delete it
				delete rule_x;
				rule_stack.push(rule_new);
			} else if (rule_x->get_node_type() == NodeType::NdDummy){
				DepRuleNode* rule_new = nullptr;
				
				if (rule->get_match_type() != MatchType::MatOr && !rule_x->is_optional()){
					//! only expand the dummy inplace when this node is not optional
					//! and it does not occur in a Or-Node
					//! Actaully, dummy node is only expanded when the the grouping is redundant.
					rule_new = expand_dummyNode(rule, i, rule_x);
					rules_vec.erase(std::remove(rules_vec.begin(), rules_vec.end(), rule), rules_vec.end());					
					delete rule;
					rule_stack.push(rule_new);
					//! rule is modified by expanding the dummyNode
					//! the copied rule is push into the rule_stack buffer
					//! present rule does not need furthur process
					break;
				} else {
					rule_new = replace_dummy_as_new_rule(rule, i, rule_x);
					rule_stack.push(rule_new);
					delete rule_x;
				}
				

			} else {
				rule_stack.push(rule_x);
			}

		}
	}

	return 0;
}

int DepRuleEngine::split_word(DepRuleNode* rule, int child_pos, DepRuleNode* term)
{
	if (term->get_node_type() != NodeType::NdTerminal) {
		return -1;
	}

	const string& word = term->get_word();
	std::stringstream ss(word);
	string tok;
	int counter = 0;
	if (word.find('/') != string::npos){
		while (std::getline(ss, tok, '/')){
			DepRuleNode* rule_word = new DepRuleNode(NodeType::NdTerminal, tok.c_str(), "", -1, rule);
			if (counter == 0){
				// the first token just replace the word in original node
				term->set_word(tok);
			} else {
				// insert new node after the original node
				rule->insert_child(child_pos + counter, rule_word);
			}
			++counter;
		}
	}

	return 0;
}

DepRuleNode* DepRuleEngine::expand_dummyNode(DepRuleNode* parent, int child_pos, DepRuleNode* dummy_node)
{
	// rule_new will be a new rule in engine, and hold the children of dummy node
	if (parent->get_match_type() == MatchType::MatOr){
		string suffix = "D";
		const int RULE_COST = 0;
		DepRuleNode* rule_new = DepRuleNode::make_from_dummy(dummy_node);
		rule_new->set_category(parent->get_category());		
		string new_class = parent->get_rule_label() + "_" + suffix;
		string new_label = parent->get_rule_label() + "_" + suffix + "_" + std::to_string(child_pos);
		rule_new->set_info(RULE_COST, new_class, new_label);
	} else {
		DepRuleNode* copied_rule = new DepRuleNode(*parent);

		copied_rule->remove_child(child_pos);
		// copied_rule->from_normalized = true;

		for (int i = 0; i < dummy_node->get_children_count(); ++i){
			DepRuleNode* chnode = new DepRuleNode(*dummy_node->get_children(i));
			copied_rule->insert_child(child_pos + i, chnode);
		}

		rules_vec.push_back(copied_rule);

		return copied_rule;
	}


		
}

DepRuleNode* DepRuleEngine::replace_dummy_as_new_rule(DepRuleNode* parent, int child_pos, DepRuleNode* dummy_node)
{
	// rule_new will be a new rule in engine, and hold the children of dummy node
	

	DepRuleNode* rule_new = DepRuleNode::make_from_dummy(dummy_node);
	rule_new->set_category(parent->get_category());
	rule_new->set_optional(false);

	string suffix;
	int rule_cost = 0;
	if (dummy_node->get_node_type() == NodeType::NdDummy){
		suffix = "D";
	} else if (dummy_node->get_node_type() == NodeType::NdOrNode) {
		suffix = "Or";
	}

	string new_class = parent->get_rule_label() + "_" + suffix;
	string new_label = parent->get_rule_label() + "_" + suffix + std::to_string(child_pos);
	rule_new->set_info(rule_cost, new_class, new_label);

	// rule_tree will take the place where dummy node locates
	DepRuleNode* rule_tree = new DepRuleNode(NodeType::NdTree, "", "", -1, parent);
	rule_tree->from_normalized = true;
	if(dummy_node->get_head_flag()) rule_tree->set_head_flag();
	rule_tree->set_optional(dummy_node->is_optional());
	rule_tree->set_node_constraint(NodeConstraint::RuleClass, new_class);
	rule_new->set_node_constraint(NodeConstraint::MatcherLabelCons, parent->get_rule_label());

	if (dummy_node->get_node_type() == NodeType::NdOrNode){
		// Or node may be matched by multiple rules, 
		// don't constraint rule label
		// rule_tree->set_node_constraint(NodeConstraint::RuleLabel, "");
	} else {
		// dummy node is just a short-syntax, 
		// it must matched exactly by the dummy node
		rule_tree->set_node_constraint(NodeConstraint::RuleLabel, new_label);
	}


	rules_vec.push_back(rule_new);
	parent->set_children(child_pos, rule_tree);
	rule_new->from_normalized = true;
	return rule_new;
}

int DepRuleEngine::expand_optionals(DepRuleNode* _rule)
{
	//! for a single rule in rule_vec

	std::stack<DepRuleNode*> rule_stack;
	rule_stack.push(_rule);
	int counter = 0;	
	while (rule_stack.size() > 0){
		DepRuleNode* rule = rule_stack.top();
		rule_stack.pop();

		for (int i = 0; i < rule->get_children_count(); ++i){
			DepRuleNode* rule_x = rule->get_children(i);
			if (rule_x->is_optional()){
				// copy another rule that does not contain the optional node
				DepRuleNode* copied_rule = new DepRuleNode(*rule);

				string suffix = "_O" + std::to_string(++counter);
				rule_x->set_optional(false);
				copied_rule->set_info(rule->get_priority(),
					rule->get_rule_class(), rule->get_rule_label() + suffix);
				copied_rule->remove_child(i);
				copied_rule->from_normalized = true;
				rules_vec.push_back(copied_rule);

				// push the copied_rule, and rule in stack again, 
				// in case there is other optional node								
				rule_stack.push(copied_rule);
				rule_stack.push(rule);
				break;
			} else {
				rule_stack.push(rule_x);
			}			
		}
	}

	return 0;
}

int DepRuleEngine::expand_match_options(DepRuleNode* _rule)
{
	std::stack<DepRuleNode*> rule_stack;
	for (int i = _rule->get_children_count(); i >= 0; --i) {
		rule_stack.push(_rule);
	}
	
	int counter = 0;

	while (rule_stack.size() > 0){
		DepRuleNode* rule = rule_stack.top();
		rule_stack.pop();

		if (rule->get_match_type() == MatchType::MatOr){
			// --------------------
			// expand the children of OR-node into a rule
			// --------------------

			for (int i = 0; i < rule->get_children_count(); ++i){
				DepRuleNode* rule_x = rule->get_children(i);				

				DepRuleNode* wrap_rule = new DepRuleNode(
					NodeType::NdRule, "", "", -1);
				string new_class = rule->get_rule_class();
				string new_label = rule->get_rule_class() + "_" + std::to_string(i);

				wrap_rule->set_info(0, new_class, new_label);
				wrap_rule->set_category(rule->get_category());
				if (rule->get_parent()){					
					wrap_rule->set_node_constraint(NodeConstraint::MatcherClassCons, new_class);
				}

				DepRuleNodeList child_list;
				child_list.push_back(rule_x);
				wrap_rule->set_children(&child_list);
				wrap_rule->get_children(0)->set_head_flag();
				rule->set_children(i, nullptr);
				wrap_rule->from_normalized = true;

				rules_vec.push_back(wrap_rule);
				rule_stack.push(wrap_rule);
			}

			//! remove &rule in rules_vec_ptr and release memory of &rule
			RuleList::iterator iter = rules_vec.begin();
			while (iter != rules_vec.end()){
				if (*iter == rule){ delete rule; rules_vec.erase(iter); break; }
				++iter;
			}
			
		}


	}

	return 0;
}

int DepRuleEngine::expand_anchor(DepRuleNode* _rule)
{
	if (_rule->get_children_count() == 0) return -1;

	for (int cur = 0; cur < _rule->get_children_count();) {
		const DepRuleNode* child_x = _rule->get_children(cur);
		if (child_x->get_node_type() == NodeType::NdSeqStart) {
			DepRuleNode* next_child = _rule->get_children(cur + 1);
			if (!next_child) { ++cur; continue; }

			next_child->set_anchor(AnchorType::AnchorStart);
			next_child->set_start_pos_hint(0);
			next_child->set_node_connection((int)child_x->get_node_connection());
			_rule->remove_child(cur);
			if (cur == 0) {
				//! the ndSeqStart node is the leading child of _rule
				//! that is, _rule must have a start anchor
				_rule->set_anchor(AnchorType::AnchorStart);
				_rule->set_start_pos_hint(0);
			}
			cur = cur;
		} else if (child_x->get_node_type() == NodeType::NdSeqEnd) {
			DepRuleNode* prev_child = _rule->get_children(cur - 1);
			prev_child->set_anchor(AnchorType::AnchorEnd);
			if (!prev_child) { ++cur; continue; }
			if (cur == _rule->get_children_count() - 1) {
				//! the ndSeqEnd node is the last child of _rule
				//! that is, _rule must have a ending anchor
				_rule->set_anchor(AnchorType::AnchorEnd);				
			}
			_rule->remove_child(cur);
			cur = cur;
		} else if (child_x->get_node_type() == NodeType::NdSeqMiddle) {
			DepRuleNode* next_child = _rule->get_children(cur + 1);
			DepRuleNode* prev_child = _rule->get_children(cur - 1);
			
			int pos_hint = child_x->get_start_pos_hint();			
			NodeConnRelation conn_rel = child_x->get_node_connection();
			if (next_child != nullptr) {	
				if (next_child->get_node_connection() == NodeConnRelation::Immediate) {
					next_child->set_start_pos_hint(pos_hint);
				}
				
				if (conn_rel != NodeConnRelation::Immediate){								
					//! child_x only receive conn_rel when it follows the arrow.
					//! There is no need to transfer the relationship when conn_rel of child_x is other 
					//! than Immediate					
					next_child->set_node_connection((int)child_x->get_node_connection());
				}
			} else {
				//! ndSeqMiddle node is the end of the rule,
				//! therefore, _rule share the end_pos_hint with ndSeqMiddle node
				_rule->set_end_pos_hint(pos_hint);
			}

			if (prev_child != nullptr) {				
				if (conn_rel == NodeConnRelation::Immediate) {
					prev_child->set_end_pos_hint(pos_hint);
				}
			} else {
				//! ndSeqMiddle node is the start of the rule,
				//! therefore, _rule share the start_pos_hint with ndSeqMiddle node
				_rule->set_start_pos_hint(pos_hint);
			}

			_rule->remove_child(cur);
			cur = cur;
		} else {
			++cur;
		}

	}

	
	return 0;
}

int DepRuleEngine::assign_rule_head(DepRuleNode* rule_x)
{
	for (int i = 0; i < rule_x->get_children_count(); ++i){
		DepRuleNode* child_x = rule_x->get_children(i);
		if (child_x->get_head_flag()){
			rule_x->set_word(child_x->get_word());
			rule_x->set_pos(child_x->get_pos());
			break;
		}
	}
	return 0;
}

int DepRuleEngine::assign_category(DepRuleNode* rule)
{
	for (int i = 0; i < rule->get_children_count(); ++i) {
		DepRuleNode* rule_ch = rule->get_children(i);

		//! 1. Check node conneciton relation, if it is relation other than immediate, 
		//!    it is a InterSeq rule
		if (rule_ch->get_node_connection() != NodeConnRelation::Immediate) {
			rule->set_category(RuleCategory::InterSeq);
			return 1;
		}				
	}

	//! 2. Check the rule class, if it is LexicalMarker, or any other
	//!    rule class specified to be lexical marker, then it is a lexical marker rule	
	if (std::find_if(lm_categories.cbegin(), lm_categories.cend(), [rule](auto x)->bool {
			return x == rule->get_rule_class() || x == rule->get_rule_label();
		}) != lm_categories.cend()) {
		rule->set_category(RuleCategory::LexicalMarker);
	}

	return 0;
}

int DepRuleEngine::assign_default_info()
{
	string rclass, rlabel;
	int rpriority = 0;

	for (int i = 0; i < rules_vec.size(); ++i) {
		DepRuleNode* rule = (rules_vec)[i];		
		if (rule->get_node_type() != NodeType::NdRule)
			continue;
		
		if (rule->get_rule_class().size() == 0){
			rclass = "Default";
		} else {
			rclass = rule->get_rule_class();
		}

		if (rule->get_rule_label().size() == 0){
			char buf[16];
			sprintf_s(buf, "R%03d", i);
			rlabel = buf;
		} else {
			rlabel = rule->get_rule_label();
		}
		
		rpriority = rule->get_priority();
		

		rule->set_info(rpriority, rclass, rlabel);	

		assign_category(rule);
	}

	return 0;
}

const DepRuleNode* DepRuleEngine::create_seq_delim_rule(int n_slot, const string& seq_rule_label, bool has_delim)
{
	DepRuleNode* rule = new DepRuleNode(NodeType::NdSeqRule, "", "", -1);
	rule->set_info(1, CAT_SEQDELIM, LAB_PREFIX_SEQDELIM + seq_rule_label);
	RuleList ch_rules;

	DepRuleNode* tree_1 = new DepRuleNode(NodeType::NdTree);
	tree_1->set_head_flag();
	tree_1->set_node_constraint(NodeConstraint::RuleLabel, seq_rule_label);
	ch_rules.push_back(tree_1);

	//! if has_delim is true, subtract 1 slot for the leading rule, and another for CATEGORY
	if (has_delim) n_slot -= 2;
	else n_slot -= 1;

	for (int i = 0; i < n_slot; ++i) {
		DepRuleNode* ch_rule = new DepRuleNode(NodeType::NdTree);
		ch_rules.push_back(ch_rule);
	}

	if (has_delim) {
		DepRuleNode* tree_2 = new DepRuleNode(NodeType::NdTree, "", "CATEGORY", -1);
		ch_rules.push_back(tree_2);
	}		
	
	rule->set_children(&ch_rules);
	rules_vec.push_back(rule);

	return rule;
}

const DepRuleNode* DepRuleEngine::create_seq_quot_end_rule(int n_slot, const string& rlabel)
{
	DepRuleNode* rule = new DepRuleNode(NodeType::NdQuotedSeqRule, "", "", -1);
	rule->set_info(1, CAT_SEQDELIM, LAB_PREFIX_QUOTEND + rlabel);
	RuleList ch_rules;

	DepRuleNode* tree_1 = new DepRuleNode(NodeType::NdTree);
	tree_1->set_head_flag();
	tree_1->set_node_constraint(NodeConstraint::RuleLabel, rlabel);
	ch_rules.push_back(tree_1);
	
	for (int i = 0; i < n_slot - 2; ++i) {
		DepRuleNode* ch_rule = new DepRuleNode(NodeType::NdTree);
		ch_rules.push_back(ch_rule);
	}

	DepRuleNode* tree_2 = new DepRuleNode(NodeType::NdTree, "", "CATEGORY", -1);
	ch_rules.push_back(tree_2);	

	rule->set_children(&ch_rules);
	rules_vec.push_back(rule);

	return rule;
}

const DepRuleNode* DepRuleEngine::create_container_rule(const vector<const MatchedDepRule*>& mrule_vec)
{
	DepRuleNode* container = new DepRuleNode(NodeType::NdRule, "", "", -1);
	container->set_info(1, "Generated", "Container");
	DepRuleNodeList ch_rule_list;
	for (auto mrule : mrule_vec) {
		DepRuleNode* ch_rule = new DepRuleNode(NodeType::NdTree);
		ch_rule->set_node_constraint(NodeConstraint::RuleLabel, mrule->matched_rule_label());
		ch_rule_list.push_back(ch_rule);
	}

	container->set_children(&ch_rule_list);
	rules_vec.push_back(container);

	return container;
}

const DepRuleNode* DepRuleEngine::create_container_rule(int n_slot, const string& rclass, const string& rlabel)
{
	DepRuleNode* container = new DepRuleNode(NodeType::NdRule, "", "", -1);
		
	container->set_info(100, 
		rclass.length() > 0? rclass: "Generated", 
		rlabel.length() > 0 ? rlabel : "Container");
	DepRuleNodeList ch_rule_list;
	for (int i = 0; i < n_slot; ++i) {
		DepRuleNode* ch_rule = new DepRuleNode(NodeType::NdTree);		
		ch_rule_list.push_back(ch_rule);
	}

	container->set_children(&ch_rule_list);
	rules_vec.push_back(container);

	return container;
}

const DepRuleNode* DepRuleEngine::find_rule(const string& rule_label) const
{
	for (auto x : rules_vec) {
		if (x->get_rule_label() == rule_label){
			return x;
		}
	}

	return nullptr;
}

const DepRuleNode* DepRuleEngine::find_rule_dynamic(const string& rule_class, const string& rule_label, int n_slot)
{
	const DepRuleNode* dn = find_rule(rule_label);
	if (dn != nullptr) return dn;

	if (rule_label.find(LAB_PREFIX_QUOTEND)!= string::npos) {
		string rlabel(rule_label);
		rlabel.replace(rlabel.find(LAB_PREFIX_QUOTEND), LAB_PREFIX_QUOTEND.length(), "");
		dn = create_seq_quot_end_rule(n_slot, rlabel);
	} else if (rule_label.find(LAB_PREFIX_SEQDELIM) != string::npos) {
		string rlabel(rule_label);
		rlabel.replace(rlabel.find(LAB_PREFIX_SEQDELIM), LAB_PREFIX_SEQDELIM.length(), "");
		dn = create_seq_delim_rule(n_slot, rlabel, n_slot > 1);
	} else {
		dn = create_container_rule(n_slot, rule_class, rule_label);
	}	

	return dn;
}

PC_RuleList DepRuleEngine::find_rules(vector<const DepRuleNode*> rn_vec) const
{	
	PC_RuleList rlist;
	for (auto rule_x : rules_vec) {		
		if (rn_vec.size() != rule_x->get_children_count()) break;
		
		bool is_match = true;
		for (int i = 0; i < rn_vec.size(); ++i) {
			if (!rule_x->get_children(i)->can_generate(rn_vec.at(i))) {
				is_match = false;
				break;
			}
		}

		if (is_match) rlist.push_back(rule_x);
	}

	return rlist;
}

PC_RuleList DepRuleEngine::find_rules(const DepRuleNode* rn) const
{
	PC_RuleList rlist;
	for (auto rule_x : rules_vec) {		

		bool is_match = true;
		int n_child = rule_x->get_children_count();
		for (int i = 0; i < n_child; ++i) {
			if (rule_x->get_children(i)->loose_equal(*rn)) {
				rlist.push_back(rule_x);
				break;
			}
		}		
	}

	return rlist;
}