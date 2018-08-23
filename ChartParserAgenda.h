#ifndef CHARTPARSERAGENDA_H
#define CHARTPARSERAGENDA_H

#include <unordered_set>
#include <array>
#include <queue>
#include <exception>
#include "MatchedDepRule.h"
#include "StateTree.h"
#include <string>
#include <unordered_map>

using std::string;
typedef MatchedDepRule EdgeType;
typedef std::unordered_set<EdgeType> EdgeSetType;
typedef std::unordered_map<const EdgeType*, const StateTree*> EdgeStateMap_t;
typedef vector<StateTree*>* STChildrenVecPtr;
typedef const StateTree*		CPStateTree;
typedef std::priority_queue<StateTree*, 
		std::vector<StateTree*>, 
		StateTreeCostComparator> st_pqueue_t;

struct AgendaInsert_t { 
	const EdgeType* ansc_edge; 
	const StateTree* src_state; 
	ExpandStrategy strategy; 
};

class ChartParserAgendaException : public std::exception{};

class ChartParserAgenda
{	
private:
	
	StateTree*			check_StateTree_ptr(const StateTree* st_ptr);
	StateTree*			create_new_state(const EdgeType* e_ptr, StateTree* _p, 
										 const EdgeType* ansc_edge, ExpandStrategy _s);
	void				set_state_attempted(StateTree* st);	
	unsigned int		pop_counter;
	int				n_token;
	int				unattempted_count;
	int				rightmost_dot_pos;	
	
	StateTree*		state_root;	
	EdgeSetType		edge_set;	
	EdgeStateMap_t  edge_state_map;
	st_pqueue_t		work_queue;
	vector<StateTree*>	root_vec;

public:	
	ChartParserAgenda();		
	~ChartParserAgenda();

	//! constant accessor	
	int			dot_pos() const;
	int			n_expanded() const { return edge_set.size(); }
	int			n_unattempted() const { return unattempted_count; }
	int			n_queue() const { return work_queue.size(); }
	bool		empty() const;	
	bool		atEnd() const;

	//! public action function
	void        set_input_length(int _n_token){ n_token = _n_token; }
	void		reset();
	void		make_root();
	void		insert(const EdgeType&& src, AgendaInsert_t insert_data);
	const StateTree*	pop();		
	const EdgeType*		find_edge(int idx) const;
	const StateTree*	find_state(const EdgeType*) const;
	vector<const EdgeType*> get_edge_snapshot(int state_idx) const;

	//! StateTree accessors	
	// const StateTree*	get_state_cursor() const { return state_cursor; }
	const vector<CPStateTree>	get_state_roots() const;
	
};

#endif //CHARTPARSERAGENDA_H