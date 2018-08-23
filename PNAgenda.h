#ifndef PNAGENDA_H
#define PNAGENDA_H

#include <unordered_set>
#include <stack>
#include "MatchedDepRule.h"

typedef MatchedDepRule MRule;
typedef std::unordered_set<MRule> MRuleSet;
typedef std::stack<const MRule*> CPMRule_Stack;

class PNAgenda
{
private:
	MRuleSet& mrule_set;
	CPMRule_Stack stack;

public:
	PNAgenda(MRuleSet& mrs);
	void	add(MRule* r);	
	bool	empty();
	size_t	size();
	const MRule* pop();
	void	clear_active_rules();
};

#endif // PNAGENDA_H

