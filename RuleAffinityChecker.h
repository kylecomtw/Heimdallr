#ifndef RULEAFFINITYCHECKER_H
#define RULEAFFINITYCHECKER_H

#include "MatchedDepRule.h"
#include "RuleConstraint.h"

enum class AffinityAspect{Matcher, Matchee};


class RuleAffinityChecker
{
public:
	struct AffinityCheckData{		
		int						check_node_i;
		AffinityAspect			aspect;
		ExpandStrategy			strategy;		
		const MatchedDepRule*	matcher;
		const MatchedDepRule*	matchee;		
		const RuleConstraint*	rule_con;
	};
	RuleAffinityChecker();
	int check(AffinityCheckData& check_data);
	int check_all(AffinityCheckData& check_data, AffinityAspect asp);
	const vector<ConstraintCheck> get_constraint_vec() { return constraint_vec; }

private:	
	vector<ConstraintCheck> constraint_vec;
	int check_rule_class(const AffinityCheckData& check_data);
	int check_rule_label(const AffinityCheckData& check_data);
	int check_parent_class(const AffinityCheckData& check_data);
	int check_parent_label(const AffinityCheckData& check_data);
	int check_simple(const AffinityCheckData& check_data);
	int check_pos(const AffinityCheckData& check_data);
	int check_strategy(const AffinityCheckData& check_data);
};

#endif //RULEAFFINITYCHECKER_H