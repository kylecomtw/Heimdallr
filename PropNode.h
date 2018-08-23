#ifndef PROPNODE_H
#define PROPNODE_H

#include <functional>
#include "MatchedDepRule.h"

enum class PropNodeType {Generic = 0, Sequence = 1, Terminal = 2, TermRule = 3, UniqueTerminal = 4};
class PropNode
{
private:
	static int id_counter;
public:	
	string	word;
	string	pos;
	string	rule_class;
	string	rule_label;	
	int		id;
	int		dot_pos;
	int		start;
	int		end;
	int		cost;
	bool    head_flag;
	const PropNode*			parent;
	PropNodeType			node_type;
	const MatchedDepRule*	mrule_ptr;

	static PropNode create_instance(const MatchedDepRule* mrule, PropNodeType pntype);
	static PropNode reset_id_counter() { id_counter = 0; }

	PropNode(PropNodeType pntype) : node_type(pntype), parent(nullptr){}
	bool is_content_node() const;

	bool operator == (const PropNode& other) const {
		return this->start == other.start && this->end == other.end &&
			this->word == other.word && this->pos == other.pos && this->node_type == other.node_type && 
			this->rule_class == other.rule_class && this->rule_label == other.rule_label;
	}

};

template<> struct std::hash<PropNode>{
	size_t operator()(const PropNode& pn) const{
		return std::hash<string>()(pn.word) ^ std::hash<string>()(pn.pos) ^
			std::hash<int>()(pn.start) ^ std::hash<int>()(pn.end) ^ std::hash<int>()((int)pn.node_type) ^
			std::hash<string>()(pn.rule_class) ^ std::hash<string>()(pn.rule_label);
	}
};

#endif //PROPNODE_H