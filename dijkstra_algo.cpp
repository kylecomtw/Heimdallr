#include "dijkstra_algo.h"
#include <limits>
#include <algorithm>

void find_partial_edges_dijkstra(int start, int end, 
	const vector<const EdgeType*> completed_edges, vector<const EdgeType*>& shortest_path)
{
	//! In chart parser, terminals and term rules have zero cost. That is to reflect the fact
	//! those rules are dummy rule for parsing operations. Same considerations apply for dummy rules
	//! used for labeling, or categorizing MatchedDepRule.
	//! However, when finding the shortest path, these rule spanning one node should be
	//! actually the "costive" route. Therefore the cost of terminal rules are set to TERM_COST
	const int TERM_COST = 1e3;
	//! construct nodes in nodes graph	

	//! construct the edges in nodes graph composed of dot positions in the sequence
	std::unordered_map<IntPairType, const EdgeType*> link_map;
	std::unordered_map<IntPairType, int> cost_map;
	for (auto edge_x : completed_edges) {
		IntPairType int_pair{ edge_x->start_pos(), edge_x->end_pos() };
		int cost_x = edge_x->get_edge_cost();		
		NodeType ndtype = edge_x->get_rule_node().get_node_type();
		if (ndtype == NodeType::NdTerminal) continue;
		if (int_pair.target - int_pair.source == 1) {
			cost_x = TERM_COST;
		}

		if (link_map.count(int_pair) == 0) {
			link_map[int_pair] = edge_x;
			cost_map[int_pair] = cost_x;
		} else {
			if (cost_x < cost_map[int_pair]) {
				link_map[int_pair] = edge_x;
				cost_map[int_pair] = cost_x;
			}
		}		
	}	

	//! initialize SeqNode state for dijkstra
	vector<int> V;
	vector<SeqNodeState> Q;
	for (int i = start; i <= end; ++i) {
		SeqNodeState st{ i, INT32_MAX, -1 };
		Q.push_back(st);
		V.push_back(i);
	}
	Q[0].cost = 0;

	//! find the shortest path with Dijkstra's algorithm
	while (V.size() > 0) {
		std::sort(V.begin(), V.end(), [&Q](int x, int y) {return Q[x].cost > Q[y].cost; });
		int nd_from = V.back();
		V.pop_back();

		for (auto item : link_map) {
			IntPairType pair = item.first;
			if (pair.source != nd_from) continue;
			int nd_to = pair.target;
			try {
				int item_cost = cost_map.at(pair);

				if (item_cost + Q[nd_from].cost < Q[nd_to].cost) {
					Q[nd_to].cost = item_cost + Q[nd_from].cost;
					Q[nd_to].bp_id = nd_from;
				}
			} catch (std::out_of_range& ex) {
				printf("Out of range exception: %s", ex.what());
			}
		}
	}

	//! reconstruct the shortest path in edges
	for (int qi = Q.size() - 1; qi > 0; ) {
		const EdgeType* edge_q = nullptr;
		if (Q[qi].bp_id >= 0) {
			IntPairType int_pair{ Q[qi].bp_id, qi };
			edge_q = link_map[int_pair];
			qi = Q[qi].bp_id;
		} else {
			IntPairType int_pair{ qi - 1, qi };
			edge_q = link_map[int_pair];
			qi = qi - 1;
		}
		if(edge_q) shortest_path.push_back(edge_q);		
	}

	std::reverse(shortest_path.begin(), shortest_path.end());
}


void dijkstra_shortest_path(int start, int end,
	const EdgeCPtrSetType& edges,
	vector<const EdgeType*>& shortest_edges)
{
	std::unordered_set<int>		 Q;
	std::stack<int>				 V;
	NodeIntMapType				 backtrack;
	NodeIntMapType				 cost;
	AdjacencyMapType			 links;
	EdgeLookupType				 link_lut;

	for (const auto* e : edges){
		int s = e->start_pos(), t = e->end_pos();
		IntPairType nd_pair = IntPairType{ s, t };
		link_lut[nd_pair] = e;
		links[nd_pair] = e->get_tree_match_cost();
		// dprintf("[tree score] %s: %d\n", ((string)e).c_str(), links[nd_pair]);
		int a = 1;
	}

	for (int i = start+1; i <= end; ++i){
		links[IntPairType{ i - 1, i }] = 1;
		Q.insert(i);
	}

	// Dijkstra algorithm
	V.push(start);
	while (V.size() > 0){
		int u = V.top(); V.pop();
		vector<int> buf;
		for (int i = start + 1; i <= end; ++i){
			if (links.count(IntPairType{ u, i }) == 0) continue;
			buf.push_back(i);
		}

		for (int v : buf){
			if (Q.count(v) > 0){ Q.erase(v); V.push(v); }
			int cost_x = 0;
			if (cost.count(u) == 0){
				cost_x = links[IntPairType{ u, v }];
			} else {
				cost_x = links[IntPairType{ u, v }] + cost[u];
			}
			
			if (cost.count(v) == 0 || cost_x < cost[v]){
				cost[v] = cost_x;
				backtrack[v] = u;
			}
		}
	}

	// find edge used in the shortest path
	int cursor = end;
	int edge_start = 0;
	while (cursor != start){
		edge_start = backtrack[cursor];
		IntPairType nd_key{ edge_start, cursor };
    	if (link_lut.count(nd_key) > 0){
			shortest_edges.push_back(link_lut[nd_key]);			
		} else if (cursor - edge_start == 1){
			// direct connect
		} else {
			printf("[ERROR] problems in finding shortes path\n");
		}
		cursor = edge_start;
		
	}

	std::reverse(shortest_edges.begin(), shortest_edges.end());
	return;

}
