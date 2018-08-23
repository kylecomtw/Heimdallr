#include "ParserInitLexicalMarker.h"
#include <algorithm>

void ParserInitLexicalMarker::initialize_parser()
{		
	seq_end_pos = parser.get_term_edges().back()->end_pos();	

	DepRuleEngine* engine = parser.get_rule_engine();
	const RuleList& rule_vec = engine->get_rules_vec();
	for (DepRuleNode* rule : rule_vec){
		if (!rule->get_enabled()) continue;
		if (rule->get_category() != RuleCategory::LexicalMarker) continue;

		rule->set_enabled(false);

		vector<int> m_idx;
		vector<const MatchedDepRule*> m_vec;
		vector<MatchedDepRule*> mrule_vec = create_LM_mrule(rule, m_idx, m_vec);

		for(auto mrule: mrule_vec){
			//! found a possible range of the edge, add to chart						
			mrule->set_parse_strategy(ParseStrategy::LexicalMarker);
			dprintf("Lexical Marker: %s\n", ((string)*mrule).c_str());
			parser.move_to_chart(mrule);			
		}
	}
	
	//! Finally, 
	//! (1) push all frame leaders into agenda, so they are initialized on their own
	//! (2) remove frame leaders and lexical markers out of chart, so they cannot be matched by 
	//!     normal chart parsing.
	initialize_agenda();
	remove_key_terminals();
}

bool ParserInitLexicalMarker::check_mrule_valid(MatchedDepRule* mrule)
{
	//! First, check the literals can properly anchor the frames
	bool is_valid_frame_rule = true;
	const DepRuleNode* rule = &mrule->get_rule_node();
	bool prev_anchored = true;
	for (int i = 0; i < rule->get_children_count(); ++i) {
		const DepRuleNode* ch_rnode = rule->get_children(i);
		const MatchedDepRule* ch_mrule = mrule->get_matched_children(i);
		bool current_anchored = false;

		//! -- Only 1 child --
		//! if it has only one child and it has AnchorBoth, it is anchored
		if (!current_anchored &&
			rule->get_children_count() == 1 &&
			((int)(ch_rnode->get_anchor() & AnchorType::AnchorBoth) > 0 ||
			 ch_mrule->matched_node_type() == NodeType::NdTerminal)) {
			current_anchored = true;
			is_valid_frame_rule = true;
			break;
		}

		//! -- More than two child -- 
		//! if this is the last child and it has an End Anchor, it is anchored
		if (!current_anchored && 
			i == rule->get_children_count() - 1 &&
			(int)(ch_rnode->get_anchor() & AnchorType::EndMask) > 0) {
			current_anchored = true;
		}		

		//! if current ch_mrule is matched (has a definite start and end), 
		//! this node is anchored
		if (!current_anchored && ch_mrule != nullptr) {
			current_anchored = true;
		}
		
		//! if current ch_rnode start or end pos_hint, it is anchored
		if (!current_anchored && 
			ch_rnode->get_start_pos_hint() >= 0 && 
			ch_rnode->get_end_pos_hint() >= 0) {
			current_anchored = true;
		}

		is_valid_frame_rule = prev_anchored || current_anchored;
		if (!is_valid_frame_rule) break;

		prev_anchored = current_anchored;
	}

	return is_valid_frame_rule;


}

vector<MatchedDepRule*> ParserInitLexicalMarker::create_LM_mrule(
	const DepRuleNode* rule, 
	const vector<int>& m_idx, const vector<const MatchedDepRule*>& m_vec)
{


	//! find end_pos_hint
	int start_pos_hint = rule->get_start_pos_hint();
	int end_pos_hint = rule->get_end_pos_hint();
	if (start_pos_hint == -1) start_pos_hint = 0;
	if (end_pos_hint == -1) end_pos_hint = seq_end_pos;


	vector<IntRulePtrPair> literal_rules = extract_literal_terminals(rule);

	//! Second, find literals in the terminals
	vector<int> matched_pos = find_literals(literal_rules);
	if (std::any_of(matched_pos.begin(), matched_pos.end(), [](int x) {return x < 0; })) {
		//! skip this rule if there is any unfound literal
		return vector<MatchedDepRule*>();
	}
	

	//! Third, create a new MatchedDepRule for the FrameRule
	MatchedDepRule* mrule = new MatchedDepRule(*rule, start_pos_hint, end_pos_hint);
	for (int i = 0; i < rule->get_children_count(); ++i) {
		const DepRuleNode* crule = rule->get_children(i);
		
		for (int j = 0; j < m_idx.size(); ++j) {
			if (m_idx[j] == i) {
				mrule->set_matched_children(i, m_vec[j]);
			}
		}

		if (crule->get_node_type() == NodeType::NdTerminal) {
			int matched_idx = -1;
			for (int j = 0; j < literal_rules.size(); ++j) {
				if (literal_rules[j].first == i) matched_idx = matched_pos[j];
			}

			MatchedDepRule* term_mrule = get_parser_terminals(matched_idx);
			mrule->set_matched_children(i, term_mrule);
		} else {
			int start_pos = crule->get_start_pos_hint();
			if (start_pos >= 0) {
				designate_frame_leader(start_pos);
			}
		}
	}

	if (mrule->is_active()) {
		vector<MatchedDepRule*> mrule_vec;
		if (check_mrule_valid(mrule)) {			
			mrule_vec.push_back(mrule);
			return mrule_vec;
		} else {
			dprintf("Invalid FrameRule %s\n", mrule->get_rule_node().get_rule_label().c_str());
			return mrule_vec;
		}
	} else {
		vector<MatchedDepRule*> mrule_new_vec = backward_match_inactive_edge(mrule);
		delete mrule;
		return mrule_new_vec;
	}

	
}

vector<MatchedDepRule*> ParserInitLexicalMarker::backward_match_inactive_edge(EdgeType* mrule)
{
		
	//! rule_pred is a predicate function that return true
	//! when 1) the rule has at least a child node can be realized by mrule
	//! and 2) the rule itself is a lexical marker
	vector<MatchedDepRule*> mrule_new_vec;

	Rule_Predicate rule_pred = [mrule](const DepRuleNode* x)->bool{
		if (x->get_category() != RuleCategory::LexicalMarker)
			return false;

		for (int i = 0; i < x->get_children_count(); ++i){
			if (mrule->is_realization_of(x->get_children(i))) return true;
		}			
		return false;
	};

	//! find all the rules that are completed by the inactive mrule
	RuleList rule_vec = parser.get_rule_engine()->filter_rules(rule_pred);
	for (const DepRuleNode* rule_x : rule_vec){				
		
		//! iterate through the rule_x, find the matched child rule node
		//! and determine the start and end position
		//! set mrule as the matched children of mrule_parent
		vector<int> m_idx;
		vector<const MatchedDepRule*> m_vec;
		for (int i = 0; i < rule_x->get_children_count(); ++i){
			if (mrule->is_realization_of(rule_x->get_children(i))){					
				m_idx.push_back(i);
				m_vec.push_back(new MatchedDepRule(*mrule));
			}
		}
				
		vector<MatchedDepRule*> mrules_new = create_LM_mrule(rule_x, m_idx, m_vec);
		for (auto mrule : mrules_new) {
			mrule_new_vec.push_back(mrule);
		}
	}
	

	return mrule_new_vec;
	
}

MatchedDepRule* ParserInitLexicalMarker::get_parser_terminals(int matched_idx)
{
	matched_literals.insert(matched_idx);
	int nTerm = parser.get_term_tokens().size();
	return parser.get_term_edges().at(matched_idx);
}

void ParserInitLexicalMarker::designate_frame_leader(int matched_idx)
{
	if (matched_idx >= 0 && matched_idx < seq_end_pos) {
		frame_leaders.insert(matched_idx);
	}
}

vector<IntRulePtrPair> ParserInitLexicalMarker::extract_literal_terminals(const DepRuleNode* rule)
{
	vector<IntRulePtrPair> ret;
	for (int i = 0; i < rule->get_children_count(); ++i){
		DepRuleNode* rule_ch = rule->get_children(i);
		if (rule_ch->get_node_type() == NodeType::NdTerminal &&
			(rule_ch->get_word().size() > 0 || rule_ch->get_pos().size() > 0)){
			ret.push_back(IntRulePtrPair(i, rule_ch));
		}
	}

	return ret;
}

vector<int>	ParserInitLexicalMarker::find_literals(const vector<IntRulePtrPair>& rule_literals)
{
	vector<int> matched_pos;
	const vector<EdgeType*> term_vec = parser.get_term_edges();
	int seq_cursor = 0;

	//! iterate through each literals in current rule
	for (int i = 0; i < rule_literals.size(); ++i){
		DepRuleNode* lit = rule_literals[i].second;		
		bool match_found = false;

		//! iterate through the terminals to find the literals
		//! the loop starts with seq_cursor, see below.
		for (int j = seq_cursor; j < term_vec.size(); ++j){
			EdgeType* term_mrule = term_vec[j];

			if (term_mrule->is_realization_of(lit)){
				//! if literals is found, record the position found, and set seq_cursor to the next index,
				//! so that next literals will match the terminal following the current one
				match_found = true;
				matched_pos.push_back(j);
				seq_cursor = j + 1;
				break;
			}			
		}

		//! if none are found, push a -1 indicating no match.
		if (!match_found) matched_pos.push_back(-1);	
	}

	return matched_pos;
}

void ParserInitLexicalMarker::initialize_agenda()
{
	int nTerm = parser.get_term_edges().size();

	for (auto leader_idx : frame_leaders){
		int match_pos = leader_idx;
		
		if (match_pos < nTerm){
			EdgeType* term_edge_x = new EdgeType(*parser.get_term_edges()[match_pos]);
			parser.move_to_agenda(term_edge_x);
		}
	}
}

void ParserInitLexicalMarker::add_lexical_marker_edge()
{
	vector<int> markers_pos;
	IntSet markers_set(frame_leaders);
	markers_set.insert(0);
	markers_set.insert(seq_end_pos);

	std::transform(
		markers_set.cbegin(), markers_set.cend(), 
		std::back_inserter(markers_pos),
		[](IntSet::value_type x) -> int{return x; });
	
	std::sort(markers_pos.begin(), markers_pos.end());
	
	const vector<int>* start_markers = nullptr;
	const vector<int>* end_markers = nullptr;	
	vector<int> edge_start_marker;
	vector<int> edge_end_marker;
	vector<int> seq_start_marker { -1 };
	vector<int> seq_end_marker { seq_end_pos };
	vector<int> rnode_start_marker;
	vector<int> rnode_end_marker;
	for (EdgeType* edge_x : lexical_marker_edges){
		
		//! if edge_x starts with literal terminal, then this edge has a definite start
		const EdgeType* first_child = edge_x->get_matched_children(0);
		const DepRuleNode* first_rnode = edge_x->get_rule_node().get_children(0);
		if (first_child != nullptr){			
			//! edge_start_markers record the position of another literal
			//! codes below advance one position to get over the literal.
			//! But in the edges already starting with a literal, the start_pos should not change.
			//! Thus start_pos_x - 1 is to compensate the advancement.
			int start_pos_x = first_child->start_pos();
			edge_start_marker.push_back(start_pos_x - 1);
			start_markers = &edge_start_marker;
		} else if ((int)(first_rnode->get_anchor() & AnchorType::AnchorStart) > 0) {
			start_markers = &seq_start_marker;
		} else if (first_rnode->get_start_pos_hint() >= 0){
			//! see above for the reason to minus 1
			rnode_start_marker.push_back(first_rnode->get_start_pos_hint() - 1);
			start_markers = &rnode_start_marker;
		} else {
			start_markers = &markers_pos;
		}

		//! if edge_x ends with literal terminal, then this edge has a definite end
		const EdgeType* last_child = edge_x->get_matched_children(edge_x->get_children_count() - 1);
		const DepRuleNode* last_rnode = edge_x->get_rule_node().get_children(edge_x->get_children_count() - 1);
		if (last_child != nullptr) {
			edge_end_marker.push_back(last_child->end_pos());
			end_markers = &edge_end_marker;
		} else if ((int)(last_rnode->get_anchor() & AnchorType::AnchorEnd) > 0){
			end_markers = &seq_end_marker;
		} else if (last_rnode->get_end_pos_hint() >= 0) {
			//! see above for the reason to minus 1
			rnode_end_marker.push_back(last_rnode->get_end_pos_hint() - 1);
			end_markers = &rnode_end_marker;
		} else {
			end_markers = &markers_pos;
		}

		// iterate through possible start position
		for (int i = 0; i < start_markers->size(); ++i){
			int starti = start_markers->at(i) + 1;
			if (starti > edge_x->start_pos()) continue;
			//! iterate through possible end position
			for (int j = 0; j < end_markers->size(); ++j){
				int endi = end_markers->at(j);
				if (endi < starti) continue;
				if (endi < edge_x->end_pos()) continue;

				//! found a possible range of the edge, add to chart
				EdgeType* edge_add = new EdgeType(*edge_x);
				edge_add->set_input_cursor(starti, endi);
				edge_add->set_parse_strategy(ParseStrategy::LexicalMarker);
				dprintf("Lexical Marker: %s\n", ((string)*edge_add).c_str());
				parser.move_to_chart(edge_add);
				
			}
		}

		edge_start_marker.clear();
		edge_end_marker.clear();
	}
	
}

void ParserInitLexicalMarker::remove_key_terminals()
{
	//! remove key terminals (i.e. lexical markers and frame leaders).
	//! Lexical markers are already matched in Lexical Marker Edges, 
	//! and frame leaders are independently initialized into agenda in initialize_agenda().
	//! Upon key terminals removal, they will not be matched in the normal chart parsing process.

	vector<const EdgeType*> to_delete;
	for (auto idx : matched_literals){		
		if (idx >= 0 && idx < parser.get_term_edges().size()){
			to_delete.push_back(parser.get_term_edges()[idx]);
		}		
	}

	for (auto idx : frame_leaders) {
		if (idx >= 0 && idx < parser.get_term_edges().size()) {
			to_delete.push_back(parser.get_term_edges()[idx]);
		}
	}

	for (auto edge_x : to_delete){
		parser.remove_edge_in_chart(edge_x);
	}
}