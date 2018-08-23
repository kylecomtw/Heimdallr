#include <algorithm>
#include "ChartParserChart.h"

void ChartParserChart::reset(int new_end_pos)
{
	end_pos = new_end_pos;
	clear();
}

void ChartParserChart::clear()
{
	edge_set.clear();
	completed_edges.clear();
}

ChartParserChart::pair_t
ChartParserChart::insert(const EdgeType* edge)
{		

	pair_t pair = edge_set.insert(edge);
	bool has_inserted = pair.second;
	//! check if incoming edge is a complete edge.
	if (has_inserted &&
		!edge->is_active() &&
		edge->start_pos() == 0 && edge->end_pos() == end_pos){

		//! only store the edge when it is passed by complete_edge_pred
		//! following condition is equivalent to:
		//! complete_edge_pred -> complete_edge_pred(edge)
		if (!complete_edge_pred || complete_edge_pred(edge)) {
			completed_edges.push_back(*pair.first);
		}
		// sort_full_edges();
	}

	return pair;
}

EdgeCPtrVec ChartParserChart::get_inactive_edges() const
{
	vector<const EdgeType*> ret_vec;
	for (auto& x : edge_set){
		if (x->is_active()) continue;
		ret_vec.push_back(x);
	}

	return ret_vec;
}

void ChartParserChart::sort_full_edges()
{
	// sort completed edges by match cost in ascending order	
	std::sort(completed_edges.begin(), completed_edges.end(),
		[](const EdgeType* edge_a, const EdgeType* edge_b){
		return edge_a->get_tree_match_cost() < edge_b->get_tree_match_cost();
	});
}

int ChartParserChart::get_full_edges_count() const
{
	return completed_edges.size();
}

const EdgeCPtrVec& ChartParserChart::get_full_edges() const
{
	return completed_edges;
}
