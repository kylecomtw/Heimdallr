#include "StateGraphNode.h"
#include "RuleAffinityChecker.h"
#include <sstream>
#include <iomanip>



unsigned int StateGraphNode::counter = 0;

StateGraphNode::StateGraphNode(
	const EdgeType* e_ptr, StateGraphNode* _anc, ExpandStrategy _s) :
	id(counter++), loop_idx(0),
	f_cost(0), g_cost(0), h_cost(0),
	affinity_penalty(0),
	attempted(false), inqueue(false),
	edge_ptr(e_ptr), branch_state(nullptr), strategy(_s)
{
	if (_anc != nullptr){
		ancestors.insert(_anc);
	}

	if (strategy == ExpandStrategy::FwdPredict){
		//! FP change edge_ptr, and use parent->edge_ptr as edge_parent
		branch_state = _anc;
	} else if (strategy == ExpandStrategy::BckPredict){
		//! BP change edge_ptr, but does not change edge_parent
		branch_state = _anc->branch_state;
	} else if (strategy == ExpandStrategy::FwdFundRule){
		//! FFR does not change edge_ptr, nor edge_parent
		branch_state = _anc->branch_state;
	} else if (strategy == ExpandStrategy::BckFundRule){
		//! BFR put edge_ptr to edge_parent and edge_parent to edge_ptr's parent
		if (_anc->branch_state){
			branch_state = _anc->branch_state->branch_state;
		}
	}

}

StateGraphNode::~StateGraphNode()
{
	for (auto x : descendents){
		delete x;
	}
	descendents.clear();
}

StateGraphNode::operator string() const
{
	int attempted = 0;
	for (auto x : descendents){ attempted += x->attempted; }
	std::stringstream ss;
	ss << std::setw(3) << loop_idx << ". ";
	ss << "[" << attempted << "/" << descendents.size() << "]";
	ss << "[" << ExpandStrategyStr[(int)strategy] << "]";
	if (edge_ptr != nullptr){
		ss << (string)*edge_ptr;
	}
	ss << "[cost: " << f_cost << "]";

	return ss.str();
}

void StateGraphNode::print_trace() const
{
	std::stringstream ss;
	ss << "----- Tree Trace (leaf->root)-----\n";
	int depth = 0;
	const StateGraphNode* st = this;
	while (st != nullptr){
		ss << "[" << std::setw(2) << std::setfill(' ') << depth << "]";
		ss << (string)*st << "\n";
		if (st->backpointer == nullptr) break;
		st = st->backpointer;
		depth += 1;
		dprintf("%s", ss.str().c_str());
		ss.str(string());
	}
	ss << std::setw(20) << std::setfill('-') << "";
	dprintf("%s\n", ss.str().c_str());
}

void StateGraphNode::compute_cost(int n_token)
{
	//! affinity_penalty is cumulative along the state tree.
	if (backpointer->edge_ptr != nullptr){
		//! matcher would be null in the root state
		affinity_penalty = 0;
	}

	if (backpointer != nullptr){
		affinity_penalty += backpointer->affinity_penalty + check_bckFundRule();
	}

	//! actual cost from initial to current state
	g_cost = get_completed_cost() +
		get_edge_penalty() +
		affinity_penalty;
	//! heuristic cost from current state to goal
	h_cost = get_remaining_cost(n_token);
	//! cost of current state
	f_cost = g_cost + h_cost;
}

vector<const EdgeType*> StateGraphNode::get_edge_snapshot() const
{
	vector<const EdgeType*> ret;
	const StateGraphNode* st = this;
	ret.push_back(st->edge_ptr);

	while (st->branch_state){
		st = st->branch_state;
		ret.push_back(st->edge_ptr);
	}

	return ret;
}

//! any backward fundamental rule in state tree must be coupled with 
//! a corresponding state that has the edge
//! (1) with the same starting position
//! (2) is not complete
//! (3) realize the same rule, or a sink cost is applied
int StateGraphNode::check_bckFundRule() const
{
	//! only check state expand with BFR
	if (this->strategy != ExpandStrategy::BckFundRule) return 0;
	bool check_ok = false;

	if (backpointer->branch_state){
		const EdgeType* depart_edge = backpointer->branch_state->edge_ptr;

		if (depart_edge->start_pos() == this->edge_ptr->start_pos() &&
			depart_edge->is_active()){
			check_ok = true;
		}
	} else {
		check_ok = false;
	}

	return (1 - check_ok) * MatchedDepRule::sink_coeff;
}

int StateGraphNode::get_edge_penalty() const
{
	EdgeTypeCPtrSet edge_set;
	const StateGraphNode* st_x = this;
	const EdgeType* edge_x = edge_ptr;


	//! get unique edge_ptr
	while (edge_x){
		edge_set.insert(edge_x);
		if (st_x->backpointer == nullptr) break;
		st_x = st_x->backpointer;
		edge_x = st_x->edge_ptr;
	}

	//! cumulate penalty of unique edge_ptr's
	int penalty = 0;
	for (auto x : edge_set){
		penalty += x->get_edge_cost();
	}

	return penalty;
}

