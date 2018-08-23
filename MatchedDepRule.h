#ifndef MATCHEDDEPRULE_H
#define MATCHEDDEPRULE_H

#include <vector>
#include "DepRuleNode.h"
#include <map>

using std::vector;
struct StateTree;
typedef std::map<string, string> MatchConstraintType;
enum class ParseStrategy { Default, LexicalMarker };
enum class ExpandStrategy{ Init, FwdPredict, BckPredict, FwdFundRule, BckFundRule, RepFundRule, UniFundRule };

class MatchedDepRule{
private:	
	unsigned int _id;
	int _dot;
	int _start;
	int _end;
	int _nTerm;	

	const DepRuleNode&    rule_node;	
	vector<const MatchedDepRule*> matched_children;
	
	string m_rule_class;
	string m_rule_label;
	string m_word;
	string m_pos;
	int	   m_priority;
	NodeType		m_node_type;
	AnchorType		m_anchor;
	ParseStrategy	parse_strategy;

	void on_match_completed();
	void on_match_added(int dot_pos, const MatchedDepRule* m_rule);
	void copy_fields(const MatchedDepRule& other);		

public:
	MatchedDepRule(const DepRuleNode& _node, int start, int end);
	~MatchedDepRule(){}
	MatchedDepRule(const MatchedDepRule& other);
	MatchedDepRule(MatchedDepRule&& other);		
	static unsigned int id_counter;
	static const unsigned int alpha = 1000;
	static const unsigned int sink_coeff = 1e7;	

	const MatchedDepRule& operator= (const MatchedDepRule& other);
	MatchedDepRule&		  operator= (MatchedDepRule&& other);
	static MatchedDepRule* make_matched_terminal(DepRuleNode* term_rule, int start, int end);
	static MatchedDepRule* make_matched_node(DepRuleNode* rnode);

	const DepRuleNode* next_rule_node() const;
	const DepRuleNode& get_rule_node() const { return rule_node; }
	
	int				id()		const { return _id; }
	int				rule_id()   const { return rule_node.get_id(); }
	int				start_pos() const { return _start; };
	int				end_pos()   const { return _end; }
	int				dot_pos()	 const { return _dot; }	
	int				find_slot(const MatchedDepRule* mrule) const;
	int				head_index() const;
	bool			is_active() const { return _dot < _nTerm; };		
	const string&	matched_word() const { return m_word; }
	const string&	matched_pos()  const { return m_pos; }
	const string&   matched_rule_class() const { return m_rule_class; }
	const string&	matched_rule_label() const { return m_rule_label; }
	NodeType		matched_node_type() const { return m_node_type; }
	AnchorType		matched_anchor() const { return m_anchor; }
	bool has_non_terminal() const { return rule_node.has_non_terminal_symbol(); }
	void make_inactive() { _dot = _nTerm; on_match_completed(); }
	int  advance_dot_with(const MatchedDepRule* m_rule);
	void update_pos();
	void update_anchor();
	
	bool is_realization_of(const DepRuleNode* other) const;
	int  replace_matched_children(int pos, const MatchedDepRule* m_rule);
	int  set_matched_children(int pos, const MatchedDepRule* m_rule);
	void set_input_cursor(int start, int end){ _start = start; _end = end; };
	void set_parse_strategy(ParseStrategy s) { parse_strategy = s; }
	void set_anchor(AnchorType a) { m_anchor = m_anchor | a; }

	int									 get_children_count() const { return (int)matched_children.size(); }
	const MatchedDepRule*				 get_matched_children(int pos) const { return matched_children.at(pos); };
	const vector<const MatchedDepRule*>& get_matched_children() const { return matched_children; };
	
	ParseStrategy get_parse_strategy() const { return parse_strategy; }

	int	get_depth() const;
	//! penalty is the extra cost impose by constraints
	int get_match_penalty() const;

	//! tree_match_cost equals to terminal_cost + penalty + edge_cost
	int get_tree_match_cost() const;
	
	//! edge_cost is just the priority of the underlying rule node
	int get_edge_cost() const{ return rule_node.get_priority(); }
	// int get_local_match_score() const;	

	//! for use in PropNodeGraph
	void reassign_id(int id){ _id = id; }
	void rebase(int base){ _start += base; _end += base; }

	bool operator == (const MatchedDepRule& other) const;
	bool operator != (const MatchedDepRule& other) const;
	operator string() const;
};


template<>
struct std::hash<MatchedDepRule>{
	size_t operator ()(const MatchedDepRule& x) const
	{
		size_t ret_hash;
		std::hash<string> string_hash = std::hash<string>();
		ret_hash = std::hash<DepRuleNode>()(x.get_rule_node());
		ret_hash ^= std::hash<int>()(x.dot_pos()) << 2;
		ret_hash ^= std::hash<int>()(x.start_pos()) << 3;
		return ret_hash;
	}
};
#endif //MATCHEDDEPRULE_H