#include "PNChart.h"

PNChart::PNChart(const MRuleSet& mset)
{
	for (const MRule& mrule_x : mset){
		//! Terminals participate in parsing with its parent node, TermRule,
		//! do not add them in chart directly
		if (mrule_x.matched_rule_class() != "Terminal"){
			mptr_set.insert(&mrule_x);
		}
	}
}

