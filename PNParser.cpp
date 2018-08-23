#include "PNParser.h"

void PNParser::reset()
{
	//! PNParser cannot be reset
	//! instantiate a new PNParser and assign new PNAgenda and PNChart to it.
	throw new std::exception();
	
}

void PNParser::init()
{
	//! PNAgenda is initialized with the edges in mrule_set in constructor

	//! construct punc_pos with all the edges(MRules in PNParser) in the chart.
	//! punc_pos records all the punctuations occured in the text.
	//! These information are used to check NodeConnRelation::Follow in FFR/BFR.
	for (const MRule* mrule_x : chart){
		if (mrule_x->get_rule_node().get_node_type() == NodeType::NdSeqRule){
			if (mrule_x->end_pos() <= 0) continue;
			punc_pos.push_back(mrule_x->end_pos());
		}
	}

	std::sort(punc_pos.begin(), punc_pos.end());
	engine.reset_context();
	engine.set_context_to_all();
	//! there is no need to switch mode here, since only the relevance ruels are loaded now.
	//! Moreover, linking stage needs rules more than inter-seq rules
	// engine.switch_mode(true);
}

void PNParser::process_edge(const MRule* edge)
{
	dprintf("Process edge %s\n", ((string)*edge).c_str());
	if (edge->is_active()){
		// edge is not yet completed
		fwd_fundamental(edge);
	} else {
		// edge is completed
		bck_fundamental(edge);
	}

	predict(edge);
}

void PNParser::fwd_fundamental(const MRule* edge)
{
	// given a rule A -> a . B c [i,j]
	// find a rule in chart where B -> b. [j,k],
	// and complete it as A -> a B. c [i, k]	
	const DepRuleNode* to_complete_rule = edge->next_rule_node();
	int n_ffr = 0;
	for (const MRule* edge_x : chart){

		NodeConnRelation conn_rel = to_complete_rule->get_node_connection();

		bool in_valid_pos = check_position_valid(edge, edge_x, conn_rel);
		if (!in_valid_pos) continue;
		
		if (edge_x->is_active()) continue;
		if (edge_x->is_realization_of(to_complete_rule)){
			// edge_x contain nodes match to_complete node in edge			
			n_ffr += 1;
			MRule* new_edge = new MRule(*edge);
			new_edge->set_input_cursor(edge->start_pos(), edge_x->end_pos());
			new_edge->advance_dot_with(edge_x);
			dprintf("[FFR] %d: %s\n", n_ffr, ((string)*new_edge).c_str());

			move_to_agenda(new_edge);
		}
	}
}

void PNParser::bck_fundamental(const MRule* edge)
{
	// given a rule B -> b . [j,k]
	// find a rule in chart where A -> a . B c [i,j],
	// and completed it as A -> a B . c [i,k]
	const MatchedDepRule* completed = edge;
	int n_bfr = 0;
	for (const MRule* edge_x : chart){
		if (!edge_x->is_active()) continue;

		const DepRuleNode* to_complete_rule = edge_x->next_rule_node();

		NodeConnRelation conn_rel = to_complete_rule->get_node_connection();
		bool in_valid_pos = check_position_valid(edge_x, edge, conn_rel);
		if (!in_valid_pos) continue;

		if (completed->is_realization_of(to_complete_rule)){
			// edge_x contain nodes match to_complete node in edge			
			// note: don't remove edge_x, it may be completed by other edge(s)
			n_bfr += 1;
			MRule* new_edge = new MRule(*edge_x);
			new_edge->set_input_cursor(edge_x->start_pos(), edge->end_pos());
			new_edge->advance_dot_with(edge);
			dprintf("[BFR] %d: %s\n", n_bfr, ((string)*new_edge).c_str());

			move_to_agenda(new_edge);
		}
	}
}

bool PNParser::check_position_valid(const MRule* leader, const MRule* follower, NodeConnRelation conn_rel){	
	if (!(leader->end_pos() <= follower->start_pos())) return false;
	int n_punc = find_npunc_between(leader->end_pos(), follower->start_pos());
	if (conn_rel == NodeConnRelation::Immediate){
		if (!(leader->end_pos() == follower->start_pos())) return false;
	} else if (conn_rel == NodeConnRelation::RightArrow){
		//! RightArrow relation dictates leader and follower must be two remote or consecutive sequences		
		return n_punc > 0;
	} else if (conn_rel == NodeConnRelation::Follow){
		//! Follow relation dictates leader and follower must be two consecutive sequences		
		return n_punc == 1;						
	} else if (conn_rel == NodeConnRelation::Siblings) {
		//! Sibling relation dictates leader and follower must be in the same sequence		
		return n_punc == 0;
	}

	return true;
}

void PNParser::predict(const MRule* edge)
{	
	//! For simplicity sake, 
	//! all the nodes are treated as immediate connection in prediction,
	//! although fundamental rules do handle different kinds of connections (see above).
	
	PC_RuleList rules;
	int start_pos = 0, end_pos = 0;
	if (edge->is_active()){
		// top-down predict
		// for a edge A -> a . B c [i, j]
		// find rules B -> b, and add to agenda
		// as B -> . b [j, j]
		rules = engine.predict_td(*edge);
		start_pos = edge->end_pos();
		end_pos = start_pos;
	} else {
		// bottom-up predict
		// for an edge B -> b . [i, j]
		// find rules A -> B c, and add to agenda
		// as A -> B . c [i, j]
		start_pos = edge->start_pos();
		end_pos = edge->end_pos();
		rules = engine.predict_bu(*edge);
	}

	int counter = 1;
	for (const auto& x : rules){
		MRule* edge_x = new MRule(*x, start_pos, end_pos);
		if (!edge->is_active()){			

			edge_x->advance_dot_with(edge);
			dprintf("[BP ]%d: %s\n", counter, ((string)*edge_x).c_str());
			move_to_agenda(edge_x);
		} else {
			dprintf("[FP ]%d: %s\n", counter, ((string)*edge_x).c_str());
			move_to_agenda(edge_x);
		}

		++counter;
	}
}

void PNParser::move_to_agenda(MRule* edge)
{
	agenda.add(edge);
}

void PNParser::add_to_chart(const MRule* edge)
{
	chart.add(edge);
}

int PNParser::parse()
{
	int n_loop = 0;

	dprintf("=== PNParser.parse() ===\n");
	init();
	
	while (!agenda.empty()){
		const MRule* edge  = agenda.pop();	
		if (edge == nullptr) continue;

		dprintf("before process, agenda left: %d\n",
			agenda.size());
		add_to_chart(edge);
		process_edge(edge);
		dprintf("process complete\n");
	}
	dprintf("======\n");
	return 0;

}

int PNParser::find_npunc_between(int start, int end)
{
	int npunc = 0;
	if (start == 12 && end == 12) {
		int a = 1;
	}
	for (int i = 0; i < punc_pos.size(); ++i) {
		if (start <= punc_pos[i] && end >= punc_pos[i]) {
			npunc += 1;
		} else if (end < punc_pos[i]) {
			break;
		}
	}

	return npunc;
}