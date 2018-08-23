#include "PropNode.h"

int PropNode::id_counter = 0;

PropNode PropNode::create_instance(const MatchedDepRule* mrule, PropNodeType pntype)
{
	PropNode pn(pntype);
	pn.node_type = pntype;
	pn.mrule_ptr = mrule;
	pn.id = ++id_counter;
	pn.dot_pos = mrule->dot_pos();
	pn.word = mrule->matched_word();
	pn.pos = mrule->matched_pos();
	pn.rule_class = mrule->matched_rule_class();
	pn.rule_label = mrule->matched_rule_label();
	pn.start = mrule->start_pos();
	pn.end = mrule->end_pos();
	pn.cost = mrule->get_tree_match_cost();
	pn.head_flag = false;
	return pn;
}

bool PropNode::is_content_node() const
{
	return pos != "T" && pos != "P" && pos != "C"
		&& pos != "DET" && pos != "POST";
}