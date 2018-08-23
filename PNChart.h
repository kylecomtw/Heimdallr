#ifndef PNCHART_H
#define PNCHART_H

#include <unordered_set>
#include <stack>
#include <vector>
#include "MatchedDepRule.h"

typedef MatchedDepRule MRule;
typedef std::unordered_set<MRule> MRuleSet;
typedef std::unordered_set<const MRule*> CPMRuleSet;

class PNChart
{
private:
	CPMRuleSet mptr_set;

public:
	PNChart(const MRuleSet& mset);
	void add(const MRule* x) { mptr_set.insert(x); }

	CPMRuleSet::iterator begin() { return mptr_set.begin(); }
	CPMRuleSet::iterator end() { return mptr_set.end(); }
	

};

#endif // PNCHART_H