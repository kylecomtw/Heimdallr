#ifndef PNPARSER_H
#define PNPARSER_H

#include "PNAgenda.h"
#include "PNChart.h"
#include "MatchedDepRule.h"
#include "DepRuleEngine.h"
#include <unordered_set>

typedef MatchedDepRule MRule;
typedef std::vector<int> PuncPos_t;

class PNParser
{
private:
	PNAgenda&	agenda;
	PNChart&	chart;
	DepRuleEngine&	engine;
	PuncPos_t	punc_pos;

	void reset();
	void init();
	void process_edge(const MRule* edge);
	void fwd_fundamental(const MRule* edge);
	void bck_fundamental(const MRule* edge);
	void predict(const MRule* edge);
	void move_to_agenda(MRule* edge);
	void add_to_chart(const MRule* edge);
	bool check_position_valid(const MRule* leader,
		const MRule* follower, NodeConnRelation conn_rel);
	int find_npunc_between(int start, int end);
public:
	PNParser(PNAgenda& a, PNChart& c, DepRuleEngine& r) 
		: agenda(a), chart(c), engine(r){}
	int parse();
};

#endif // PNPARSER_H