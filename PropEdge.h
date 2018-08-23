#ifndef PROPEDGE_H
#define PROPEDGE_H

#include "PropNode.h"
#include <unordered_map> 
#include <unordered_set>
#include <functional>

enum class PropEdgeType {
	Other = 0, Dependency = 1, Rule = 2, Reference = 3, LDA = 4, Identity = 5, Sequence = 6
};

struct PropEdgeInfo {
	std::array<bool, 7> edge_type;
	double weight;

	PropEdgeInfo(){
		std::fill(edge_type.begin(), edge_type.end(), false); 
	}

	void set_type_on(PropEdgeType petype){ edge_type[(int)petype] = true; }
	bool has_type(PropEdgeType petype) const{ return edge_type[(int)petype]; }
};

class PropEdge
{
public:
	const PropNode* a;
	const PropNode* b;	
	PropEdge() :a(nullptr), b(nullptr){}
	bool operator == (const PropEdge& other) const{
		return this->a == other.a && this->b == other.b;
	}

	PropEdge(const PropEdge& other){
		this->a = other.a;
		this->b = other.b;
	}
};


template<> struct std::hash<PropEdge>{
	size_t operator()(const PropEdge& pe) const{
		return std::hash<const PropNode*>()(pe.a) ^ std::hash<const PropNode*>()(pe.b);
	}
};

typedef std::unordered_map<PropEdge*, PropEdgeInfo> EdgeInfo_map_t;
typedef std::unordered_set<PropEdge> PropEdge_set_t;

#endif //PROPEDGE_H