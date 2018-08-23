#include "RuleAffinityChecker.h"
#include "RuleConstraintChecker.h"

extern std::array<string, 7> ExpandStrategyStr;

RuleAffinityChecker::RuleAffinityChecker()
{
}

int RuleAffinityChecker::check_all(AffinityCheckData& check_data, AffinityAspect asp)
{	
	int check_result = 0;
	const DepRuleNode* asp_rule = nullptr;	
	if (asp == AffinityAspect::Matcher){
		asp_rule = &check_data.matcher->get_rule_node();
	} else {
		asp_rule = &check_data.matchee->get_rule_node();
	}

	check_data.aspect = asp;
	for (int i = 0; i < asp_rule->get_rule_constraints().size(); ++i)
	{		
		check_data.rule_con = &asp_rule->get_rule_constraints().at(i);
		int result = check(check_data);
		check_result += result;
		if (result != 0){
			constraint_vec.emplace_back(ConstraintCheck{ i, result, true });
		}
	}

	return check_result;
}

int RuleAffinityChecker::check(AffinityCheckData& check_data)
{
	int check_result = 0;
	int is_satisfied = 0;
	const RuleConstraint* rc = check_data.rule_con;

	if (rc->apply_node >= 0 && rc->apply_node != check_data.check_node_i){
		//! the affinity_score is 0 when current node is not rc->apply_node
		check_result = 0;
		return check_result;
	} else {
		switch (rc->cond){
		case RCCondition::Branching:
			if (check_data.aspect != AffinityAspect::Matcher){
				is_satisfied = 1;
			} else {
				const MatchedDepRule* matchee = check_data.matchee;
				is_satisfied = (int)(matchee->end_pos() - matchee->start_pos() > 1);
			}
			break;
		case RCCondition::RuleClass:
			is_satisfied = check_rule_class(check_data);			
			break;
		case RCCondition::POS:
			is_satisfied = check_pos(check_data);
			break;
		case RCCondition::NoParent:
			if (check_data.aspect != AffinityAspect::Matchee){
				is_satisfied = 1;
			} else {
				//! NoParent constraint means the node cannot have a parent.
				//! but if the aspect is Matchee, indicates the node already has a parent
				//! therefore the constraint is not satisfied.
				is_satisfied = 0;
			}
		case RCCondition::ParentClass:			
			is_satisfied = check_parent_class(check_data);			
			break;
		case RCCondition::ParentLabel:			
			is_satisfied = check_parent_label(check_data);			
			break;
		case RCCondition::Strategy:
			is_satisfied = check_strategy(check_data);
			break;
		case RCCondition::Simple:
			is_satisfied = check_simple(check_data);
			break;
		default:
			_ASSERT(false);
			break;
		}
	}

	if (rc->valence == 1){
		check_result = is_satisfied * rc->weight;
	} else if (rc->valence == -1){
		check_result = (1-is_satisfied) * rc->weight;
	} else {
		//! sink constraint, it impose a large cost to make the cost infeasible
		check_result = (1 - is_satisfied) * MatchedDepRule::sink_coeff;
	}

	return check_result;
}

int RuleAffinityChecker::check_rule_class(const AffinityCheckData& check_data)
{

	const MatchedDepRule* matchee = check_data.matchee;
	const RuleConstraint* rc_ptr = check_data.rule_con;
	if (check_data.aspect != AffinityAspect::Matcher){
		return 1;
	}

	RCRelOpComp comp(rc_ptr->rel_op);
	return comp(matchee->matched_rule_class(), rc_ptr->arg);	
}

int RuleAffinityChecker::check_rule_label(const AffinityCheckData& check_data)
{
	const MatchedDepRule* matchee = check_data.matchee;
	const RuleConstraint* rc_ptr = check_data.rule_con;
	if (check_data.aspect != AffinityAspect::Matcher){
		return 1;
	}

	RCRelOpComp comp(rc_ptr->rel_op);
	return comp(matchee->matched_rule_label(), rc_ptr->arg);

}

int RuleAffinityChecker::check_parent_class(const AffinityCheckData& check_data)
{
	const MatchedDepRule* matcher = check_data.matcher;
	const RuleConstraint* rc_ptr = check_data.rule_con;
	if (check_data.aspect != AffinityAspect::Matchee) {
		return 1;
	}

	const DepRuleNode& rule = matcher->get_rule_node();
	RCRelOpComp comp(rc_ptr->rel_op);
	return comp(rule.get_rule_class(), rc_ptr->arg);

}

int RuleAffinityChecker::check_parent_label(const AffinityCheckData& check_data)
{
	const MatchedDepRule* matcher = check_data.matcher;
	const RuleConstraint* rc_ptr = check_data.rule_con;
	if (check_data.aspect != AffinityAspect::Matchee) {
		return 1;
	}

	const DepRuleNode& rule = matcher->get_rule_node();
	RCRelOpComp comp(rc_ptr->rel_op);
	return comp(rule.get_rule_label(), rc_ptr->arg);
}

int RuleAffinityChecker::check_pos(const AffinityCheckData& check_data)
{
	const MatchedDepRule* matchee = check_data.matchee;
	const RuleConstraint* rc_ptr = check_data.rule_con;
	if (check_data.aspect != AffinityAspect::Matcher){
		return 1;
	}
	
	RCRelOpComp comp(rc_ptr->rel_op);
	return comp(matchee->matched_pos(), rc_ptr->arg);
}

int RuleAffinityChecker::check_simple(const AffinityCheckData& check_data)
{
	const MatchedDepRule* matchee = check_data.matchee;
	const RuleConstraint* rc_ptr = check_data.rule_con;
	if (check_data.aspect != AffinityAspect::Matcher){
		return 1;
	}

	int matchee_depth = matchee->get_depth();
	RCRelOpComp comp(RCRelOp::LessThan);
	return comp(matchee_depth, 3);
}

int RuleAffinityChecker::check_strategy(const AffinityCheckData& check_data)
{	
	const RuleConstraint* rc_ptr = check_data.rule_con;
	if (check_data.aspect != AffinityAspect::Matchee){
		return 1;
	}
	
	RCRelOpComp comp(rc_ptr->rel_op);
	const string& strategy = ExpandStrategyStr[(int)check_data.strategy];
	return comp(strategy, rc_ptr->arg);
}

