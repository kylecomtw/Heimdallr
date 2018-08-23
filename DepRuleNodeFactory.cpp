#include "DepRuleNodeFactory.h"
#include <algorithm>

DepRuleNodeUP DepRuleNodeFactory::generalize(const DepRuleNode* r)
{	
	DepRuleNodeUP rule_uptr(new DepRuleNode(*r));
	bool has_generalized = false;
	if (r->get_node_type() == NodeType::NdRule) {
		//! I. try to find larger matching rules
		// has_generalized = find_embedded_rule(rule_uptr);
		
		// if (has_generalized) return rule_uptr;

		//! II. if step I failed, generalize the node
		for (int i = 0; i < r->get_children_count(); ++i) {			
			const DepRuleNode* r_ch = r->get_children(i);
			DepRuleNodeUP gr_ch = generalize_node(r_ch);
			if (*gr_ch != *r_ch) {												
				rule_uptr->set_children(i, gr_ch.get());
				gr_ch.release();
				has_generalized = true;
				break;
			}
		}

		if (has_generalized) return rule_uptr;

		//! III. if step II failed, merge the node
		//!      {*}{*} -> {Tree:*}
		//!      {Tree:*}{*} -> {Tree:*}				
		for (int i = 0; i < r->get_children_count()-1; ++i) {
			const DepRuleNode* rch_1 = r->get_children(i);
			const DepRuleNode* rch_2 = r->get_children(i + 1);

			if ((is_any_terminal(rch_1) || is_any_tree(rch_1)) &&
				(is_any_terminal(rch_2) || is_any_tree(rch_2))) {
				DepRuleNode* any_tree = new DepRuleNode(NodeType::NdTree, "", "", -1, rule_uptr.get());
				delete rule_uptr->get_children(i + 1);
				rule_uptr->remove_child(i+1);
				delete rule_uptr->get_children(i);
				rule_uptr->set_children(i, any_tree);
				break;
			}
		}
	}

	return rule_uptr;
}

bool DepRuleNodeFactory::find_embedded_rule(DepRuleNodeUP& ruptr)
{
	PC_RuleList ch_vec;
	bool has_embed = false;
	for (int i = 0; i < ruptr->get_children_count(); ++i) {
		ch_vec.push_back(ruptr->get_children(i));
	}

	DepRuleNode* embed_rule = nullptr;
	for (int i = 0; i < ch_vec.size(); ++i) {
		if (embed_rule) break;		
		int start_i = std::min((size_t)i + 2, ch_vec.size() - 1);
		for (int j = start_i; j <= ch_vec.size(); ++j) {
			if (embed_rule) break;

			PC_RuleList sub_vec(ch_vec.begin() + i, ch_vec.begin() + j);
			PC_RuleList embeds = engine->find_rules(sub_vec);	
			PC_RuleList::const_iterator aa = embeds.cend();
			PC_RuleList::const_iterator citer = std::find(embeds.cbegin(), embeds.cend(), ruptr.get());
			if (citer != embeds.cend()) {
				embeds.erase(citer);
			}

			if (embeds.size() > 0) {
				embed_rule = new DepRuleNode(NodeType::NdTree);
				embed_rule->set_node_constraint(NodeConstraint::RuleLabel, embed_rule->get_rule_label());

				for (int k = i; k < j; ++k) {
					delete ruptr->get_children(k);
					if (k == i) ruptr->set_children(k, embed_rule);
					ruptr->remove_child(k);
				}

				has_embed = true;
			} 
		}
	}

	return has_embed;

}

bool DepRuleNodeFactory::is_any_terminal(const DepRuleNode* r)
{
	if (r->get_word().size() == 0 &&
		r->get_pos().size() == 0 &&
		r->get_node_type() == NodeType::NdTermRule) {
		return true;
	}

	return false;
}

bool DepRuleNodeFactory::is_any_tree(const DepRuleNode* r)
{
	if (r->get_word().size() == 0 &&
		r->get_pos().size() == 0 &&
		r->get_node_type() == NodeType::NdTree &&
		r->get_node_constraints().size() == 0) {
		return true;
	}

	return false;
}

DepRuleNodeUP DepRuleNodeFactory::specialize(const DepRuleNode* r)
{
	return DepRuleNodeUP();
}

DepRuleNodeUP DepRuleNodeFactory::generalize_node(const DepRuleNode* r)
{
	DepRuleNodeUP rule_uptr(new DepRuleNode(*r));
	ConstraintMapType cont_map = rule_uptr->get_node_constraints();
	if (rule_uptr->get_word().size() > 0) {
		rule_uptr->set_word("");
	} else if (rule_uptr->get_pos().size() > 0){
		rule_uptr->set_pos("");
	} else if (cont_map.count(NodeConstraint::RuleLabel)){		
		cont_map.erase(NodeConstraint::RuleLabel);
	} else if (cont_map.count(NodeConstraint::NotRuleLabel)) {
		cont_map.erase(NodeConstraint::NotRuleLabel);
	} else if (cont_map.count(NodeConstraint::MatcherLabelCons)) {
		cont_map.erase(NodeConstraint::MatcherLabelCons);
	} else if (cont_map.count(NodeConstraint::RuleClass)) {
		cont_map.erase(NodeConstraint::RuleClass);
	} else if (cont_map.count(NodeConstraint::NotRuleClass)) {
		cont_map.erase(NodeConstraint::NotRuleClass);
	} else if (cont_map.count(NodeConstraint::MatcherClassCons)) {
		cont_map.erase(NodeConstraint::MatcherClassCons);
	}

	rule_uptr->set_node_constraint(cont_map);
	
	return rule_uptr;
}

//! return an archetype of DepRuleNode* or an empty MatchedDepRuleUP if r not normalized.
MatchedDepRuleUP DepRuleNodeFactory::make_archetype(const DepRuleNode* r)
{	
	bool not_normalized = false;
	int nchild = r->get_children_count();
	MatchedDepRuleUP mrule_up(new MatchedDepRule(*r, 0, nchild));
	for (int i = 0; i < nchild; ++i) {
		const DepRuleNode* ch = r->get_children(i);
		//! if DepRuleNode has not been normalized, break the loop and return an empty pointer
		if (ch->get_match_type() == MatchType::MatOr || ch->get_node_type() == NodeType::NdDummy) {
			not_normalized = true;
			break;
		}

		MatchedDepRule* mrule_ch = new MatchedDepRule(*ch, i, i+1);
		mrule_up->advance_dot_with(mrule_ch);
	}

	if (not_normalized) {
		mrule_up.reset();
	}

	return mrule_up;
	
}