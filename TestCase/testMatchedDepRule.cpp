#ifdef _TEST_
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include "../DepRuleEngine.h"
#include "../MatchedDepRule.h"
#include "unicode/unistr.h"

namespace fs = boost::filesystem;
#include <string>
#include <iostream>
using std::string;

extern string Boost_Test_Dir;

BOOST_AUTO_TEST_SUITE(MatchedDepRuleTest)

BOOST_AUTO_TEST_CASE(Test_SimpleMatching)
{
	DepRuleEngine engine;
	string test_rule_path = Boost_Test_Dir + "/matched/test_match_rules_1.txt";
	if (fs::exists(test_rule_path)){
		engine.import_dep_rules(test_rule_path);
	}

	engine.post_process_rules();
	DepRuleNode* rule = engine.get_rules_vec()[0];
	MatchedDepRule m1(*rule, 0, 0);
	DepRuleNode* rt1 = DepRuleNode::make_terminal_rule("", "N");
	MatchedDepRule* mt1 = MatchedDepRule::make_matched_terminal(rt1, 0, 1);
	DepRuleNode* rt2 = DepRuleNode::make_terminal_rule("", "V");
	MatchedDepRule* mt2 = MatchedDepRule::make_matched_terminal(rt2, 1, 2);	
	MatchedDepRule* mt3 = MatchedDepRule::make_matched_terminal(rt1, 2, 3);

	m1.advance_dot_with(mt1);
	m1.advance_dot_with(mt2);
	m1.advance_dot_with(mt3);

	BOOST_CHECK(m1.is_realization_of(rule));

	delete rt1, rt2;
	delete mt1, mt2, mt3;
}

BOOST_AUTO_TEST_CASE(Test_AnchorStartMatching)
{
	DepRuleEngine engine;
	string test_rule_path = Boost_Test_Dir + "/matched/test_match_rules_2.txt";
	if (fs::exists(test_rule_path)){
		engine.import_dep_rules(test_rule_path);
	}

	engine.post_process_rules();
	DepRuleNode* rule_1 = engine.get_rules_vec()[0];
	DepRuleNode* rule_2 = engine.get_rules_vec()[1];
	MatchedDepRule m1(*rule_1, 0, 0);
	MatchedDepRule m2(*rule_2, 0, 0);

	DepRuleNode* rt1_mid = DepRuleNode::make_terminal_rule("", "N");
	DepRuleNode* rt1_start = DepRuleNode::make_terminal_rule("", "N");
	rt1_start->set_anchor(AnchorType::AnchorStart);
	MatchedDepRule* mt1_mid = MatchedDepRule::make_matched_terminal(rt1_mid, 0, 1);
	MatchedDepRule* mt1_start = MatchedDepRule::make_matched_terminal(rt1_start, 0, 1);
	DepRuleNode* rt2 = DepRuleNode::make_terminal_rule("", "V");
	MatchedDepRule* mt2 = MatchedDepRule::make_matched_terminal(rt2, 1, 2);
	MatchedDepRule* mt3 = MatchedDepRule::make_matched_terminal(rt1_mid, 2, 3);

	m1.advance_dot_with(mt1_mid);
	m1.advance_dot_with(mt2);
	m1.advance_dot_with(mt3);

	m2.advance_dot_with(mt1_start);
	m2.advance_dot_with(mt2);
	m2.advance_dot_with(mt3);

	BOOST_CHECK(!m1.is_realization_of(rule_2));
	BOOST_CHECK(m2.is_realization_of(rule_2));

	delete rt1_mid, rt1_start, rt2;
	delete mt1_mid, mt1_start, mt2, mt3;
}

BOOST_AUTO_TEST_CASE(Test_MatchCost)
{
	DepRuleEngine engine;
	string test_rule_path = Boost_Test_Dir + "/matched/test_match_rules_1.txt";
	if (fs::exists(test_rule_path)){
		engine.import_dep_rules(test_rule_path);
	}

	engine.post_process_rules();
	DepRuleNode* rule_1 = engine.get_rules_vec()[0];		
	DepRuleNode* rt1 = DepRuleNode::make_terminal_rule("", "N");	
	DepRuleNode* rt2 = DepRuleNode::make_terminal_rule("", "V");
	MatchedDepRule* mt1 = MatchedDepRule::make_matched_terminal(rt1, 0, 1);
	MatchedDepRule* mt2 = MatchedDepRule::make_matched_terminal(rt2, 1, 2);
	MatchedDepRule* mt3 = MatchedDepRule::make_matched_terminal(rt1, 2, 3);
	MatchedDepRule* mt4 = MatchedDepRule::make_matched_terminal(rt2, 0, 1);

	MatchedDepRule m1(*rule_1, 0, 0);
	MatchedDepRule m2(*rule_1, 0, 0);

	m1.advance_dot_with(mt1);
	m1.advance_dot_with(mt2);
	m1.advance_dot_with(mt3);	

	m2.advance_dot_with(mt4);
	m2.advance_dot_with(mt2);
	m2.advance_dot_with(mt3);

	BOOST_CHECK(m1.get_match_penalty() == 0);	
	BOOST_CHECK(m2.get_match_penalty() > 0);

	delete rt1, rt2;
	delete mt1, mt2, mt3, mt4;
}

BOOST_AUTO_TEST_CASE(Test_MatchPosition)
{
	DepRuleEngine engine;
	string test_rule_path = Boost_Test_Dir + "/matched/test_match_rules_3.txt";
	if (fs::exists(test_rule_path)) {
		engine.import_dep_rules(test_rule_path);
	}

	engine.post_process_rules();
	DepRuleNode* rule_1 = engine.get_rules_vec()[0];
	DepRuleNode* rt1 = DepRuleNode::make_terminal_rule("", "N");
	DepRuleNode* rt2 = DepRuleNode::make_terminal_rule("", "V");
	MatchedDepRule* mt1 = MatchedDepRule::make_matched_terminal(rt1, 0, 1);
	MatchedDepRule* mt2 = MatchedDepRule::make_matched_terminal(rt2, 1, 2);
	MatchedDepRule* mt3 = MatchedDepRule::make_matched_terminal(rt1, 2, 3);

	engine.set_context_to_all();
	PC_RuleList rlist1 = engine.predict_bu(*mt2);
	PC_RuleList rlist0 = engine.predict_bu(*mt3);
	
	BOOST_CHECK(!mt1->is_realization_of(rule_1->get_children(0)));
	BOOST_CHECK(mt2->is_realization_of(rule_1->get_children(0)));
	BOOST_CHECK(rlist0.size() == 0);	
	BOOST_CHECK(rlist1.size() == 1);

	delete rt1, rt2;
	delete mt1, mt2, mt3;
}
BOOST_AUTO_TEST_SUITE_END()

#endif // _TEST_