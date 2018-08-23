#ifndef STATEGRAPHNODE_H
#define STATEGRAPHNODE_H
#include <string>
#include <vector>
#include "MatchedDepRule.h"
#include <array>
#include <unordered_set>

using std::string;
using std::vector;

class StateGraphNode;
extern std::array<string, 7> ExpandStrategyStr;
typedef MatchedDepRule EdgeType;
typedef vector<ConstraintCheck> ConstCheckVec;
typedef std::unordered_set<const EdgeType*> EdgeTypeCPtrSet;
typedef std::unordered_set<StateGraphNode*> SGNodeSet;

class StateGraphNode
{
public:
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
	ConstCheckVec		const_vec;
	const EdgeType*	    edge_ptr;

	StateGraphNode*		branch_state;
	StateGraphNode*		backpointer;
	SGNodeSet			ancestors;
	SGNodeSet			descendents;
	

	StateGraphNode(const EdgeType* e_ptr, StateGraphNode* _p, ExpandStrategy _s);
	~StateGraphNode();

	void				print_trace() const;
	void				compute_cost(int n_token);
	vector<const EdgeType*>	get_edge_snapshot() const;
	operator string() const;

private:
	int get_completed_cost() const { return MatchedDepRule::alpha * edge_ptr->end_pos(); }
	int get_remaining_cost(int n_token) { return MatchedDepRule::alpha * (n_token - edge_ptr->end_pos()); };
	int get_edge_penalty() const;	
	int check_bckFundRule() const;

};

struct StateGraphNodeCostComparator
{
	bool operator() (const StateGraphNode* lhs, const StateGraphNode* rhs) const{
		return lhs->f_cost > rhs->f_cost;
	}
};

#endif //STATEGRAPHNODE_H