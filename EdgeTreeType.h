#ifndef EDGETREETYPE_H
#define EDGETREETYPE_H
#include "MatchedDepRule.h"

typedef MatchedDepRule EdgeType;
using std::vector;

class EdgeTreeType
{
private:
	EdgeType*			_parent;
	EdgeType*			_current;
	EdgeType*			_next_term;
	EdgeType*			_prev_term;
	vector<EdgeType*>	_children;

public:
	const EdgeType* parent() const { return _parent; }
	const EdgeType* next_match() const { return _next_term; }
};
#endif //EDGETREETYPE_H