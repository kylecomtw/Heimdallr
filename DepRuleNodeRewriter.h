#ifndef DEPRULENODEFACTORY_H
#define DEPRULENODEFACTORY_H

#include <vector>
#include <unordered_set>
#include "DepRuleNode.h"
#include "DepRuleEngine.h"

class DepRuleNodeRewriter
{
private:		
	const DepRuleEngine* engine;
	DepRuleNode&& generalize_node(const DepRuleNode* r);
	DepRuleNode&& generalize_tree(const DepRuleNode* r);
	DepRuleNode&& specialize_node(const DepRuleNode* r);
	DepRuleNode&& specialize_tree(const DepRuleNode* r);

public:
	DepRuleNodeRewriter(const DepRuleEngine* _e) : engine(_e) {}
	DepRuleNode&& generalize(const DepRuleNode* r);
	DepRuleNode&& specialize(const DepRuleNode* r);
};

#endif //DEPRULENODEFACTORY_H
