#include "DepRuleNode.h"
#include <sstream>
#include <unicode/ucnv.h>
#include <stack>

std::array<string, 3> ruleCateg_str = { "Grammar", "Lexical", "Sentence" };
unsigned int DepRuleNode::counter = 0;

AnchorType operator | (const AnchorType& a, const AnchorType& b){ return AnchorType((int)a | (int)b); }
AnchorType operator & (const AnchorType& a, const AnchorType& b){ return AnchorType((int)a & (int)b); }
AnchorType operator ~ (const AnchorType& a){ return AnchorType(~(int)a); }

DepRuleNode::DepRuleNode(NodeType _type, const char* _word, const char* _pos, int _ref, DepRuleNode* _parent)
{	
	use_default(_type);	
	word = _word;
	pos = _pos;
	equal_ref = _ref;
	head_flag = false;	
	conn_relation = NodeConnRelation::Immediate;

	id = counter++;
}

DepRuleNode::DepRuleNode(NodeType _type)
{
	use_default(_type);
	id = counter++;
}

DepRuleNode::DepRuleNode()
{
	use_default(NodeType::NdDummy);
	id = counter++;
}

const DepRuleNode& DepRuleNode::operator =(DepRuleNode other)
{
	// swap children field
	copy_fields(other);
	id = counter++;
	auto tmp = this->children;
	auto tmp_rc = this->rule_cons_list;
	auto tmp_rp = this->prop_list;
	this->children = other.children;
	this->rule_cons_list = other.rule_cons_list;
	other.children = tmp;
	other.rule_cons_list = tmp_rc;
	other.prop_list = tmp_rp;

	return *this;
}

DepRuleNode::DepRuleNode(const DepRuleNode& other)
{
	copy_fields(other);
	id = counter++;
	for (auto x : other.children){
		DepRuleNode* ch = new DepRuleNode(*x);
		ch->set_parent(this);
		this->children.push_back(ch);
	}

	for (auto x : other.rule_cons_list){
		RuleConstraint rc(x);
		this->rule_cons_list.push_back(rc);
	}

	for (auto x : other.prop_list) {
		RuleProperty rp(x);
		this->prop_list.push_back(rp);
	}
}

void DepRuleNode::copy_fields(const DepRuleNode& other)
{
	category = other.category;
	parent = other.parent;
	node_type = other.node_type;	
	word = other.word;
	pos = other.pos;
	equal_ref = other.equal_ref;
	optional = other.optional;
	head_flag = other.head_flag;
	rule_class = other.rule_class;
	rule_label = other.rule_label;
	priority = other.priority;
	conn_relation = other.conn_relation;
	from_normalized = other.from_normalized;
	enabled = other.enabled;
	anchor = other.anchor;
	start_pos_hint = other.start_pos_hint;
	end_pos_hint = other.end_pos_hint;
	cont_map = other.cont_map;
	passive = other.passive;

}

DepRuleNode::~DepRuleNode()
{
	for (auto x : children){
		delete x;
	}
}

DepRuleNode* DepRuleNode::make_terminal_rule(string word, string pos)
{
	DepRuleNode* node = new DepRuleNode(NodeType::NdTerminal, word.c_str(), pos.c_str(), -1);
	node->set_info(0, "Terminal", "");
	node->set_head_flag();
	DepRuleNode* rule = new DepRuleNode(NodeType::NdTermRule, word.c_str(), pos.c_str(), -1);
	rule->set_info(0, "TermRule", "");
	DepRuleNodeList node_list;
	node_list.push_back(node);
	rule->set_children(&node_list);

	return rule;
}

DepRuleNode* DepRuleNode::make_from_dummy(const DepRuleNode* rule)
{
	if (rule->get_node_type() != NodeType::NdDummy && 
		rule->get_node_type() != NodeType::NdOrNode){
		return nullptr;
	}

	DepRuleNode* new_node = new DepRuleNode(*rule);
	new_node->set_node_type(NodeType::NdRule);
	new_node->get_children(0)->set_head_flag();
	if (rule->get_node_type() == NodeType::NdDummy){
		new_node->set_match_type(MatchType::MatAll);
	} else if (rule->get_node_type() == NodeType::NdOrNode){
		new_node->set_match_type(MatchType::MatOr);
	}
	return new_node;
}

void DepRuleNode::use_default(NodeType ntype)
{
	parent = 0;
	node_type = ntype;
	word = "";
	pos = "";
	equal_ref = -1;
	head_flag = false;
	rule_class = "";
	rule_label = "";
	priority = 0;
	start_pos_hint = -1;
	end_pos_hint = -1;
	enabled = true;
	match_type = MatchType::MatAll;	
	from_normalized = false;
	passive = false;
}

bool DepRuleNode::is_start_passive() const
{
	if (children.size() > 0) {
		return children[0]->get_passive();
	} else {
		return get_passive();
	}
}

int DepRuleNode::set_children(const DepRuleNodeList* node_list)
{
	for (int i = 0; i < node_list->size(); ++i){
		DepRuleNode* n = node_list->at(i);
		n->set_parent(this);		
		this->children.push_back(node_list->at(i));
	}

	return 0;
}

int DepRuleNode::set_children(int pos, DepRuleNode* node)
{
	if (pos >= children.size()) return -1;
	this->children[pos] = node;
	if (node != nullptr){
		node->set_parent(this);
	}
	return 0;
}

int DepRuleNode::insert_child(int before, DepRuleNode* node)
{
	
	if (before < children.size()){
		int counter = 0;
		auto iter = children.begin();
		while (counter < before){
			++iter;
			++counter;
		}
		node->set_parent(this);
		this->children.insert(iter, node);
	} else if (before == children.size()){
		node->set_parent(this);
		this->children.push_back(node);
	} else {
		return -1;
	} 

	
	return 0;
}

int DepRuleNode::remove_child(int pos)
{
	// this funciton only remove the child pointer from children
	// it does not delete the child itself.
	// The child may be in use else where after removing,
	// releasing the memory should be took care at the caller site.
	
	if (pos >= children.size()) return -1;	
		
	this->children.erase(children.begin() + pos);

	return 0;
}

void DepRuleNode::set_rule_constraints(RuleConstraintList* rc_list)
{
	//! Rule constraints are overloaded with two purpose:
	//! 1) simply used as a rule constriant
	//! 2) assign values to fields in DepRuleNode
	//! This method differentiates the constraint for the second purpose (which starts with a PERCT valence),
	//! and remove it from rule_cons_list after assigning the values to target field in DepRuleNode.

	//! rc_list might be a null pointer
	if (rc_list == nullptr) {
		return;
	}

	const int RULE_VALENCE_PERCT = 9;
	for (int i = 0; i < rc_list->size(); ++i){
		RuleConstraint* rc = rc_list->at(i);
		if (rc->rel_op == RCRelOp::Assignment && rc->valence != RULE_VALENCE_PERCT){
			printf("[WARNING] assignment in a constraint not starts with \"%\", ignored\n");
			continue;
		}
		if (rc->valence == RULE_VALENCE_PERCT){
			if (rc->rel_op != RCRelOp::Assignment){
				printf("[PARSE ERROR] constraint starts with \"%\" should use assignment operator\n");
				continue;
			}

		} else {
			rule_cons_list.push_back(*rc);
		}
	}
}

void DepRuleNode::set_rule_properties(const vector<RuleProperty*>* prop_vec)
{
	if (prop_vec == nullptr) return;
	
	for (auto rp : *prop_vec) {
		auto iter = std::find_if(prop_list.begin(), prop_list.end(), 
			[rp](const RuleProperty& x) {return x.get_key() == rp->get_key(); });
		if (iter == prop_list.end()) {
			RuleProperty prop(*rp);
			prop_list.push_back(std::move(prop));
		} else {
			RuleProperty& prop_x = *iter;
			prop_x.set_value(prop_x.get_value() + "\n" + rp->get_value());
		}
	}
}

const string* DepRuleNode::get_rule_properties(const string& key) const
{
	for (auto& rp : prop_list) {
		if (rp.get_key() == key) {
			return &rp.get_value();
		}
	}
	return nullptr;
}

void DepRuleNode::set_info(int _priority, const string& _class, const string& _label)
{
	priority = _priority;
	rule_class = _class;
	rule_label = _label;
}

string DepRuleNode::to_node_string() const
{
	std::stringstream ss;
	if (this->get_head_flag()) ss << "<";

	if (this->get_node_type() == NodeType::NdTerminal){
		ss << this->get_word();
		if (this->get_pos().size() > 0){
			ss << "(" << this->get_pos() << ")";
		} else {
			if (this->get_word().size() == 0) ss << "(*)";
		}
	} else if (this->get_node_type() == NodeType::NdRoot){
		ss << "{Root} ";
	} else if (this->get_node_type() == NodeType::NdTree){
		ss << "{";
		if (this->start_pos_hint >= 0) {
			ss << start_pos_hint << "-";
		}
		ss << "Tree";
		if (this->cont_map.count(NodeConstraint::RuleClass) > 0)
			ss << "." << this->cont_map.at(NodeConstraint::RuleClass);
		if (this->cont_map.count(NodeConstraint::RuleLabel) > 0)
			ss << "." << this->cont_map.at(NodeConstraint::RuleLabel);
		if (this->cont_map.count(NodeConstraint::NotRuleClass) > 0)
			ss << ".~" << this->cont_map.at(NodeConstraint::NotRuleClass);
		if (this->cont_map.count(NodeConstraint::NotRuleLabel) > 0)
			ss << ".~" << this->cont_map.at(NodeConstraint::NotRuleLabel);
		if (this->get_pos().size() > 0) ss << ":" << this->get_pos();
		if (this->get_word().size() > 0) ss << ":" << this->get_word();
		if (this->end_pos_hint >= 0) {
			ss << "-" << end_pos_hint;
		}
		ss << "}";

	} else if (this->get_node_type() == NodeType::NdDummy){
		ss << "{Dummy} ";
	} else if (this->get_node_type() == NodeType::NdOrNode){
		ss << "{Or} ";
	}
	if (this->get_head_flag()) ss << ">";
	ss << "";
	return ss.str();
}

string DepRuleNode::to_simple_string() const
{
	std::stringstream ss;
	for (int i = 0; i < this->children.size(); ++i){
		DepRuleNode* cnode = children[i];
		ss << cnode->to_node_string();
	}

	return ss.str();
}

void DepRuleNode::set_node_connection(int conn)
{
	switch (conn){
	case(1) :
		conn_relation = NodeConnRelation::LeftArrow; break;
	case(2) :
		conn_relation = NodeConnRelation::RightArrow; break;
	case(3) :
		conn_relation = NodeConnRelation::UDLink; break;
	case(4) :
		conn_relation = NodeConnRelation::Follow; break;
	case(5):
		conn_relation = NodeConnRelation::Siblings; break;
	default:
		conn_relation = NodeConnRelation::Immediate; break;
	}
}

DepRuleNode::operator string() const
{
	std::stringstream ss;

	if (this->node_type == NodeType::NdRule){
		if (this->from_normalized)	ss << "[normalized] ";
		ss << this->rule_class << "." << this->rule_label << ", ";
		ss << "Priority: " << this->priority << std::endl;
	}

	
	if (this->cont_map.count(NodeConstraint::MatcherClassCons) > 0)
		ss << "    [Parent]." << this->cont_map.at(NodeConstraint::MatcherClassCons) << std::endl;
	if (this->cont_map.count(NodeConstraint::MatcherLabelCons) > 0)
		ss << "    [Parent]@" << this->cont_map.at(NodeConstraint::MatcherLabelCons) << std::endl;

	for (auto x : rule_cons_list){
		ss << "Rule constraints: ";
		ss << (string)x;
		ss << "\n";
	}

	ss << "     ";
	ss << to_simple_string();

	return ss.str();
}

string DepRuleNode::to_detail_str() const {
	std::stringstream ss;

	ss << "== Node 0x" << this << " ==" << std::endl;
	ss << "NodeType: " << (int)this->node_type;
	if (head_flag) { ss << "*";  }
	ss << std::endl;
	if (this->node_type == NodeType::NdRule){
		ss << "Rule Info: [" << this->rule_class << "] " << this->rule_label << ",";
		ss << "Priority: " << this->priority << std::endl;
	}
	
	ss << "Parent: " << this->parent << std::endl;
	ss << "ref: " << this->equal_ref << std::endl;
	
	ss << "Word: " << this->word.c_str() << "(" << this->pos.c_str() << ") ";
	for (int i = 0; i < this->children.size(); ++i){
		ss << "[" << i << "] ";
		ss << (string)(*this->children.at(i));
	}

	ss << std::endl;
	return ss.str();
}

int DepRuleNode::fit_score(const vector<DepToken>& dt_vec) const
{
	return -1;
}

bool DepRuleNode::match_multiple_string(const string& needle, const string& hay) const
{	
	size_t found_pos = -1;
	size_t prev_found = 0;

	// if hay is empty, it is always matched
	if (hay.size() == 0) return true;

	std::stringstream ss(hay);
	string tok;
	while (std::getline(ss, tok, '|')){
		if (tok == needle) return true;
	}	

	return false;
}

bool DepRuleNode::match_word(const string& other) const
{	
	return match_multiple_string(other, word);
}

bool DepRuleNode::match_pos(const string& other) const
{
	return match_multiple_string(other, pos);	
}

bool DepRuleNode::match_rule_class(const string& other) const
{
	bool ret = true;
	if (cont_map.count(NodeConstraint::RuleClass) > 0) {
		ret &= match_multiple_string(other, cont_map.at(NodeConstraint::RuleClass));
	}

	if (cont_map.count(NodeConstraint::NotRuleClass) > 0){
		ret &= !match_multiple_string(other, cont_map.at(NodeConstraint::NotRuleClass));
	}
	
	return ret;
}

bool DepRuleNode::match_rule_label(const string& other) const
{
	bool ret = true;
	if (cont_map.count(NodeConstraint::RuleLabel) > 0) {
		ret &= match_multiple_string(other, cont_map.at(NodeConstraint::RuleLabel));
	}

	if (cont_map.count(NodeConstraint::NotRuleLabel) > 0){
		ret &= !match_multiple_string(other, cont_map.at(NodeConstraint::NotRuleLabel));	
	}

	return ret;
}

bool DepRuleNode::match_node_type(NodeType other) const
{
	bool ret = node_type == other;
	if (ret) return ret;

	if (node_type == NodeType::NdTree){
		ret |= other == NodeType::NdTerminal || 
			   other == NodeType::NdRule ||
			   other == NodeType::NdTermRule;
	} else if (node_type == NodeType::NdRule){
		ret |= other == NodeType::NdTree ||
			   other == NodeType::NdTermRule;
	} else if (node_type == NodeType::NdTerminal){
		ret |= other == NodeType::NdTermRule;
	} else if (node_type == NodeType::NdSeqRule) {
		ret |= other == NodeType::NdSeqTree;
	} else if (node_type == NodeType::NdSeqTree) {
		ret |= other == NodeType::NdSeqRule;
	}
	
	return ret;

}

bool DepRuleNode::match_matchee_constraint(const DepRuleNode* matchee) const 
{
	bool ret = true;
	const ConstraintMapType& cmap = matchee->cont_map;
	if (cmap.count(NodeConstraint::MatcherClassCons) > 0) {
		if (this->cont_map.count(NodeConstraint::RuleClass) > 0) {
			ret &= match_multiple_string(
				this->cont_map.at(NodeConstraint::RuleClass),
				cmap.at(NodeConstraint::MatcherClassCons));
		} else {
			// if there is a MatcherClassCons but no corresponding cont_map[RuleClass],
			// the match failed
			return false;
		}
	}

	if (cmap.count(NodeConstraint::MatcherLabelCons) > 0){
		if (this->cont_map.count(NodeConstraint::RuleLabel) > 0) {
			ret &= match_multiple_string(
				this->cont_map.at(NodeConstraint::RuleLabel),
				cmap.at(NodeConstraint::MatcherLabelCons));
		} else {
			return false;
		}
	}
	
	return ret;
}

bool DepRuleNode::match_anchor(const AnchorType& other) const
{
	//! Matching is proceeded as bit operation.
	//! if the state of this->anchor is 0, then it is always matched.
	//! Or, if the state is 1, then other->anchor of corresponding state must be also 1
	//! This conditional logic is equivalent with (~p | q)
	int match_mask = (int)AnchorType::AllMatched;
	int match_res = (int)(~anchor | other);
	int dbg = (match_mask & match_res) == (int)AnchorType::AllMatched;
	return (match_mask & match_res) == (int)AnchorType::AllMatched;
}

bool DepRuleNode::match_position(int start_pos, int end_pos) const
{
	
	bool ret = true;
	if (start_pos_hint >= 0) {
		ret &= (start_pos == start_pos_hint);
	}

	//! end_pos check will be ignored if pass-in end_pos < 0
	if (end_pos_hint >= 0 && end_pos >= 0) {
		ret &= (end_pos == end_pos_hint);
	}

	return ret;
}

bool DepRuleNode::has_non_terminal_symbol() const
{
	std::stack<const DepRuleNode*> work_stack;
	work_stack.push(this);
	int term_i = 0;
	while (work_stack.size() > 0){
		const DepRuleNode* node = work_stack.top();
		work_stack.pop();
		if (node->children.size() == 0 && node->node_type == NodeType::NdTree){
			// terminal
			return false;
		} else {
			// non-terminal
			for (int i = node->children.size()-1; i >= 0; --i){
				work_stack.push(node->children.at(i));
			}
		}
	}

	return true;
}

DepRuleNode* DepRuleNode::get_children(int pos) const
{
	if (pos < children.size() && pos >= 0){
		return children[pos];
	} else {
		return nullptr;
	}
}

int DepRuleNode::get_children_count() const
{
	return children.size();
}

bool DepRuleNode::loose_equal(const DepRuleNode& other) const
{
	bool ret = pos == other.pos && word == other.word && node_type == other.node_type;
	ret &= children.size() == other.children.size();
	ret &= cont_map == other.cont_map;

	if (!ret) return ret;

	for (int i = 0; i < children.size(); ++i) {
		ret &= children.at(i) == other.children.at(i);
		if (!ret) return ret;
	}

	return ret;
}

bool DepRuleNode::operator ==(const DepRuleNode& other) const
{
	bool ret = pos == other.pos && word == other.word && node_type == other.node_type &&
		head_flag == other.head_flag;
	ret &= children.size() == other.children.size();
	ret &= cont_map == other.cont_map;

	if (!ret) return ret;

	for (int i = 0; i < children.size(); ++i){
		ret &= children.at(i) == other.children.at(i);
		if (!ret) return ret;
	}

	return ret;
}

bool DepRuleNode::operator !=(const DepRuleNode& other) const
{
	return !(*this == other);
}

bool DepRuleNode::can_generate(const DepRuleNode* other) const
{
	// Proposition: 
	// If A can generate B, then B must be able to match A.
	
	//debug
	NodeType aa = this->get_node_type();
	NodeType bb = other->get_node_type();
	//
	bool m01 = other->match_node_type(get_node_type());
	bool m02 = other->match_rule_class(get_rule_class());
	bool m03 = other->match_rule_label(get_rule_label());
	bool m04 = other->match_pos(get_pos());
	bool m05 = other->match_word(get_word());	
	bool m07 = other->match_matchee_constraint(this);
	
	//! Strictly speaking, anchor is a property of mathcing, 
	//! one cannot determine the RuleNode compatibility with anchor
	//! Particularly, StartAnchor can generate another rule that is not constrained with StartAnchor
	//! and NoneAnchor can also accommodate a rule node that has StartAnchor constrain
	//! Therefore, match_anchor is not used here.
	// bool m06 = other->match_anchor(get_anchor());

	if ( m01 && m02 && m03 && m04 && m05 && m07 ) {	
		return true;
	} else {
		return false;
	}
}