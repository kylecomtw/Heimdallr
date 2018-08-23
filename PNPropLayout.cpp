#include "PNPropLayout.h"
#include <fstream>
#include <stack>
#include <sstream>

PNPropLayout::PNPropLayout()
{
	s_counter = 0; c_counter = 0;
}



void PNPropLayout::initialize_rule_vec(const PropNodeGraph& graph, vector<const MRule*>& mrule_vec)
{
	std::unordered_set<const MRule*> mrule_buf;
	std::stack<const MRule*> work_stack;
	const MRuleSet& seq_rules = graph.cmatched_rules();
	vector<const MRule*> mrule_vec_tmp;

	for (const auto& mrule_x : seq_rules) {
		//! only 1) InterSeq rules, 2) top level rule
		//! are put into mrule_vec_tmp

		if (mrule_x.get_rule_node().get_category() == RuleCategory::InterSeq) {
			mrule_vec_tmp.push_back(&mrule_x);
		} else {
			NodeType ntype = mrule_x.get_rule_node().get_node_type();
			if (ntype == NodeType::NdSeqRule || ntype == NodeType::NdQuotedSeqRule) {
				mrule_vec_tmp.push_back(&mrule_x);
			}
		}
	}
	
	std::sort(mrule_vec_tmp.begin(), mrule_vec_tmp.end(), [](const MRule* a, const MRule* b)-> bool {
		RuleCategory cat_a = a->get_rule_node().get_category();
		RuleCategory cat_b = b->get_rule_node().get_category();
		if (cat_a != cat_b) return cat_a != RuleCategory::InterSeq;
		if (a->start_pos() != b->start_pos()) return a->start_pos() < b->start_pos();
		return a->end_pos() < b->end_pos();
	});
	
	vector<int> quoted_seq_start;
	for (int i = mrule_vec_tmp.size() - 1; i >= 0; --i) {
		if (mrule_vec_tmp[i]->get_rule_node().get_node_type() == NodeType::NdQuotedSeqRule)
			quoted_seq_start.push_back(mrule_vec_tmp[i]->start_pos());
		work_stack.push(mrule_vec_tmp[i]);
	}

	while (work_stack.size() > 0) {
		const MRule* mrule_x = work_stack.top();
		work_stack.pop();

		//! skip SeqRule that is included in a quoted sequence
		if (mrule_x->matched_node_type() == NodeType::NdSeqRule && 
			std::find(quoted_seq_start.begin(), quoted_seq_start.end(), mrule_x->start_pos()) !=
			quoted_seq_start.end())
			continue;

		NodeType m_nodetype = mrule_x->matched_node_type();
		int rule_cost = mrule_x->get_rule_node().get_priority();
		if (m_nodetype == NodeType::NdTerminal ||
			m_nodetype == NodeType::NdTermRule) {
			continue;
		} else {
			for (int i = mrule_x->get_children_count() - 1; i >= 0; --i) {
				work_stack.push(mrule_x->get_matched_children(i));
			}

			if (mrule_x->get_children_count() > 1) {
				if (mrule_buf.count(mrule_x) == 0) {					
					mrule_buf.insert(mrule_x);
					mrule_vec.push_back(mrule_x);
				}
			}

			
		}
	}

	std::sort(mrule_vec.begin(), mrule_vec.end(), [](const MRule* a, const MRule* b)-> bool {
		RuleCategory cat_a = a->get_rule_node().get_category();
		RuleCategory cat_b = b->get_rule_node().get_category();
		if (cat_a != cat_b) return cat_a != RuleCategory::InterSeq;
		if (a->start_pos() != b->start_pos()) return a->start_pos() < b->start_pos();
		if (a->end_pos() != b->end_pos()) return a->end_pos() > b->end_pos();
		return a->matched_rule_label() < b->matched_rule_label();

	});

}

void PNPropLayout::transform(const PropNodeGraph& graph, PNPropVector& prop_vec)
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

	for (const auto mrule_x : mrule_vec) {

		if (mrule_x->get_rule_node().get_category() == RuleCategory::InterSeq) {
			transform_interseq(mrule_x, prop_vec);
		} else {
			transform_chunk(mrule_x, prop_vec);
		}
	}

	return;

}

void PNPropLayout::index_mrule_code(const vector<const MRule*>& mrule_vec)
{
	for (const auto& x : mrule_vec) {
		get_code(x);
	}
}

void PNPropLayout::transform_interseq(const MRule* mrule_x, PNPropVector& prop_vec)
{
	//! Construction transform is not applied in current codes
	//! S1 (Construction, Lex1(C5), Lex2(C6))
	//! S2 (Connection, Term1, Term2)

	PNProp prop;
	prop.code = get_code(mrule_x);		
	
	const DepRuleNode& rnode = mrule_x->get_rule_node();	

	bool is_arrow = false;
	for (int i = 0; i < rnode.get_children_count(); ++i) {
		NodeConnRelation conn = rnode.get_children(i)->get_node_connection();
		if (conn == NodeConnRelation::RightArrow || conn == NodeConnRelation::LeftArrow) {
			is_arrow |= true;
		}
	}
	
	prop.predicate = rnode.get_rule_class() + "." + rnode.get_rule_label();		
	

	for (const MRule* x : mrule_x->get_matched_children()) {		
		if (is_arrow) {
			prop.args.push_back(get_arg_form(x));
		} else {
			PropArg arg = get_arg_form(x);
			arg.decorator = get_lex_marker(x);
			prop.args.push_back(arg);			
		}
	}

	prop_vec.push_back(prop);

}

void PNPropLayout::transform_chunk(const MRule* mrule_x, PNPropVector& prop_vec)
{
	// C1 (RuleClass, Args...)
	// C2 (Head, Args...)
	
	int skip_arg_i = -1;
	PNProp prop;

	prop.code = get_code(mrule_x);	
	//! there should be a better way to handle proposition rewrite
	if (mrule_x->head_index() < 0 || true) {
		//! rule head is not assigned				
		prop.predicate = mrule_x->matched_rule_class() + "." + mrule_x->matched_rule_label();
	} else {
		//! rule head is assigned, use as predicate
		skip_arg_i = mrule_x->head_index();
		prop.predicate = mrule_x->get_matched_children(mrule_x->head_index())->matched_word();		
	}

	for (int i = 0; i < mrule_x->get_children_count(); ++i) {
		const MRule* mrule_ch = mrule_x->get_matched_children(i);
		if (i == skip_arg_i) continue;
		prop.args.push_back(get_arg_form(mrule_ch));		
	}
	
	prop_vec.push_back(prop);
}

const string& PNPropLayout::get_code(const MRule* mrule_x)
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

PropArg PNPropLayout::get_arg_form(const MRule* mrule_x)
{
	PropArg arg;
	arg.start = mrule_x->start_pos();
	arg.end = mrule_x->end_pos();
	arg.pos = mrule_x->matched_pos();
	if (mrule_x->matched_node_type() == NodeType::NdTermRule) {				
		arg.is_reference = false;
		arg.text = mrule_x->matched_word();				
	} else if (mrule_x->get_children_count() == 1) {
		arg = get_arg_form(mrule_x->get_matched_children(0));
	} else {
		arg.is_reference = true;
		arg.text = get_code(mrule_x);
	}

	return arg;
}

string PNPropLayout::get_lex_marker(const MRule* mrule_x)
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