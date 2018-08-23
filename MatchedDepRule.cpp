#include "MatchedDepRule.h"
#include "RuleConstraintChecker.h"
#include "StateTree.h"
#include <sstream>

unsigned int MatchedDepRule::id_counter = 0;

MatchedDepRule::MatchedDepRule(const DepRuleNode& _node, int start, int end)
: rule_node(_node)
{	
	_dot = 0;
	_start = start; _end = end;
	m_rule_class = _node.get_rule_class();
	m_rule_label = _node.get_rule_label();	
	m_anchor = _node.get_anchor();
	m_priority = 0;	
	parse_strategy = ParseStrategy::Default;

	if (_node.get_node_type() == NodeType::NdRule) m_node_type = NodeType::NdTree;
	else m_node_type = _node.get_node_type();
		
	for (int i = 0; i < rule_node.get_children_count(); ++i)
	{
		//unmatched_buf.emplace_back();
		matched_children.push_back(nullptr);
	}

	_nTerm = (int)matched_children.size();


}

MatchedDepRule::MatchedDepRule(const MatchedDepRule& other)
: rule_node(other.rule_node)
{
	_id = ++id_counter;
	copy_fields(other);
	for (int i = 0; i < other.matched_children.size(); ++i)
	{		
		const MatchedDepRule* m_node = other.matched_children[i];
		matched_children.push_back(m_node);		
	}
}

MatchedDepRule::MatchedDepRule(MatchedDepRule&& other)
	: rule_node(other.rule_node)
{
	_id = other._id;
	copy_fields(other);
	
	matched_children = std::move(other.matched_children);
}

const MatchedDepRule& MatchedDepRule::operator= (const MatchedDepRule& other)
{
	_id = ++id_counter;
	copy_fields(other);
	matched_children.clear();
	for (int i = 0; i < other.matched_children.size(); ++i)
	{		
		const MatchedDepRule* m_node = other.matched_children[i];		
		matched_children.push_back(m_node);
	}

	return *this;
}

MatchedDepRule& MatchedDepRule::operator= (MatchedDepRule&& other)
{
	if (this != &other){
		_id = other._id;
		copy_fields(other);
		matched_children.clear();
		matched_children = std::move(other.matched_children);

	}

	return *this;
}

void MatchedDepRule::copy_fields(const MatchedDepRule& other)
{	
	_dot = other._dot;
	_start = other._start; 		_end = other._end;
	_nTerm = (int)other.matched_children.size();	
	m_priority = other.m_priority;
	m_word = other.m_word;		m_pos = other.m_pos;
	m_node_type = other.m_node_type;
	m_rule_class = other.m_rule_class;
	m_rule_label = other.m_rule_label;
	m_anchor	 = other.m_anchor;
	parse_strategy = other.parse_strategy;
}

//! matched POS (m_pos) is automatically set when the MatchedDepRule becomes inactive.
//! However, when converting PNProps back to MatchedDepRules (e.g. in PropNodeGraph),
//! m_pos is not set until the terminal rules are created and linked into parent MRules.
//! Therefore, there need to be an update mechanism to set the m_pos manually.
void MatchedDepRule::update_pos()
{
	int head_i = head_index();
	if (head_i < 0) return;

	MatchedDepRule* head_mrule = const_cast<MatchedDepRule*>(matched_children[head_i]);	
	//! I have to violate const-ness here, is there a better way?

	if (head_mrule->matched_pos().length() == 0) {
		head_mrule->update_pos();				
	} else {
		if (head_mrule->matched_pos() != m_pos) {
			m_pos = head_mrule->matched_pos();
			m_word = head_mrule->matched_word();
		}
	}			
}

//! a delayed update for PNProp conversion back
void MatchedDepRule::update_anchor()
{	
	const MatchedDepRule* m_rule0 = matched_children[0];
	const MatchedDepRule* m_rule_end = matched_children.back();
	
	if ((m_rule0->m_anchor & AnchorType::StartMask) == AnchorType::AnchorStart) {
		m_anchor = m_anchor | AnchorType::AnchorStart;
	}
	
	if ((m_rule_end->m_anchor & AnchorType::EndMask) == AnchorType::AnchorEnd) {
		m_anchor = m_anchor | AnchorType::AnchorEnd;
	}
		
}

MatchedDepRule* MatchedDepRule::make_matched_terminal(DepRuleNode* term_rule, int start, int end)
{
	MatchedDepRule* m_rule = new MatchedDepRule(*term_rule, start, end);
	MatchedDepRule* m_term = nullptr;
	if (m_rule->matched_children[0] != nullptr){
		delete m_rule->matched_children[0];
	}	
	m_term = new MatchedDepRule(*term_rule->get_children(0), start, start);
	m_term->m_word = m_term->get_rule_node().get_word();
	m_term->m_pos = m_term->get_rule_node().get_pos();
	m_term->m_node_type = NodeType::NdTerminal;
	m_term->_end = end;
	m_rule->advance_dot_with(m_term);
	return m_rule;
}

MatchedDepRule* MatchedDepRule::make_matched_node(DepRuleNode* rnode)
{
	MatchedDepRule* m_rule = new MatchedDepRule(*rnode, 0, 0);
	if(rnode->get_node_type() != NodeType::NdRule){
		m_rule->m_word = rnode->get_word();
		m_rule->m_pos = rnode->get_pos();
		m_rule->m_node_type = rnode->get_node_type();

		const ConstraintMapType& nc = rnode->get_node_constraints();
		if (nc.count(NodeConstraint::RuleClass) > 0) {
			m_rule->m_rule_class = nc.at(NodeConstraint::RuleClass);
		}

		if (nc.count(NodeConstraint::RuleLabel) > 0) {
			m_rule->m_rule_label = nc.at(NodeConstraint::RuleLabel);
		}

	}

	return m_rule;	
}

bool MatchedDepRule::is_realization_of(const DepRuleNode* other) const
{
	bool ret = true;
		
	// check rule constraints
	ret &= other->match_node_type(m_node_type) &&
		other->match_word(m_word) &&
		other->match_pos(m_pos) &&
		other->match_rule_class(m_rule_class) &&
		other->match_rule_label(m_rule_label) && 
		other->match_anchor(m_anchor) && 
		other->match_position(_start, _end) && 
		other->match_matchee_constraint(&rule_node);
	
	if (!ret){ return false; }

	
	return ret;
}

int MatchedDepRule::replace_matched_children(int pos, const MatchedDepRule* m_rule)
{
	//! simply replace the matched_children, it is usually called in a copy MatchedDepRule scenario
	if (pos >= 0 && pos < matched_children.size()) {
		matched_children[pos] = m_rule;
	}

	return 0;
}

int MatchedDepRule::set_matched_children(int pos, const MatchedDepRule* m_rule)
{
	if (matched_children[pos] == nullptr) {
		matched_children[pos] = m_rule;
		on_match_added(pos, m_rule);
	} else if (matched_children[pos] == m_rule) {
		// do nothing
	} else {
		dprintf("[WARNING] set new node over an already assigned matched child\n");
	}

	//! advance _dot before the last unmatched children.
	//! In lexical marker parsing, a terminal node could be matched
	//! before other children before it are matched.
	//! Therefore, if _dot is before a matched terminal node, 
	//! it must advance after the matched node.
	while (_dot < matched_children.size() && matched_children[_dot] != nullptr){				
		_dot += 1;
		dprintf("[MatchedDepRule] set_matched_children advancing dot: %d\n", _dot);
	}

	if (_dot == _nTerm){
		on_match_completed();
	}
	
	return 0; 	
};

int MatchedDepRule::advance_dot_with(const MatchedDepRule* m_rule)
{
	//! put new m_rule into the child slot
	if (_dot < _nTerm && _nTerm > 0){
		matched_children[_dot] = m_rule;
		// update input position
		_end = m_rule->_end;
		on_match_added(_dot, m_rule);
		_dot += 1;
	} 


	if (_dot == _nTerm) {
		on_match_completed();
		return 1;
	} else {
		return 0;
	}
}

int	MatchedDepRule::get_depth() const
{
	int max_depth = 0;
	if (matched_children.size() == 0){
		return 0;
	}

	for (int i = 0; i < matched_children.size(); ++i){
		if (matched_children.at(i) == nullptr) continue;
		int ch_depth_x = matched_children.at(i)->get_depth() + 1;
		if (max_depth < ch_depth_x) max_depth = ch_depth_x;
	}

	return max_depth;
}

int MatchedDepRule::get_match_penalty() const
{	
	int score = 0;
	for (int i = 0; i < _dot; ++i){
		if (matched_children[i] == nullptr || 
			!matched_children[i]->is_realization_of(rule_node.get_children(i))){
			score += MatchedDepRule::sink_coeff;
		}
	}
	score += RuleConstraintChecker(this).result();
	
	return score;
}

int MatchedDepRule::get_tree_match_cost() const
{		
	int match_cost = (_end - _start) * alpha;
	vector<const MatchedDepRule*> buf;
	buf.push_back(this);
	while (buf.size() > 0){
		const MatchedDepRule* mnode = buf.back();
		buf.pop_back();
		match_cost += mnode->get_match_penalty();
		match_cost += mnode->get_edge_cost();
		for (int i = 0; i < mnode->get_children_count(); ++i){
			const MatchedDepRule* ch_ptr = mnode->get_matched_children(i);
			if (ch_ptr == nullptr) continue;
			buf.push_back(ch_ptr);
		}
	}

	return match_cost;
}

void MatchedDepRule::on_match_completed()
{	
	// determine matched node type
	if (rule_node.get_node_type() == NodeType::NdTermRule) {
		m_node_type = NodeType::NdTermRule;
		m_priority = rule_node.get_priority();
		m_word = matched_children.at(0)->m_word;
		m_pos = matched_children.at(0)->m_pos;
	}else if (rule_node.get_node_type() == NodeType::NdSeqRule){
		m_node_type = NodeType::NdSeqTree;
		m_priority = rule_node.get_priority();
	} else {
		m_node_type = NodeType::NdTree;
		m_priority = rule_node.get_priority();
	}

	// determine matched word, pos
	for (int i = 0; i < matched_children.size(); ++i) {
		const MatchedDepRule* m_child = matched_children[i];
		const DepRuleNode* r_child = rule_node.get_children(i);

		if (r_child->get_head_flag()){
			m_word = m_child->matched_word();
			m_pos = m_child->matched_pos();			
		}
	}	
}

void MatchedDepRule::on_match_added(int dot_pos, const MatchedDepRule* m_rule)
{
	//! update start and end to accommodate m_rule
	if (dot_pos == 0) {
		//! if this is the first child, set m_rule->start_pos() as start
		_start = m_rule->start_pos();
		_end = std::max(_end, m_rule->end_pos());

		//! Update anchor of matched rule from m_rule
		if ((m_rule->m_anchor & AnchorType::StartMask) == AnchorType::AnchorStart) {
			m_anchor = m_anchor | AnchorType::AnchorStart;
		}
	}

	if (dot_pos == matched_children.size() - 1) {
		//! if this is the last child, set m_rule->end_pos() as end
		_start = std::min(_start, m_rule->start_pos());
		_end = m_rule->end_pos();

		if ((m_rule->m_anchor & AnchorType::EndMask) == AnchorType::AnchorEnd) {
			m_anchor = m_anchor | AnchorType::AnchorEnd;
		}
	}
	
	if (dot_pos > 0 && dot_pos < matched_children.size()) {
		//! if this is the child in the middle, make sure the _start and _end include the m_rule
		_start = std::min(_start, m_rule->start_pos());
		_end = std::max(_end, m_rule->end_pos());
	}



}

const DepRuleNode* MatchedDepRule::next_rule_node() const
{
	if (_dot < _nTerm){
		return rule_node.get_children(_dot);
	} else {
		return nullptr;
	}
}

int MatchedDepRule::find_slot(const MatchedDepRule* mrule) const
{
	int ret = -1;
	for (int i = 0; i < _nTerm; ++i){
		//! if slot i has already matched, this slot is not available
		if (matched_children[i] != nullptr) continue;
		const DepRuleNode* rchild = rule_node.get_children(i);
		int match_start = 0;
		int match_end = 0;
		
		//! is slot_i at the end?
		if (i == _nTerm - 1) {
			//! use end_pos as match_end
			match_end = end_pos();			
		} else {					
			if (matched_children[i + 1]) {
				match_end = matched_children[i + 1]->start_pos();
			} else {								
				match_end = rchild->get_end_pos_hint();
			}
		}

		//! is slot_i at the start
		if (i == 0) {
			match_start = start_pos();
		} else {
			if (matched_children[i - 1]) {
				match_start = matched_children[i - 1]->end_pos();
			} else {
				match_start = rchild->get_start_pos_hint();
			}
		}				

		if (mrule->start_pos() == match_start && mrule->end_pos() == match_end){
			ret = i;
			break;
		} 
			
	}

	return ret;
	
}

int MatchedDepRule::head_index() const
{

	for (int i = 0; i < rule_node.get_children_count(); ++i){
		if (rule_node.get_children(i)->get_head_flag()){
			return i;
		}
	}

	return -1;
}

bool MatchedDepRule::operator == (const MatchedDepRule& other) const
{
	if (rule_node != other.rule_node){ 
		return false; 
	} else {
		bool ret = _start == other._start &&
		    	   _end == other._end &&
	    		   _dot == other._dot;
		if (!ret) return false;
		// for additional check, children size should be the same,
		// given the MatchedDepRule's are from same rule.
		if (this->matched_children.size() != other.matched_children.size())
			return false;

		bool word_equal = (this->m_word.size() + other.m_word.size() == 0) ||
			this->m_word == other.m_word;
		bool pos_equal = (this->m_pos.size() + other.m_pos.size() == 0) ||
			this->m_pos == other.m_pos;
		bool rule_class_equal = (this->m_rule_class.size() + other.m_rule_class.size() == 0) ||
			this->m_rule_class == other.m_rule_class;
		bool rule_label_equal = (this->m_rule_label.size() + other.m_rule_label.size() == 0) ||
			this->m_rule_label == other.m_rule_label;
		if (!word_equal || !pos_equal || !rule_class_equal || !rule_label_equal){
			return false;
		}

		
		for (int i = 0; i < matched_children.size(); ++i){
			const MatchedDepRule* this_child = this->get_matched_children(i);
			const MatchedDepRule* other_child = other.get_matched_children(i);
			if (this_child != other_child){
				return false;
			}
		}
		
		return true;
	}
}

bool MatchedDepRule::operator != (const MatchedDepRule& other) const
{
	return !(*this == other);
}

MatchedDepRule::operator string() const
{
	std::stringstream ss;
	ss << "Edge," << get_rule_node().get_rule_label() << ": ";
	for (int i = 0; i < rule_node.get_children_count(); ++i){
		if (i == _dot){ ss << ". "; }		
		const DepRuleNode* rule_x = rule_node.get_children(i);
		if (rule_x->get_node_type() == NodeType::NdTerminal){
			if (rule_x->get_head_flag()) ss << "<";
			if (rule_x->get_word().size() == 0 &&
				rule_x->get_pos().size() == 0){
				ss << "{*}";
			}

			ss << rule_x->get_word();
			if (rule_x->get_pos().size() > 0){
				ss << "(" << rule_x->get_pos() << ")";
			}
			if (rule_x->get_head_flag()) ss << ">";
			ss << " ";
		} else if (rule_x->get_node_type() == NodeType::NdRoot){
			ss << "{Root} ";
		} else if (rule_x->get_node_type() == NodeType::NdTree){			
			if (rule_x->get_head_flag()) ss << "<";
			if (matched_children[i] != nullptr) ss << "[";
			ss << rule_x->to_node_string();
			if (matched_children[i] != nullptr) ss << "]";
			if (rule_x->get_head_flag()) ss << ">";
		} else if (rule_x->get_node_type() == NodeType::NdDummy){
			ss << "{Dummy} ";
		} else if (rule_x->get_node_type() == NodeType::NdOrNode){
			ss << "{Or} ";
		}
	}
	if (_dot == rule_node.get_children_count()){ ss << ". "; }
	ss << "[" << _start << ", " << _end << "]";
	return ss.str();
}