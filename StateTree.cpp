#include "StateTree.h"
#include "RuleAffinityChecker.h"
#include <sstream>
#include <iomanip>
#include <unordered_set>

std::array<string, 7> ExpandStrategyStr = { "Init", "FP", "BP", "FFR", "BFR", "RFR", "UFR" };
typedef std::unordered_set<const EdgeType*> EdgeTypeCPtrSet;
unsigned int StateTree::counter = 0;

StateTree::StateTree(
	const EdgeType* e_ptr, StateTree* _p, 
	const StateTree* emit_state, ExpandStrategy _s) :
	id(counter++), loop_idx(0),
	f_cost(0), g_cost(0), h_cost(0),
	affinity_penalty(0),
	attempted(false), inqueue(false),
	edge_ptr(e_ptr), branch_state(nullptr), parent(_p), strategy(_s)
{
	if (parent != nullptr){
		parent->children.push_back(this);
	}
	
	if (edge_ptr && edge_ptr->start_pos() == 1 && edge_ptr->end_pos() == 5 && edge_ptr->matched_rule_label()=="SR02") {
		int a = 1;
	}

	if (strategy == ExpandStrategy::FwdPredict){
		//! FP change edge_ptr, and use parent->edge_ptr as edge_parent
		branch_state = parent;
	} else if (strategy == ExpandStrategy::BckPredict){
		//! BP change edge_ptr, but does not change edge_parent
		branch_state = parent->branch_state;
	} else if (strategy == ExpandStrategy::FwdFundRule || 
		strategy == ExpandStrategy::UniFundRule){
		//! FFR does not change edge_ptr, nor edge_parent
		branch_state = parent->branch_state;
	} else if (strategy == ExpandStrategy::BckFundRule ||
		strategy == ExpandStrategy::RepFundRule){
		//! BFR put edge_ptr to edge_parent and edge_parent to edge_ptr's parent
		if (emit_state->branch_state){
			branch_state = emit_state->branch_state;
		}	
	} else {
		//! Init strategy
	}

}

StateTree::~StateTree()
{
	for (auto x : children){
		delete x;
	}
	children.clear();
}

StateTree::operator string() const
{
	int attempted = 0;
	for (auto x : children){ attempted += x->attempted; }
	std::stringstream ss;
	ss << std::setw(3) << loop_idx << ". ";
	ss << "[" << attempted << "/" << children.size() << "]";
	ss << "[" << ExpandStrategyStr[(int)strategy] << "]";
	if (edge_ptr != nullptr){		
		ss << (string)*edge_ptr;
	}
	ss << "[f/g/h: " << f_cost << "/" << g_cost << "/" << h_cost << "]";

	return ss.str();
}

void StateTree::print_tree() const
{
#ifdef __DEBUG__
	std::stringstream ss;
	ss << "----- Tree Trace (leaf->root)-----\n";
	int depth = 0;
	const StateTree* st = this;
	while (st != nullptr){
		ss << "[" << std::setw(2) << std::setfill(' ') << depth << "]";
		ss << (string)*st << "\n";
		if (st->parent == nullptr) break;
		st = st->parent;
		depth += 1;
		dprintf("%s", ss.str().c_str());
		ss.str(string());
	}
	ss << std::setw(20) << std::setfill('-') << "";
	dprintf("%s\n", ss.str().c_str());
#endif // __DEBUG__
}

void StateTree::compute_cost(int n_token)
{
	g_cost = compute_state_cost();
	//! heuristic cost from current state to goal
	h_cost = get_remaining_cost(n_token);
	//! cost of current state
	f_cost = g_cost + h_cost;
}

int StateTree::get_remaining_cost(int n_token)
{ 
	int remaining_tok = n_token - edge_ptr->end_pos();
	return MatchedDepRule::alpha * remaining_tok; 
}


vector<const EdgeType*> StateTree::get_edge_snapshot() const
{
	vector<const EdgeType*> ret;
	const StateTree* st = this;	
	ret.push_back(st->edge_ptr);
	
	while (st->branch_state){
		st = st->branch_state;		
		ret.push_back(st->edge_ptr);
	}

	return ret;
}

int StateTree::compute_state_cost()
{
	
	unsigned int state_cost = 0;
	//! the edge_ptr_vec is pushed from leaf to root.
	//! The first element in edge_ptr_vec is the edge closest to leaf.
	vector<const EdgeType*> edge_ptr_vec = this->get_edge_snapshot();
	const EdgeType* buf_ptr = nullptr;
	int edge_depth = 0;

	for (int i = 0; i < edge_ptr_vec.size(); ++i){
		const EdgeType* edge_ptr = edge_ptr_vec[i];
		int edge_dot = edge_ptr->dot_pos();
		state_cost += edge_ptr->get_tree_match_cost();		
		if (buf_ptr && edge_dot < edge_ptr->get_children_count()){
			RuleAffinityChecker affinity_checker;
			RuleAffinityChecker::AffinityCheckData adata;
			adata.matcher = edge_ptr;
			adata.matchee = buf_ptr;			
			adata.check_node_i = edge_ptr->dot_pos();
			adata.strategy = this->strategy;

			//! matcher-side constraint
			state_cost += affinity_checker.check_all(adata, AffinityAspect::Matcher);

			//! matchee-side constraint
			state_cost += affinity_checker.check_all(adata, AffinityAspect::Matchee);
			
			this->const_vec = affinity_checker.get_constraint_vec();
			
		}
		buf_ptr = edge_ptr;
	}

	return state_cost;
}
