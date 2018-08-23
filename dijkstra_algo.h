#ifndef DIJKSTRA_ALGO_H
#define DIJKSTRA_ALGO_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include "MatchedDepRule.h"
using std::vector;

typedef MatchedDepRule EdgeType;
typedef std::unordered_set<const EdgeType*> EdgeCPtrSetType;

struct SeqNodeState {
	int node_id;
	int cost;
	int bp_id;
};

struct IntPairType{
	int source;
	int target;
	bool operator == (const IntPairType& x) const{
		return source == x.source && target == x.target;
	}
};

template<>
struct std::hash<IntPairType>{
	size_t operator()(const IntPairType& x) const{
		return x.source ^ (x.target << 2);
	}
};

typedef std::unordered_map<IntPairType, const EdgeType*> EdgeLookupType;
typedef std::unordered_map<IntPairType, int> AdjacencyMapType;
typedef std::unordered_map<int, int> NodeIntMapType;

void find_partial_edges_dijkstra(int start, int end,
	const vector<const EdgeType*> completed_edges, 
	vector<const EdgeType*>& shortest_path);
void dijkstra_shortest_path(int start, int end,
	const EdgeCPtrSetType& edges,
	vector<const EdgeType*>& shortest_edges);

#endif //DIJKSTRA_ALGO_H