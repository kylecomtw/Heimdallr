#ifndef DEPRULENODEFACTORY_H
#define DEPRULENODEFACTORY_H

#include <vector>
#include <unordered_set>
#include <memory>
#include "DepRuleNode.h"
#include "DepRuleEngine.h"
#include "MatchedDepRule.h"

typedef std::unordered_set<DepRuleNode> RuleSet_t;
typedef std::unique_ptr<DepRuleNode> DepRuleNodeUP;
typedef std::unique_ptr<MatchedDepRule> MatchedDepRuleUP;

class DepRuleNodeFactory
{
private:	
	RuleSet_t rule_set;
	const DepRuleEngine* engine;
	DepRuleNodeUP generalize_node(const DepRuleNode* r);
	DepRuleNodeUP generalize_tree(const DepRuleNode* r);
	DepRuleNodeUP specialize_node(const DepRuleNode* r);
	DepRuleNodeUP specialize_tree(const DepRuleNode* r);
	bool		  is_any_terminal(const DepRuleNode* r);
	bool		  is_any_tree(const DepRuleNode* r);
	bool		  find_embedded_rule(DepRuleNodeUP& ruptr);
	
public:
	DepRuleNodeFactory(const DepRuleEngine* _e) : engine(_e) {}
	DepRuleNodeUP generalize(const DepRuleNode* r);
	DepRuleNodeUP specialize(const DepRuleNode* r);
	static MatchedDepRuleUP make_archetype(const DepRuleNode* r);
};

#endif //DEPRULENODEFACTORY_H
