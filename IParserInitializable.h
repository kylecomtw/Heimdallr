#ifndef IPARSERINITIALIZABLE_H
#define IPARSERINITIALIZABLE_H
#include "DepRuleEngine.h"
#include "MatchedDepRule.h"
typedef MatchedDepRule EdgeType;

class IParserInitializable
{
public:	

	//!
	virtual const vector<DepToken>& get_term_tokens() = 0;

	//!
	virtual const vector<EdgeType*>& get_term_edges() = 0;

	//!
	virtual DepRuleEngine*  get_rule_engine() = 0;

	//! move_to_agenda takes an EdgeType* and move the content
	//! to agenda itself, the edge is null after the move.
	//! This function is used to add initiall edges into the agenda
	virtual void move_to_agenda(EdgeType* edge) = 0;

	//! move_to_chart takes an  EdgeType* and move the content
	//! to parsers internal buffer, and add the edge pointer into chart.
	//! This function is used to initializing the chart.
	virtual void move_to_chart(EdgeType* edge) = 0;

	//!
	virtual void remove_edge_in_chart(const EdgeType* edge) = 0;

	//! this function simply add the EdgeType* into chart,
	//! no copying, moving is involved
	virtual const EdgeType* add_to_chart(const EdgeType* edge) = 0;

};

#endif //IPARSERINITIALIZABLE_H