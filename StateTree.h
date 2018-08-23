#ifndef STATETREE_H
#define STATETREE_H
#include <string>
#include <vector>
#include <array>
#include "MatchedDepRule.h"
#include "RuleConstraint.h"

using std::string;
using std::vector;

extern std::array<string, 7> ExpandStrategyStr;
typedef MatchedDepRule EdgeType;
typedef vector<ConstraintCheck> ConstCheckVec;

struct StateTree{
	static unsigned int	counter;
	unsigned int		id;
	unsigned int		loop_idx;
	unsigned int		f_cost; //! g_cost + h_cost
	unsigned int		g_cost; //! actual cost from initial to current state
	unsigned int		h_cost;	//! heuristic cost from current state to initial
	unsigned int		affinity_penalty;
	bool				attempted;
	bool				inqueue;
	ExpandStrategy		strategy;
	vector<StateTree*>	children;
	ConstCheckVec		const_vec;
	StateTree*			parent;
	const StateTree*	branch_state;
	const EdgeType*	    edge_ptr;
	
	StateTree(const EdgeType* e_ptr, StateTree* _p, const StateTree* emit_state, ExpandStrategy _s);
	~StateTree();	

	void				print_tree() const;	
	void				compute_cost(int n_token);
	int					compute_state_cost();
	vector<const EdgeType*>	get_edge_snapshot() const;
	operator string() const;

private:
	int get_completed_cost() const { return MatchedDepRule::alpha * edge_ptr->end_pos(); }
	int get_remaining_cost(int n_token);

};

struct StateTreeCostComparator
{
	bool operator() (const StateTree* lhs, const StateTree* rhs) const{
		if (lhs->f_cost != rhs->f_cost)
			return lhs->f_cost > rhs->f_cost;		
		if (lhs->edge_ptr->start_pos() != rhs->edge_ptr->start_pos())
			return lhs->edge_ptr->start_pos() > rhs->edge_ptr->start_pos();
		if (lhs->edge_ptr->end_pos() != rhs->edge_ptr->end_pos())
			return lhs->edge_ptr->end_pos() > rhs->edge_ptr->end_pos();
		
		return lhs->edge_ptr->matched_rule_label() > rhs->edge_ptr->matched_rule_label();
	}
};


#endif //STATETREE_H