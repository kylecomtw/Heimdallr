#include "PNTextLayout.h"
#include <fstream>
#include <stack>

PNTextLayout::PNTextLayout()
{
	s_counter = 0; c_counter = 0;
}

void PNTextLayout::initialize_rule_vec(const PropNodeGraph& graph, vector<const MRule*>& mrule_vec)
{	
	std::unordered_set<const MRule*> mrule_buf;
	std::stack<const MRule*> work_stack;
	const MRuleSet& seq_rules = graph.cmatched_rules();
	vector<const MRule*> mrule_vec_tmp;

	for (const auto& mrule_x: seq_rules) {
		//! only 1) InterSeq rules, 2) top level rule
		//! are put into mrule_vec_tmp

		if (mrule_x.get_rule_node().get_category() == RuleCategory::InterSeq) {
			mrule_vec_tmp.push_back(&mrule_x);
		} else {
			if (mrule_x.matched_anchor() == AnchorType::AnchorBoth) {
				mrule_vec_tmp.push_back(&mrule_x);
			}
		}		
	}

	std::sort(mrule_vec_tmp.begin(), mrule_vec_tmp.end(), [](const MRule* a, const MRule* b)-> bool{		
		RuleCategory cat_a = a->get_rule_node().get_category();
		RuleCategory cat_b = b->get_rule_node().get_category();
		if (cat_a != cat_b) return cat_a != RuleCategory::InterSeq;
		if (a->start_pos() != b->start_pos()) return a->start_pos() < b->start_pos();
		return a->end_pos() < b->end_pos();
	});

	for (int i = mrule_vec_tmp.size() - 1; i >= 0; --i) {
		work_stack.push(mrule_vec_tmp[i]);
	}

	while (work_stack.size() > 0) {
		const MRule* mrule_x = work_stack.top();
		work_stack.pop();

		NodeType m_nodetype = mrule_x->matched_node_type();
		if (m_nodetype == NodeType::NdTerminal ||
			m_nodetype == NodeType::NdTermRule ||
			mrule_x->get_rule_node().from_normalized) continue;

		if (mrule_buf.count(mrule_x) > 0) {
			continue;
		} else {
			mrule_buf.insert(mrule_x);
			mrule_vec.push_back(mrule_x);
		}

		for (int i = mrule_x->get_children_count() - 1; i >= 0; --i) {
			work_stack.push(mrule_x->get_matched_children(i));
		}
	}
}

void PNTextLayout::transform(const PropNodeGraph& graph, string fpath)
{
	vector<const MRule*> mrule_vec;
	initialize_rule_vec(graph, mrule_vec);

	index_mrule_code(mrule_vec);
	std::sort(mrule_vec.begin(), mrule_vec.end(), [this](const MRule* a, const MRule* b)->bool {
		const string& code_a = this->get_code(a);
		const string& code_b = this->get_code(b);
		if (code_a.substr(0, 1) == code_b.substr(0, 1)) {
			return stoi(code_a.substr(1)) < stoi(code_b.substr(1));
		} else {
			return code_a < code_b;
		}
	});


	// write out
	std::ofstream ofs(fpath);
	if (!ofs.is_open()) {
		printf("Cannot open %s to write", fpath.c_str());
		return;
	}

	for (const auto mrule_x : mrule_vec) {

		if (mrule_x->get_rule_node().get_category() == RuleCategory::InterSeq) {
			transform_interseq(mrule_x, ofs);
		} else {
			transform_chunk(mrule_x, ofs);
		}		
	}

	ofs.close();
	return;

}

void PNTextLayout::index_mrule_code(const vector<const MRule*>& mrule_vec)
{
	for (const auto& x : mrule_vec) {
		get_code(x);
	}
}

void PNTextLayout::transform_interseq(const MRule* mrule_x, std::ofstream& ofs)
{
	// S1 (Construction, Lex1(C5), Lex2(C6))
	// S2 (Connection, Term1, Term2)
	
	const string& mrule_code = get_code(mrule_x);
	ofs << mrule_code << " (";
	const DepRuleNode& rnode = mrule_x->get_rule_node(); 

	bool is_arrow = false;
	for (int i = 0; i < rnode.get_children_count(); ++i) {
		NodeConnRelation conn = rnode.get_children(i)->get_node_connection();
		if (conn == NodeConnRelation::RightArrow) {
			is_arrow |= true;
		}
	}

	if (is_arrow) {
		ofs << "Connection";		
	} else {
		ofs << "Construction";
	}

	for (const MRule* x : mrule_x->get_matched_children()) {
		ofs << ", ";
		if (is_arrow) {
			ofs << get_arg_form(x);
		} else {
			ofs << get_lex_marker(x) << "(" << get_arg_form(x) << ")";
		}		
	}

	ofs << ")" << std::endl;

}

void PNTextLayout::transform_chunk(const MRule* mrule_x, std::ofstream& ofs)
{
	// C1 (RuleClass, Args...)
	// C2 (Head, Args...)

	const string& mrule_code = get_code(mrule_x);
	ofs << mrule_code << " (";

	int skip_arg_i = -1;

	//! there should be a better way to handle proposition rewrite
	if (mrule_x->head_index() < 0 || true) {
		//! rule head is not assigned		
		ofs << mrule_x->matched_rule_class();
	} else {
		//! rule head is assigned, use as predicate
		skip_arg_i = mrule_x->head_index();
		ofs << mrule_x->get_matched_children(mrule_x->head_index())->matched_word();
	}



	for (int i = 0; i < mrule_x->get_children_count(); ++i) {
		const MRule* mrule_ch = mrule_x->get_matched_children(i);
		if (i == skip_arg_i) continue;
		ofs << ", ";		
		ofs << get_arg_form(mrule_ch);
	}

	ofs << ")" << std::endl;
	
}

const string& PNTextLayout::get_code(const MRule* mrule_x)
{
	if (mrule_code_map.count(mrule_x) == 0) {			
		if (mrule_x->get_rule_node().get_category() == RuleCategory::InterSeq) {
			mrule_code_map[mrule_x] = "S" + std::to_string(++s_counter);
		} else {
			mrule_code_map[mrule_x] = "C" + std::to_string(++c_counter);
		}				
	}

	return mrule_code_map[mrule_x];
}

string PNTextLayout::get_arg_form(const MRule* mrule_x)
{
	if (mrule_x->matched_node_type() == NodeType::NdTermRule) {
		return mrule_x->matched_word();
	} else if (mrule_x->get_rule_node().from_normalized && 
			   mrule_x->get_children_count() == 1) {
		return get_arg_form(mrule_x->get_matched_children(0));
	} else {
		return get_code(mrule_x);
	}
}

string PNTextLayout::get_lex_marker(const MRule* mrule_x)
{
	string ret;
	for (int i = 0; i < mrule_x->get_children_count(); ++i) {
		const MRule* mrule_ch = mrule_x->get_matched_children(i);
		if (mrule_ch->matched_node_type() == NodeType::NdTermRule) {
			if (ret.size() > 0) ret = ret + ".." + mrule_ch->matched_word();
			else ret = mrule_ch->matched_word();
		}
	}
	return ret;
}