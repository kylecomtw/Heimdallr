#include "PNAgenda.h"

PNAgenda::PNAgenda(MRuleSet& mrs) : mrule_set(mrs)
{
	for (const MRule& mr_x : mrs){
		//! Terminals participate in parsing with its parent node, TermRule,
		//! do not add them in chart directly
		if (mr_x.matched_rule_class() != "Terminal"){
			stack.push(&mr_x);
		}
	}
}

void PNAgenda::add(MRule* r)
{
	if (r->get_match_penalty() >= MRule::sink_coeff){
		dprintf("[Sink edge]: %s\n", ((string)*r).c_str());
		return;
	}

	auto ret = mrule_set.insert(std::move(*r)); 
	bool insert_ok = ret.second;
	if (insert_ok){
		const MRule* rptr = &(*ret.first);
		stack.push(rptr);
	} else {
		printf("[Duplicate edge]: %s\n", ((string)*r).c_str());
	}
}

bool PNAgenda::empty()
{
	return stack.empty();
}

size_t PNAgenda::size()
{
	return stack.size();
}

const MRule* PNAgenda::pop()
{
	if (!stack.empty()){
		const MRule* mr = stack.top();
		stack.pop();
		return mr;
	}

	return nullptr;
}

void PNAgenda::clear_active_rules()
{
	printf("There are %d rules in mrule_set\n", mrule_set.size());
	MRuleSet::iterator iter = mrule_set.begin();
	MRuleSet::iterator iter_end = mrule_set.end();
	while (iter != iter_end){		
		
		//! MRule from normalized rule cannot be removed, or there will be dangling pointer
		if ((*iter).is_active()){
			iter = mrule_set.erase(iter);
		} else {			
			dprintf("MRule[%s]: %s\n", (*iter).matched_rule_class().c_str(), ((string)*iter).c_str());
			++iter;
		}
		

	}		
	printf("After cleaning active ruels, %d rules left in mrule_set\n", mrule_set.size());

}