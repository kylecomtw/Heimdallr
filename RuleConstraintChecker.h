#ifndef RULECONSTRAINTCHECKER_H
#define RULECONSTRAINTCHECKER_H

#include "MatchedDepRule.h"
#include "RuleConstraint.h"

class RuleConstraintChecker
{
public:
	RuleConstraintChecker(const MatchedDepRule* m_rule);
	int result() const { return check_result; };
private:
	int check_result;
};

#endif //RULECONSTRAINTCHECKER_H