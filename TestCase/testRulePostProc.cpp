#ifdef _TEST_
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include "../DepRuleEngine.h"
#include "unicode/unistr.h"

namespace fs = boost::filesystem;
#include <string>
#include <iostream>
using std::string;

extern string Boost_Test_Dir;

BOOST_AUTO_TEST_SUITE(PostProcTest)

BOOST_AUTO_TEST_CASE(Test_Optional_Single)
{
	DepRuleEngine engine;
	string test_rule_path = Boost_Test_Dir + "/postproc/test_post_proc_rules_opt1.txt";
	if (fs::exists(test_rule_path)){		
		engine.import_dep_rules(test_rule_path);
	}

	engine.post_process_rules();
	BOOST_CHECK(engine.get_rules_vec().size() == 2);
	
}

BOOST_AUTO_TEST_CASE(Test_Optional_Double)
{
	DepRuleEngine engine;
	string test_rule_path = Boost_Test_Dir + "/postproc/test_post_proc_rules_opt2.txt";
	if (fs::exists(test_rule_path)){
		engine.import_dep_rules(test_rule_path);
	}

	engine.post_process_rules();

	BOOST_CHECK(engine.get_rules_vec().size() == 4);

}

BOOST_AUTO_TEST_CASE(Test_Single_Group)
{
	DepRuleEngine engine;
	string test_rule_path = Boost_Test_Dir + "/postproc/test_post_proc_rules_group_1.txt";
	if (fs::exists(test_rule_path)){
		engine.import_dep_rules(test_rule_path);
	}

	engine.post_process_rules();
	BOOST_CHECK(engine.get_rules_vec().size() == 1);
	BOOST_CHECK(engine.get_rules_vec().at(0)->get_children_count() == 3);

}

BOOST_AUTO_TEST_CASE(Test_Or_Group)
{
	DepRuleEngine engine;
	string test_rule_path = Boost_Test_Dir + "/postproc/test_post_proc_rules_or_1.txt";
	if (fs::exists(test_rule_path)){
		engine.import_dep_rules(test_rule_path);
	}

	engine.post_process_rules();
	vector<int> expected{2,1,1};
	vector<int> actual;
	const RuleList& rlist = engine.get_rules_vec();
	for (DepRuleNode* rule_x : rlist){
		actual.push_back(rule_x->get_children_count());
	}
	vector<int> intset;

	std::sort(expected.begin(), expected.end());
	std::sort(actual.begin(), actual.end());
	std::set_intersection(expected.begin(), expected.end(),
		actual.begin(), actual.end(), std::back_inserter(intset));
	BOOST_CHECK(engine.get_rules_vec().size() == 3);
	BOOST_CHECK(intset.size() == actual.size());

}

BOOST_AUTO_TEST_CASE(Test_GroupedOr)
{
	DepRuleEngine engine;
	string test_rule_path = Boost_Test_Dir + "/postproc/test_post_proc_rules_group_or.txt";
	if (fs::exists(test_rule_path)){
		engine.import_dep_rules(test_rule_path);
	}

	engine.post_process_rules();

	vector<int> expected{ 2,2,2 };
	vector<int> actual;
	const RuleList& rlist = engine.get_rules_vec();
	for (DepRuleNode* rule_x : rlist){
		actual.push_back(rule_x->get_children_count());
	}
	vector<int> intset;

	std::sort(expected.begin(), expected.end());
	std::sort(actual.begin(), actual.end());
	std::set_intersection(expected.begin(), expected.end(),
		actual.begin(), actual.end(), std::back_inserter(intset));
	BOOST_CHECK(engine.get_rules_vec().size() == 3);
	BOOST_CHECK(intset.size() == actual.size());	

}

BOOST_AUTO_TEST_CASE(Test_GroupedOptional)
{
	DepRuleEngine engine;
	string test_rule_path = Boost_Test_Dir + "/postproc/test_post_proc_rules_group_opt.txt";
	if (fs::exists(test_rule_path)){
		engine.import_dep_rules(test_rule_path);
	}

	engine.post_process_rules();

	vector<int> expected{ 2, 3, 2, 1 };
	vector<int> actual;
	const RuleList& rlist = engine.get_rules_vec();
	for (DepRuleNode* rule_x : rlist){
		actual.push_back(rule_x->get_children_count());
	}
	vector<int> intset;

	std::sort(expected.begin(), expected.end());
	std::sort(actual.begin(), actual.end());
	std::set_intersection(expected.begin(), expected.end(),
		actual.begin(), actual.end(), std::back_inserter(intset));
	BOOST_CHECK(engine.get_rules_vec().size() == 4);
	BOOST_CHECK(intset.size() == actual.size());	

}

BOOST_AUTO_TEST_CASE(Test_Anchor)
{
	DepRuleEngine engine;
	string test_rule_path = Boost_Test_Dir + "/postproc/test_post_proc_rules_anchor.txt";
	if (fs::exists(test_rule_path)){
		engine.import_dep_rules(test_rule_path);
	}

	engine.post_process_rules();				

	BOOST_CHECK(engine.get_rules_vec().size() == 1);

	DepRuleNode* rule = engine.get_rules_vec().at(0);
	BOOST_CHECK(rule->get_children_count() == 2);
	BOOST_CHECK(rule->get_children(1)->get_anchor() == AnchorType::AnchorStart);
	BOOST_CHECK(rule->get_children(1)->get_node_connection() == NodeConnRelation::Follow);

}

BOOST_AUTO_TEST_SUITE_END()

#endif // _TEST_