#include "StateTree.h"
#include "MatchedDepRule.h"
#include "DepRuleEngine.h"

void test_compute_state_cost()
{
	DepRuleEngine dep_rule_eng;
	dep_rule_eng.import_dep_rules("../etc/dep_rules_gram.txt");
	dep_rule_eng.import_dep_rules("../etc/dep_rules_lex.txt");
	dep_rule_eng.import_dep_rules("../etc/dep_rules_sen.txt");
	dep_rule_eng.post_process_rules();
	
	const DepRuleNode* rule = dep_rule_eng.find_rule("PUNC01");
	const DepRuleNode* rule_m01_or = dep_rule_eng.find_rule("M01_Or_0");
	DepRuleNode* term_rule = DepRuleNode::make_terminal_rule("Term", "TT");

	MatchedDepRule mrule(*rule, 0, 5);
	MatchedDepRule mrule_m01_or(*rule_m01_or, 1, 3);
	MatchedDepRule* mrule_term = MatchedDepRule::make_matched_terminal(term_rule, 0, 1);
	mrule.advance_dot_with(mrule_term);
	mrule.advance_dot_with(&mrule_m01_or);
	StateTree* st = new StateTree(&mrule, nullptr, nullptr, ExpandStrategy::Init);
	int result = st->compute_state_cost();

	const int answer = 1000330;
	if (result == answer){
		printf("test passed: %d == %d\n", result, answer);
	} else {
		printf("test failed: %d != %d\n", result, answer);
	}

	delete term_rule;
	delete mrule_term;
}

void test_back_predict()
{
	DepRuleEngine dep_rule_eng;
	dep_rule_eng.import_dep_rules("../etc/dep_rules_gram.txt");	
	dep_rule_eng.post_process_rules();
	dep_rule_eng.set_context_to_all();
	const DepRuleNode* rule_d01a = dep_rule_eng.find_rule("D01a");
	const DepRuleNode* rule_g05 = dep_rule_eng.find_rule("G05");
	MatchedDepRule mrule_d01a(*rule_d01a, 1, 3);
	DepRuleNode* term_rule = DepRuleNode::make_terminal_rule("Term", "TT");
	MatchedDepRule* mrule_term = MatchedDepRule::make_matched_terminal(term_rule, 0, 1);
	PC_RuleList rule_list = dep_rule_eng.predict_bu(mrule_d01a);

	MatchedDepRule mrule_g05(*rule_g05, 0, 4);
	mrule_g05.advance_dot_with(mrule_term);


	dprintf("==== Back prediction test ====\n");
	for (const DepRuleNode* rn : rule_list){
		dprintf("%s\n", ((string)*rn).c_str());
	}

	
	delete term_rule;
	delete mrule_term;
}