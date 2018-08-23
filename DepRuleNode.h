#ifndef DEPRULENODE_H
#define DEPRULENODE_H

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <array>
#include "common.h"
#include "RuleConstraint.h"
#include "RuleProperty.h"

using std::string;
using std::vector;

class DepRuleNode;
// Only Rule (NdRule, NdTermRule) has rule properties (priority, rule_class, and rule_label),
// others (e.g. NdTerminal, NdDummy, NdTree) is just a collection of node, they don't have rule properties
enum class NodeType { NdRule, NdTermRule, NdDummy, NdTerminal, NdTree, NdRoot, NdOrNode, 
					  NdSeqStart, NdSeqEnd, NdSeqMiddle, NdSeqRule, NdSeqTree, NdQuotedSeqRule };
enum class AnchorType {
	AnchorNone = 0, AnchorStart = 1, AnchorEnd = 2, AnchorBoth = 3,
	AllMatched = 3, EndMask = 2, StartMask = 1 };
enum class NodeConstraint { RuleClass, RuleLabel, MatcherClassCons, MatcherLabelCons, NotRuleClass, NotRuleLabel };
enum class NodeConnRelation { Immediate, LeftArrow, RightArrow, UDLink, Follow, Siblings};
enum class MatchType { MatAnd, MatOr, MatAll };
enum class RuleCategory{ Grammar, LexicalMarker, InterSeq };
typedef vector<DepRuleNode*> DepRuleNodeList;
typedef std::map<NodeConstraint, string> ConstraintMapType;
extern std::array<string, 3> ruleCateg_str;

AnchorType operator | (const AnchorType& a, const AnchorType& b);
AnchorType operator & (const AnchorType& a, const AnchorType& b);
AnchorType operator ~ (const AnchorType& a);


class DepRuleNode{

protected:
	RuleCategory category;
	DepRuleNode*		parent;
	const DepRuleNode*	source_rule;
	NodeType node_type;
	MatchType match_type;
	NodeConnRelation conn_relation;
	AnchorType anchor;
	string rule_class;
	string rule_label;
	string word;
	string pos;
	string comment;
	string src_file;
	unsigned int id;
	int priority;
	int equal_ref;	
	int start_pos_hint;
	int end_pos_hint;
	bool enabled;
	bool head_flag;
	bool optional;	
	bool passive;
	int src_line;	

	vector<DepRuleNode*> children;
	vector<RuleConstraint> rule_cons_list;
	vector<RuleProperty> prop_list;
	ConstraintMapType cont_map;
	
	void use_default(NodeType ntype);
	void copy_fields(const DepRuleNode& other);

public:
	DepRuleNode(NodeType _type, const char* _word, const char* _pos, int equal_ref, DepRuleNode* _parent = 0);
	DepRuleNode(NodeType _type);
	DepRuleNode();
	~DepRuleNode();
	DepRuleNode(const DepRuleNode&);
	const DepRuleNode& operator =(DepRuleNode);

	static unsigned int counter;
	static DepRuleNode* make_terminal_rule(string word, string pos);
	static DepRuleNode* make_from_dummy(const DepRuleNode* rule);

	void    add_rule_constraint(const RuleConstraint& rc){ rule_cons_list.push_back(rc); }
	void	set_head_flag() { head_flag = true; }	
	void	set_optional(bool val = true) { optional = val;; }
	int		set_children(const DepRuleNodeList* node_list);
	int		set_children(int pos, DepRuleNode* node);	
	int     insert_child(int after, DepRuleNode* node);
	int		remove_child(int pos);
	void    set_category(RuleCategory c){ category = c; }
	void	set_parent(DepRuleNode* _parent) { parent = _parent; }
	void	set_info(int _priority, const string& _class, const string& _label);
	void    set_node_constraint(NodeConstraint cont, const string& cond) { if (cond.size() > 0) cont_map[cont] = cond; };
	void    set_node_constraint(const ConstraintMapType& _map) { cont_map = _map; }
	void    set_rule_constraints(RuleConstraintList* rc_list);
	void	set_rule_properties(const vector<RuleProperty*>* prop_vec);
	void    set_word(const string& w) { word = w; }
	void    set_pos(const string& p) { pos = p; }
	void    set_node_type(NodeType t) { node_type = t; }
	void    set_match_type(MatchType m) { match_type = m; }	
	void    set_src_line(int n) { src_line = n; }
	void    set_src_file(string fname) { src_file = fname; }
	void    set_comment(const string& str) { comment = str; }
	void    set_anchor(AnchorType a) { anchor = anchor | a; }
	void	set_enabled(bool v) { enabled = v; }
	void    set_node_connection(int conn);	
	void	set_start_pos_hint(int p) { start_pos_hint = p; }
	void	set_end_pos_hint(int p) { end_pos_hint = p; }
	void	set_passive(bool p) { passive = p; }
	void	set_source_rule(const DepRuleNode* r) { source_rule = r; }
	int		fit_score(const vector<DepToken>& dt_vec) const;
	bool	match(const vector<DepToken>& input) const;
	

	bool		has_non_terminal_symbol() const;	
	RuleCategory get_category() const { return category; }
	int			get_priority() const { return priority; }
	NodeType	get_node_type() const { return node_type; };
	MatchType   get_match_type() const { return match_type; }
	NodeConnRelation get_node_connection() const { return conn_relation; }
	AnchorType  get_anchor() const { return anchor; }
	const vector<RuleConstraint>& get_rule_constraints() const { return rule_cons_list; }	
	const ConstraintMapType& get_node_constraints() const { return cont_map; }
	const vector<RuleProperty>& get_rule_properties() const { return prop_list; }
	const string*				get_rule_properties(const string& key) const;
	

	const string&	get_word() const { return word; };
	const string&	get_pos() const { return pos; };
	const string&	get_rule_class() const { return rule_class; }
	const string&	get_rule_label() const { return rule_label; }
	const string&   get_comment() const { return comment; }
	const string&	get_src_file() const { return src_file; }
	bool			is_start_passive() const;
	bool			get_head_flag() const { return head_flag; }	
	bool			is_optional() const { return optional; }
	bool			get_enabled() const { return enabled; }
	bool			get_passive() const { return passive; }	
	int				get_start_pos_hint() const { return start_pos_hint; }
	int				get_end_pos_hint() const { return end_pos_hint; }
	unsigned int	get_id() const { return id; }
	int				get_equal_ref() const { return equal_ref; }
	int				get_src_line() const { return src_line; }	
	
	
	bool		match_word(const string& other) const;
	bool		match_pos(const string& other) const;
	bool		match_node_type(NodeType other) const;
	bool		match_rule_class(const string& other) const;
	bool		match_rule_label(const string& other) const;
	bool		match_anchor(const AnchorType& other) const;
	bool		match_position(int start_pos, int end_pos) const;
	bool		match_matchee_constraint(const DepRuleNode* matchee) const;
	bool		match_multiple_string(const string& needle, const string& hay) const;		
	DepRuleNode*        get_children(int pos) const;
	DepRuleNode*		get_parent() const { return parent; }
	const DepRuleNode*	get_source_rule() const { return source_rule; };
	int                 get_children_count() const;

	bool from_normalized;
	
	string to_node_string() const;
	string to_simple_string() const;
	string to_detail_str() const;	
	bool loose_equal(const DepRuleNode& other) const;
	operator string() const;
	bool operator ==(const DepRuleNode& other) const;
	bool operator !=(const DepRuleNode& other) const;
	bool can_generate(const DepRuleNode* other) const;

};



template<>
struct std::hash<DepRuleNode>{
	size_t operator()(const DepRuleNode& x) const{
		std::hash<string> string_hash = std::hash<string>();
		size_t ret_hash = 0;
		ret_hash = string_hash(x.get_word());
		ret_hash ^= string_hash(x.get_pos());
		ret_hash ^= std::hash<int>()((int)x.get_node_type());

		for (int i = 0; i < x.get_children_count(); ++i){
			ret_hash ^= std::hash<DepRuleNode>()(*x.get_children(i));
		}		
		
		return ret_hash;
	}
};


class DepRuleNodeSet{

public:
	DepRuleNodeSet() :vec(nullptr){}
	~DepRuleNodeSet(){
		//! content in vec are copied to DepRuleEngine after parsing
		//! don't delete the content of vec here
		delete vec;
	}
	DepRuleNodeList* vec;
	DepRuleNodeSet(DepRuleNodeList* ptr) { vec = ptr; };
};

#endif //DEPRULENODE_H