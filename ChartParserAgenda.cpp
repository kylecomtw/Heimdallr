#include "ChartParserAgenda.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

ChartParserAgenda::ChartParserAgenda() :
	unattempted_count(0), rightmost_dot_pos(0),
	state_root(nullptr), pop_counter(0)
{
	make_root();
}

ChartParserAgenda::~ChartParserAgenda()
{
	reset();
}

const StateTree* ChartParserAgenda::pop()
{
	StateTree* next_st = work_queue.top();	
	next_st->loop_idx = ++pop_counter;
	work_queue.pop();
	dprintf("Pop state %d: f_cost = %d\n", next_st->id, next_st->f_cost);

	// a valid next_st is found, pop it to parser	
	set_state_attempted(next_st);
		
	next_st->print_tree();
	return next_st;
}

void ChartParserAgenda::insert(const EdgeType&& edge, AgendaInsert_t insert_data)
{	
	auto ret_pair = edge_set.insert(edge);
	bool insert_ok = ret_pair.second;	
	const EdgeType* edge_ptr = &(*ret_pair.first);
	ExpandStrategy _strategy = insert_data.strategy;	
	const EdgeType* ansc_edge = insert_data.ansc_edge;
	StateTree* ansc_state = check_StateTree_ptr(insert_data.src_state);
	//StateTree* ansc_state = check_StateTree_ptr(find_state(ansc_edge));

	if (insert_ok){		
		rightmost_dot_pos = std::max(rightmost_dot_pos, edge_ptr->end_pos());
		StateTree* st = create_new_state(edge_ptr, ansc_state, ansc_edge, _strategy);
		
		if (st){
			work_queue.push(st);
			edge_state_map[edge_ptr] = st;
			st->inqueue = true;			
		} else {
			size_t e = edge_set.erase(*edge_ptr);
			_ASSERT(e > 0);
		}
		
	} else {

		const StateTree* state_ptr = find_state(edge_ptr);
		dprintf("Duplicate edge in agenda (State %d): %s\n", state_ptr->id, ((string)edge).c_str());
	}	
}

int ChartParserAgenda::dot_pos() const
{
	return rightmost_dot_pos;
}

//! atEnd() indicates there is no more children in current state.
//! It either means the parsing is complete so there is nothing left to attempt, 
//! or parser is in a wrong path that backtrack is in order.
bool ChartParserAgenda::atEnd() const {
	return work_queue.empty();
}

//! Create a new StateTree root in the agenda and append it in root_vec.
//! This function left edge_set and previous state_root intact.
void ChartParserAgenda::make_root()
{
	state_root = new StateTree(nullptr, nullptr, nullptr, ExpandStrategy::Init);
	state_root->attempted = true;	
	rightmost_dot_pos = 0;
	unattempted_count = 0;
	
	root_vec.push_back(state_root);
}

//! clean up all the resources and make a new root in agenda.
void ChartParserAgenda::reset()
{	
	while (!work_queue.empty()) work_queue.pop();
	for (auto x : root_vec){ delete x; }		
	root_vec.clear();
	edge_set.clear();
	edge_state_map.clear();
	n_token = 0;
	pop_counter = 0;
	StateTree::counter = 0;
	MatchedDepRule::id_counter = 0;
	

	make_root();
}

StateTree* ChartParserAgenda::create_new_state(const EdgeType* e_ptr, 
		StateTree* parent_state, const EdgeType* ansc_edge, ExpandStrategy strategy)
{
	const StateTree* compl_st = check_StateTree_ptr(find_state(ansc_edge));
	StateTree* st = new StateTree(e_ptr, parent_state, compl_st, strategy);	
	st->compute_cost(n_token);

	if (st->f_cost > MatchedDepRule::sink_coeff){
		dprintf("Drop State %d, %s\n", st->id, ((string)*st->edge_ptr).c_str());
	
		vector<StateTree*>& parent_ch = parent_state->children;
		bool has_removed = false;
		for (int i = parent_ch.size()-1; i >= 0; --i){
			if (parent_ch.at(i) == st){
				parent_ch.erase(parent_ch.begin() + i);
				has_removed = true;
				break;
			}
		}

		_ASSERT(has_removed);
		st = nullptr;
		
	} else {
		dprintf("Enqueue State %d, %s\n", st->id, ((string)*st->edge_ptr).c_str());
		unattempted_count += 1;
	}

	return st;
}

StateTree* ChartParserAgenda::check_StateTree_ptr(const StateTree* st_cptr)
{	
	if (st_cptr == nullptr){
		return state_root;
	} else {
		StateTree* st_ptr = const_cast<StateTree*>(st_cptr);
		return st_ptr;
	}
}

void ChartParserAgenda::set_state_attempted(StateTree* st)
{
	st->attempted = true;
	unattempted_count -= 1;
}

const vector<CPStateTree> ChartParserAgenda::get_state_roots() const
{
	vector<CPStateTree> ret_vec(root_vec.size());
	std::copy(root_vec.begin(), root_vec.end(), ret_vec.begin());
	return ret_vec;
}

vector<const EdgeType*> ChartParserAgenda::get_edge_snapshot(int state_idx) const{
	std::queue<StateTree*> work_queue;
	work_queue.push(state_root);
	while (!work_queue.empty()){
		StateTree* state_x = work_queue.front();
		work_queue.pop();

		std::for_each(state_x->children.begin(), state_x->children.end(),
			[&work_queue](StateTree* x){work_queue.push(x); });

		if (state_x->id == state_idx){
			return state_x->get_edge_snapshot();
		}
	}

	return vector<const EdgeType*>();
}

const EdgeType* ChartParserAgenda::find_edge(int idx) const
{
	for (auto& edge : edge_set){
		if (edge.id() == idx) return &edge;
	}

	_ASSERT(false);
	return nullptr;
}

const StateTree* ChartParserAgenda::find_state(const EdgeType* edge) const
{
	if (edge_state_map.count(edge) > 0){
		return edge_state_map.at(edge);
	} else{		
		return nullptr;
	}
}
