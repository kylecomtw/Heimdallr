#include "ChartParser.h"
#include <algorithm>
#include <string>
#include "dijkstra_algo.h"
#include "DepData.h"
#include "ParserInitFirstTerminal.h"
#include "ParserInitLexicalMarker.h"
#include "SeqDelimiters.h"

ChartParser::ChartParser()
	:to_complete_count(1), full_edge_index(-1), iter_max(5000)
{	
	chart.set_complete_predicate(get_complete_pred());
}

ChartParser::~ChartParser()
{
	reset();	
	// for (auto x : edge_repo){ delete x;	}
}

void ChartParser::reset()
{
	full_edge_index = -1;
	
	for (auto x : aux_rule){ delete x; }
	for (auto x : edge_info){delete x; }
	for (auto x : term_vec) { delete x; }
	for (auto x : edge_buf) { delete x; }
	for (auto x : delim_edges) { delete x; }
	dot_pos = 0;
	edge_info.clear();
	aux_rule.clear();
	dep_vec.clear();
	chart.reset();
	agenda.reset();	
	term_vec.clear();
	edge_buf.clear();
	delim_edges.clear();
}

void ChartParser::move_aux_rule(vector<DepRuleNode*>& aux_repo)
{
	for (auto x : aux_rule){
		aux_repo.push_back(x);
	}
	aux_rule.clear();
	return;
}

void ChartParser::on_init(std::function<void(const ChartParserChart&)> init_func)
{
	init_callback = init_func;
}

int ChartParser::parse(const vector<DepToken>& _dep_vec)
{
	reset();
	
	int n_loop = 0;
	int last_init_dot_pos = 0;
	DepToken seq_marker;
	bool quot_end = false;
	dep_vec = trim_quotation(_dep_vec, quot_end);
	dep_vec = trim_sequence_marker(dep_vec, seq_marker);	
	engine->set_context(_dep_vec);
	engine->switch_mode(false);

	init();
	//! parse loop will continue either when full edge count does not reach to_complete_count,
	//! or agenda has more edges waiting to pop.	
	while (chart.get_full_edges_count() < to_complete_count){	
		dprintf("== Parse loop %d: dot at %d ==\n", ++n_loop, dot_pos);
		if (n_loop > iter_max){
			printf("==== maximum number of loops reached, exiting ====\n");
			break;
		}
		if (!agenda.atEnd()){						

			current_state = agenda.pop();
			const EdgeType* edge = current_state->edge_ptr;
			dprintf("--- Process State %d, Edge %d ---\n", current_state->id, edge->id());					
			dot_pos = agenda.dot_pos();
			add_to_chart(edge);
			process_edge(edge);			

		} else {
			
			if (dot_pos == term_vec.size() ||
				last_init_dot_pos + 1 >= term_vec.size()){
				printf("[INFO] Parser at the end of sentence.\n");
				break;
			}
						
			EdgeType* edge_term = term_vec[(last_init_dot_pos++)];
			dprintf("Initialize with terminal edge: %s\n", ((string)*edge_term).c_str());
			agenda.make_root();
			AgendaInsert_t insert_data;
			insert_data.strategy = ExpandStrategy::Init;
			insert_data.ansc_edge = nullptr;
			insert_data.src_state = nullptr;
			move_to_agenda(edge_term, insert_data);					
			// dot_pos += 1;
		}
		dprintf("== Complete Parse loop %d == \n\n", n_loop);
	}	

	//! if there is no full edge in chart, create a MatchedDepRule based 
	//! on "sequence container rule" that contains the partial edges
	if (chart.get_full_edges_count() == 0) {
		auto mrule_vec = get_partial_completed_edges();
		if (mrule_vec.size() > 0) {
			EdgeType* mrule_container = create_seq_container_mrule(mrule_vec);
			move_to_chart(mrule_container);
		}
	}

	attach_seq_delimited_mrule(seq_marker, quot_end);	

	dprintf("dot position at %d, agenda queue size: %d\n", dot_pos, agenda.n_queue());
	
	return n_loop;
	
}

void ChartParser::build_full_edge_info()
{
	dprintf("chart completed: %d, agenda.atEnd(): %d\n", chart.get_full_edges_count(), agenda.atEnd());
	for (int i = 0; i < chart.get_full_edges_count(); ++i){
		const EdgeType* full_edge_ptr = chart.get_full_edges().at(i);
		dprintf("Full Edge %d (cost: %d): %s\n", i,
			full_edge_ptr->get_tree_match_cost(),
			((string)*full_edge_ptr).c_str());
	}
	printf("unattempted/expanded states in agenda: %d/%d\n", agenda.n_unattempted(), agenda.n_expanded());
	printf("full edges in chart: %d, agenda.atEnd(): %s\n", chart.get_full_edges_count(),
		agenda.atEnd() ? "true" : "false");
	
	int full_edges_count = chart.get_full_edges_count();
	if (full_edges_count == 0){
		build_edge_info(0);
	} else {
		build_edge_info(1);
	}
}

// a convenient overload of parse()
int ChartParser::parse(const vector<DepInfo>& dep_info_vec)
{
	vector<DepToken> dep_vec;
	for (int i = 0; i < dep_info_vec.size(); ++i){
		DepToken dt;
		dt.index = i; dt.word = dep_info_vec.at(i).word;
		dt.pos = dep_info_vec.at(i).pos;
		dep_vec.push_back(std::move(dt));
	}

	int ret = parse(dep_vec);
	dprintf("Agenda edge set size: %d\n", agenda.n_expanded());
	return ret;
}

void ChartParser::init()
{		
	chart.reset(dep_vec.size());
	agenda.set_input_length(dep_vec.size());
	// term_vec.resize(dep_vec.size());
	for (int i = 0; i < dep_vec.size(); ++i){
		// add all terminals into chart
		const DepToken& dt = dep_vec.at(i);
		DepRuleNode* rule_term = DepRuleNode::make_terminal_rule(dt.word, dt.pos);
		
		//! set proper anchor flag
		if (i == 0){
			rule_term->set_anchor(AnchorType::AnchorStart);
		} else if (i == dep_vec.size() - 1){
			rule_term->set_anchor(AnchorType::AnchorEnd);
		}

		//! store terminal rule in a container, so that they can be released
		//! or transfered later
		aux_rule.push_back(rule_term);
		EdgeType* edge_term = EdgeType::make_matched_terminal(rule_term, i, i+1);
		add_to_chart(edge_term);
		term_vec.push_back(edge_term);
		dprintf("TermRule %d: %s\n", i, ((string)*edge_term).c_str());
	}			

	if (init_callback){
		init_callback(chart);
	}

}

vector<DepToken> ChartParser::trim_sequence_marker(const vector<DepToken>& _dep_vec, DepToken& seq_marker)
{
	const string& last_tok = _dep_vec.back().word;
	auto ret = std::find(PUNC_SEQ_DELIM_ARR, PUNC_SEQ_DELIM_ARR + N_PUNC_SEQ_DELIM, last_tok);

	vector<DepToken> ret_vec = _dep_vec;		
	if (ret != PUNC_SEQ_DELIM_ARR + N_PUNC_SEQ_DELIM) {
		seq_marker = _dep_vec.back();
		ret_vec.pop_back();
	}		
	

	return ret_vec;
}

vector<DepToken> ChartParser::trim_quotation(const vector<DepToken>& dp_vec, bool& quot_end)
{
	if (dp_vec.size() <= 1) return dp_vec;

	const string& last_tok = dp_vec.back().word;
	auto ret_end = std::find(PUNC_QUOTATION_ARR, PUNC_QUOTATION_ARR + N_PUNC_QUOTATION, last_tok);

	vector<DepToken> ret_vec = dp_vec;
	if (ret_end != PUNC_QUOTATION_ARR + N_PUNC_QUOTATION) {
		quot_end = true;
		ret_vec.pop_back();
	}

	return ret_vec;
}

void ChartParser::attach_seq_delimited_mrule(const DepToken& seq_marker, bool quot_end)
{	
	DepRuleNode* delim_term = DepRuleNode::make_terminal_rule(seq_marker.word, seq_marker.pos);			
	EdgeType* edge_delim = EdgeType::make_matched_terminal(delim_term, dep_vec.size(), dep_vec.size() + 1);	
	aux_rule.push_back(delim_term);	
	add_to_chart(edge_delim);

	for (const EdgeType* full_edge : chart.get_full_edges()) {
		const DepRuleNode* punc_rule = nullptr;
		if (seq_marker.pos != "CATEGORY" && seq_marker.pos != "PU") {
			punc_rule = engine->create_seq_delim_rule(1, full_edge->matched_rule_label(), false);
		} else {
			punc_rule = engine->create_seq_delim_rule(2, full_edge->matched_rule_label(), true);
		}

		EdgeType* delim_full_edge = new EdgeType(*punc_rule, 0, full_edge->end_pos() + 1);
		delim_full_edge->advance_dot_with(full_edge);
		if (punc_rule->get_children_count() > 1) {
			delim_full_edge->advance_dot_with(edge_delim);
		}

		if (quot_end) {
			DepRuleNode* quot_end_term = DepRuleNode::make_terminal_rule(PUNC_QUOTATION_ARR[1], "CATEGORY");
			EdgeType* edge_qend = EdgeType::make_matched_terminal(quot_end_term, 
									dep_vec.size() + 1, dep_vec.size() + 2);
			aux_rule.push_back(quot_end_term);
			add_to_chart(edge_qend);
			const DepRuleNode* quot_end_rule = engine->create_seq_quot_end_rule(1, delim_full_edge->matched_rule_label());
			EdgeType* quot_full_edge = new EdgeType(*quot_end_rule, 0, delim_full_edge->end_pos() + 1);
			quot_full_edge->advance_dot_with(delim_full_edge);
			quot_full_edge->advance_dot_with(edge_qend);

			delim_edges.push_back(quot_full_edge);
		} else {
			delim_edges.push_back(delim_full_edge);
		}
	}

	std::sort(delim_edges.begin(), delim_edges.end(),
		[](EdgeType* x, EdgeType* y) {return x->get_edge_cost() < y->get_edge_cost(); });
}

EdgeType* ChartParser::create_seq_container_mrule(const vector<const EdgeType*>& mrule_vec)
{	
	const DepRuleNode* container_rule = engine->create_container_rule(mrule_vec);
	MatchedDepRule* mrule_ptr = new MatchedDepRule(*container_rule,
		mrule_vec[0]->start_pos(), mrule_vec.back()->end_pos());
	for (auto mrule : mrule_vec) {
		mrule_ptr->advance_dot_with(mrule);
	}

	return mrule_ptr;
}

void ChartParser::process_edge(const EdgeType* edge)
{	
	dprintf("Process state %d, edge: %s\n", current_state->id, ((string)*edge).c_str());

	if (edge->matched_rule_label() == "Merge01_PR")
		int a = 1;

	if (engine->is_blacklisted(edge)) return;
	if (edge->get_parse_strategy() == ParseStrategy::LexicalMarker) {
		if (edge->is_active()) {
			// edge is not yet completed			
			uni_fundamental(edge);
		} else {
			// edge is completed			
			rep_fundamental(edge);
			bck_fundamental(edge);
		}
		
	} else {
		if (edge->is_active()) {
			fwd_fundamental(edge);
		} else {
			bck_fundamental(edge);		
			rep_fundamental(edge);
		}		
	}

	//! ChartParser actually does not permit forward predict for LexicalMarker edges.
	//! It is enforced in predcit()
	predict(edge);

}

void ChartParser::predict(const EdgeType* edge)
{
	if (engine == nullptr){
		throw ChartParserException("engine is not assigned");
	}
	
	PC_RuleList rules;
	int start_pos = 0, end_pos = 0;
	if (edge->is_active() && edge->get_parse_strategy() != ParseStrategy::LexicalMarker){
		// top-down predict
		// for a edge A -> a . B c [i, j]
		// find rules B -> b, and add to agenda
		// as B -> . b [j, j]
		rules = engine->predict_td(*edge);
		start_pos = edge->end_pos();
		end_pos = start_pos;
	} else {
		// bottom-up predict
		// for an edge B -> b . [i, j]
		// find rules A -> B c, and add to agenda
		// as A -> B . c [i, j]
		start_pos = edge->start_pos();
		end_pos = edge->end_pos();
		rules = engine->predict_bu(*edge);
	}

	int counter = 1;
	for (const auto& x : rules){		
		EdgeType* edge_x = new EdgeType(*x, start_pos, end_pos);
		AgendaInsert_t insert_data;
		insert_data.ansc_edge = edge;
		insert_data.src_state = current_state;		
		if (!edge->is_active()){
			dprintf("[BP ]%d: %s\n", counter, ((string)*edge_x).c_str());
			edge_x->advance_dot_with(edge);
			insert_data.strategy = ExpandStrategy::BckPredict;
			move_to_agenda(edge_x, insert_data);
		} else {			
			dprintf("[FP ]%d: %s\n", counter, ((string)*edge_x).c_str());

			insert_data.strategy = ExpandStrategy::FwdPredict;

			//! if edge_x is forward predicted by edge that is (1) trying to match the first node (dot_pos = 0), 
			//! and (2) edge is anchored with AnchorStart, then assign AnchorStart to edge_x
			if (edge->dot_pos() == 0 &&
				(edge->matched_anchor() & AnchorType::StartMask) == AnchorType::AnchorStart) {
				edge_x->set_anchor(AnchorType::AnchorStart);
			}
			move_to_agenda(edge_x, insert_data);
		}
			
		++counter;
	}

}

void ChartParser::fwd_fundamental(const EdgeType* edge)
{
	// dprintf("-- Forward Fundamentals --\n");
	// given a rule A -> a . B c [i,j]
	// find a rule in chart where B -> b. [j,k],
	// and complete it as A -> a B. c [i, k]	
	const DepRuleNode* to_complete_rule = edge->next_rule_node();	
	int edge_start = edge->start_pos();
	int edge_end = edge->end_pos();
	int n_ffr = 0;
	for (const EdgeType* edge_x : chart){
		// if (edge_x->get_parse_strategy() == ParseStrategy::LexicalMarker) continue;
		if (edge_x->start_pos() != edge_end) continue;
		if (edge_x->is_active()) continue;				
		if (edge_x->is_realization_of(to_complete_rule)){		
			// edge_x contain nodes match to_complete node in edge			
			n_ffr += 1;
			EdgeType* new_edge = new EdgeType(*edge);
			new_edge->set_input_cursor(edge->start_pos(), edge_x->end_pos());
			new_edge->advance_dot_with(edge_x);		
			if (true || new_edge->is_active() || (new_edge->end_pos() - new_edge->start_pos() > 1)) {			
				dprintf("[FFR] %d: %s\n", n_ffr, ((string)*new_edge).c_str());

				AgendaInsert_t insert_data;
				insert_data.ansc_edge = edge_x;
				insert_data.src_state = current_state;
				insert_data.strategy = ExpandStrategy::FwdFundRule;
				move_to_agenda(new_edge, insert_data);
			} else {

				//! prevent a single terminal rule from entering the agenda.
				//! A nominal rule which is only composed of a single terminal would
				//! get in the way of parsing (but how??).
				delete new_edge;
			}
		
		}
	}
}

void ChartParser::bck_fundamental(const EdgeType* edge)
{
	// dprintf("-- Backward Fundamentals --\n");
	// given a rule B -> b . [j,k]
	// find a rule in chart where A -> a . B c [i,j],
	// and completed it as A -> a B . c [i,k]
	const MatchedDepRule* completed = edge;	
	int n_bfr = 0;	

	for (const EdgeType* edge_x : chart){
		if (edge_x->get_parse_strategy() == ParseStrategy::LexicalMarker) continue;
		if (edge_x->end_pos() != edge->start_pos()) continue;				
		if (!edge_x->is_active()) continue;

		const DepRuleNode* to_complete_rule = edge_x->next_rule_node();

		if (!to_complete_rule->match_anchor(completed->matched_anchor())) continue;
		
		if (completed->is_realization_of(to_complete_rule)){
			// edge_x contain nodes match to_complete node in edge			
			// note: don't remove edge_x, it may be completed by other edge(s)
			n_bfr += 1;
			EdgeType* new_edge = new EdgeType(*edge_x);
			new_edge->set_input_cursor(edge_x->start_pos(), edge->end_pos());
			new_edge->advance_dot_with(edge);
			dprintf("[BFR] %d: %s\n", n_bfr, ((string)*new_edge).c_str());

			AgendaInsert_t insert_data;
			insert_data.ansc_edge = edge_x;
			insert_data.src_state = current_state;
			insert_data.strategy = ExpandStrategy::BckFundRule;
			move_to_agenda(new_edge, insert_data);
		}
	}
}

void ChartParser::rep_fundamental(const EdgeType* edge)
{
	// dprintf("-- Replace Fundamentals --\n");
	// given a rule A -> d. [i,j]
	// find a rule in chart where R -> . a B c [ i, j, j+1, k ],	
	// and completed it as R -> A B . c [i, j, j+1, k]

	const MatchedDepRule* completed = edge;
	int n_rfr = 0;
	for (const EdgeType* edge_x : chart){
		//! edge_x must replace to_complete_rule
		if (!edge_x->is_active()) continue;		
		if (edge_x->get_parse_strategy() != ParseStrategy::LexicalMarker) continue;

		int slot_i = edge_x->find_slot(edge);
		if (slot_i < 0) continue;
		
		const DepRuleNode* to_complete_rule = edge_x->get_rule_node().get_children(slot_i);
		if (completed->is_realization_of(to_complete_rule)){
			// edge_x contain nodes match to_complete node in edge			
			// note: don't remove edge_x, it may be completed by other edge(s)
			n_rfr += 1;
			EdgeType* new_edge = new EdgeType(*edge_x);			
			new_edge->set_matched_children(slot_i, edge);
			dprintf("[RFR] %d: %s\n", n_rfr, ((string)*new_edge).c_str());

			AgendaInsert_t insert_data;
			insert_data.ansc_edge = edge_x;
			insert_data.src_state = current_state;
			insert_data.strategy = ExpandStrategy::RepFundRule;
			move_to_agenda(new_edge, insert_data);
		}
	}
}

void ChartParser::uni_fundamental(const EdgeType* edge_x)
{
	// dprintf("-- Union Fundamentals --\n");
	// given a rule X -> A B . c [i, j], where c is an empty slot.
	// find a rule in chart where Y -> a B C [i, j], and a is an empty slot,	
	// and unionly completed it as Z -> A B C. [i, j]		
	int n_ufr = 0;
	for (const EdgeType* edge_y : chart) {
		//! edge_x must be an active edge
		if (!edge_y->is_active()) continue;
		if (edge_y->get_parse_strategy() != ParseStrategy::LexicalMarker) continue;

		//! edge_x must have the same underlying rule node as edge_y, and
		//! they must have span the same sequence
		if (edge_x == edge_y) continue;
		if (edge_x->get_rule_node() != edge_y->get_rule_node()) continue;
		if (edge_x->start_pos() != edge_y->start_pos() ||
			edge_x->end_pos() != edge_y->end_pos()) continue;

		//! try combining edge_x and edge_y
		EdgeType* union_edge = new EdgeType(*edge_x);
		for (int i = 0; i < edge_x->get_children_count(); ++i) {
			const EdgeType* ch_x = edge_x->get_matched_children(i);
			const EdgeType* ch_y = edge_y->get_matched_children(i);
			if (ch_x == ch_y) {
				//! do nothing if they are equal, which might be both null or both the same node
			} else if (ch_x == nullptr && ch_y != nullptr) {
				union_edge->set_matched_children(i, ch_y);
			} else if (ch_x != nullptr && ch_y == nullptr) {
				union_edge->set_matched_children(i, ch_x);
			} else {				
				delete union_edge;
				union_edge = nullptr;
				break;
			}
		}
		
		if (union_edge != nullptr) {
			n_ufr += 1;									
			dprintf("[UFR] %d: %s\n", n_ufr, ((string)*union_edge).c_str());

			AgendaInsert_t insert_data;
			insert_data.ansc_edge = edge_y;
			insert_data.src_state = current_state;
			insert_data.strategy = ExpandStrategy::UniFundRule;
			move_to_agenda(union_edge, insert_data);
		}
	}
}

const EdgeType* ChartParser::add_to_chart(const EdgeType* edge)
{
	dprintf("Add to chart, %s\n", ((string)*edge).c_str());
	auto chart_pair = chart.insert(edge);
	return *chart_pair.first;
}

void ChartParser::move_to_chart(EdgeType* edge)
{
	//! chart does not own any of the pointer it stored.
	//! The object may be belong to agenda or the internal buffer of parser (i.e. term_vec).
	//! For this readon, edge is first move to a new object that owned by Chartparser, 
	//! and add the pointer into chart.
	EdgeType* cpy_edge = new EdgeType(std::move(*edge));
	edge_buf.push_back(cpy_edge);
	edge = nullptr; 
	
	chart.insert(cpy_edge);
	return;	
}

void ChartParser::remove_edge_in_chart(const EdgeType* edge)
{
	chart.erase(edge);
}

void ChartParser::move_to_agenda(EdgeType* edge)
{
	AgendaInsert_t insert_data;
	insert_data.strategy = ExpandStrategy::Init;
	insert_data.src_state = nullptr;
	insert_data.ansc_edge = nullptr;
	agenda.insert(std::move(*edge), insert_data);
	edge = nullptr;
}

void ChartParser::move_to_agenda(EdgeType* edge, AgendaInsert_t insert_data)
{		
	agenda.insert(std::move(*edge), insert_data);
	edge = nullptr;
}

const EdgeType* ChartParser::get_full_edge() const
{
	if (delim_edges.size() > 0) {		
		return delim_edges[0];
	} else if (chart.get_full_edges_count() > 0) {
		return chart.get_full_edges()[0];
	} else {
		return nullptr;
	}
}

vector<const EdgeType*> ChartParser::get_partial_completed_edges() const
{
	vector<const EdgeType*> completed_edges;
	vector<const EdgeType*> selected_edges;
	for (auto edge_x : chart.get_inactive_edges()) {
		if (get_complete_pred()(edge_x)) {
			completed_edges.push_back(edge_x);
		}
	}
	
	find_partial_edges_dijkstra(0, dep_vec.size(), completed_edges, selected_edges);
	return selected_edges;
}

CompleteEdgePred ChartParser::get_complete_pred() const
{
	CompleteEdgePred com_edge_pred = [this](const MatchedDepRule* edge_x) -> bool {
		return this->get_const_rule_engine()->check_precedence_rules(edge_x);
	};
	return com_edge_pred;
}

void ChartParser::build_edge_info(int full_idx)
{
	for (auto x : edge_info){ delete x; }
	edge_info.clear();


	vector<const EdgeType*> selected_edges;
	if (full_idx > 0 && full_idx <= chart.get_full_edges_count()){
		//! if chart has full edges, use them directly		
		const EdgeType* max_edge = chart.get_full_edges().at(full_idx-1);
		int optim_cost = max_edge->get_tree_match_cost();
		selected_edges.push_back(max_edge);
		printf("find edge with max score: %d\n", optim_cost);

	} else {
		//! if chart does not have full edge, use dijkstra algorithm to find a shortest path from
		//! the complete edges in chart.

		EdgeCPtrSetType completed_edges;		
		for (const EdgeType* edge : chart){
			if (edge->is_active()) continue;
			// if (edge.get_children_count() == 1) continue;
			dprintf("Complete Edge %d: %s\n", completed_edges.size() + 1, ((string)*edge).c_str());			
			completed_edges.insert(edge);
		}

		int nToken = dep_vec.size();
		dijkstra_shortest_path(0, nToken, completed_edges, selected_edges);
		printf("Completed/Chart edges: %d/%d\n", completed_edges.size(), chart.size());
		printf("-- Shortest-path edges: %d --\n", selected_edges.size());
	}


	for (auto& edge_x : selected_edges){
		rprintf("%s\n", ((string)*edge_x).c_str());
		edge_info.push_back(EdgeInfoType::from_matched_dep_rule(edge_x));
	}
}

int ChartParser::select_parse_result(int idx)
{	
	build_edge_info(idx);
	return 0;
}

vector<EdgeInfoTypeUP> ChartParser::get_edge_info() const
{
	vector<EdgeInfoTypeUP> ret_edge_info;
	if (edge_info.size() == 0){
		printf("[WARNING] edge_info is empty, try call build_full_edge_info() first\n");
	}

	for (int i = 0; i < edge_info.size(); ++i){
		ret_edge_info.push_back(EdgeInfoTypeUP(edge_info.at(i)->CopyTree()));
	}

	return ret_edge_info;
}

vector<DepInfo> ChartParser::get_dep_info() const
{			
	if (edge_info.size() == 0){
		printf("[WARNING] edge_info is empty, try call build_full_edge_info() first\n");
	}

	int nToken = dep_vec.size();
	DepData dep_data;
	vector<DepInfo> dep_info = dep_data.dep_convert(edge_info, nToken);
	return dep_info;
}

EdgeInfoTypeUP ChartParser::find_edge(int edge_idx) const
{
	const EdgeType* edge_ptr = agenda.find_edge(edge_idx);
	EdgeInfoType* einfo_ptr = EdgeInfoType::from_matched_dep_rule(edge_ptr);
	return EdgeInfoTypeUP(einfo_ptr);
	
}

EdgeInfoTypeUP ChartParser::create_edge_snapshot(int state_idx) const
{
	vector<const EdgeType*> edge_ptr_vec = agenda.get_edge_snapshot(state_idx);
	EdgeInfoType* buf_ptr = nullptr;
	for (int i = 0; i < edge_ptr_vec.size(); ++i){
		const EdgeType* edge_ptr = edge_ptr_vec[i];
		int edge_dot = edge_ptr->dot_pos();
		EdgeInfoType* einfo_ptr = EdgeInfoType::from_matched_dep_rule(edge_ptr);
		if (buf_ptr && edge_dot < einfo_ptr->children.size()){
			einfo_ptr->children[edge_dot] = buf_ptr;
			einfo_ptr->end = buf_ptr->end;
		}
		buf_ptr = einfo_ptr;
	}
	
	return EdgeInfoTypeUP(buf_ptr);

}

