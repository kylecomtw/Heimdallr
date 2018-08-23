#ifndef CHARTPARSERCHART_H
#define CHARTPARSERCHART_H

#include <unordered_set>
#include "MatchedDepRule.h"
#include <functional>

typedef MatchedDepRule EdgeType;
typedef std::unordered_set<const EdgeType*> EdgeCPtrSetType;
typedef vector<const EdgeType*>		 EdgeCPtrVec;
typedef std::function<bool(const EdgeType*)> CompleteEdgePred;

class ChartParserChart
{
private:
	int			n_complete;
	int			end_pos;	
	EdgeCPtrVec		completed_edges;
	EdgeCPtrVec		full_edges;
	EdgeCPtrSetType edge_set;
	void			clear();
	void			sort_full_edges();
	CompleteEdgePred complete_edge_pred;

public:
	typedef EdgeCPtrSetType::iterator		iterator_t;
	typedef std::pair<iterator_t, bool>		pair_t;

	ChartParserChart() : n_complete(0), end_pos(0){}

	// interface of underlying edge_set	
	pair_t		insert(const EdgeType* edge);
	iterator_t	begin() { return edge_set.begin(); }
	iterator_t	end()	{ return edge_set.end(); }
	size_t		size() const { return edge_set.size(); }
	void		erase(const EdgeType* edge) { edge_set.erase(edge); }

	void set_complete_predicate(CompleteEdgePred pred) { complete_edge_pred = pred; }

	// Chart operations
	void reset(int end_dot_pos = 0);
	int  get_full_edges_count() const;
	EdgeCPtrVec get_inactive_edges() const;
	const EdgeCPtrVec& get_full_edges() const;

};

#endif //CHARTPARSERCHART_H