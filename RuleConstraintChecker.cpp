#include "RuleConstraintChecker.h"
#include "RuleAffinityChecker.h"

RuleConstraintChecker::RuleConstraintChecker(const MatchedDepRule* m_rule)
{
	check_result = 0;
	RuleAffinityChecker::AffinityCheckData adata;	
	adata.strategy = ExpandStrategy::Init;
	adata.matcher = m_rule;
	RuleAffinityChecker affinity_checker;
	
	for (int i = 0; i < m_rule->get_children_count(); ++i){
		adata.matchee = m_rule->get_matched_children(i);
		adata.check_node_i = i;
		if (adata.matchee == nullptr) continue;
		check_result += affinity_checker.check_all(adata, AffinityAspect::Matcher);
		check_result += affinity_checker.check_all(adata, AffinityAspect::Matchee);								
	}	
}
