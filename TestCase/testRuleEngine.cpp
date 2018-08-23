// --detect_memory_leaks = 0 --run_test = RuleParseTest
#ifdef _TEST_
#define BOOST_TEST_MODULE HeimdallrTest
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "../DepRuleEngine.h"
#include "../MatchedDepRule.h"
#include "unicode/unistr.h"

namespace fs = boost::filesystem;
namespace ag = boost::algorithm;

#include <string>
#include <iostream>
using std::string;

string Boost_Test_Dir = "../TestData/test_heimdallr/boost_test";

struct ParseTestFixture{
	DepRuleEngine* engine;
	ParseTestFixture(): engine(nullptr){
		string test_rule_path = Boost_Test_Dir + "/test_rules_boost.txt";
		if (fs::exists(test_rule_path)){
			engine = new DepRuleEngine();			
			engine->import_dep_rules(test_rule_path);
		}
	}

	~ParseTestFixture(){
		delete engine;
	}
};

BOOST_FIXTURE_TEST_SUITE(RuleParseTest, ParseTestFixture)

BOOST_AUTO_TEST_CASE(Test_Setup)
{	
	BOOST_REQUIRE(engine != nullptr);
}

BOOST_AUTO_TEST_CASE(Test_ParseRuleFile)
{	
	BOOST_REQUIRE(engine != nullptr);
	RuleList rlist = engine->get_rules_vec();
	BOOST_REQUIRE(rlist.size() > 0);

	DepRuleNode* rule = rlist[0];
	string comment = rule->get_comment();
	ag::trim(comment);
	BOOST_CHECK(comment == "A simple test");
	BOOST_CHECK(rule->get_priority() == 50);
	BOOST_CHECK(rule->get_rule_class() == "TestCategory");
	BOOST_CHECK(rule->get_rule_label() == "TC01");

	BOOST_REQUIRE(rule->get_children_count() == 3);	
	DepRuleNode* rule_1 = rule->get_children(0);
	DepRuleNode* rule_2 = rule->get_children(1);
	DepRuleNode* rule_3 = rule->get_children(2);
	BOOST_CHECK(rule_1->get_pos() == "N");
	BOOST_CHECK(rule_1->get_node_type() == NodeType::NdTerminal);
	BOOST_CHECK(rule_2->get_pos() == "Vt");
	BOOST_CHECK(rule_2->get_node_type() == NodeType::NdTerminal);
	BOOST_CHECK(rule_3->get_pos() == "N");
	BOOST_CHECK(rule_3->get_node_type() == NodeType::NdTree);
	
}

BOOST_AUTO_TEST_CASE(Test_ParseUTF8Text)
{
	BOOST_REQUIRE(engine != nullptr);
	RuleList rlist = engine->get_rules_vec();
	BOOST_REQUIRE(rlist.size() > 1);

	string u8str;

	DepRuleNode* rule = rlist[1];
	string comment = rule->get_comment();
	ag::trim(comment);
	UnicodeString big5str("測試規則", "big5");
	big5str.toUTF8String(u8str);
	BOOST_CHECK(comment == u8str);

	BOOST_REQUIRE(rule->get_children_count() == 4);
	DepRuleNode* rule_lit = rule->get_children(2);
	big5str = UnicodeString("之於", "big5");
	u8str.clear();
	big5str.toUTF8String(u8str);
	BOOST_CHECK(rule_lit->get_word() == u8str);
}

BOOST_AUTO_TEST_CASE(Test_NodeConstraint)
{
	BOOST_REQUIRE(engine != nullptr);
	RuleList rlist = engine->get_rules_vec();
	BOOST_REQUIRE(rlist.size() > 2);

	DepRuleNode* rule = rlist[2];
	DepRuleNode* ch1 = rule->get_children(0);
	DepRuleNode* ch3 = rule->get_children(2);
	BOOST_CHECK(ch1->get_node_constraints().at(NodeConstraint::RuleClass) == "NounCompound");
	BOOST_CHECK(ch3->get_node_constraints().at(NodeConstraint::RuleLabel) == "SD01");

}

BOOST_AUTO_TEST_CASE(Test_NodeReference)
{
	BOOST_REQUIRE(engine != nullptr);
	RuleList rlist = engine->get_rules_vec();
	BOOST_REQUIRE(rlist.size() > 3);

	DepRuleNode* rule = rlist[3];
	DepRuleNode* ch3 = rule->get_children(2);
	BOOST_CHECK(ch3->get_equal_ref() == 1);
}

BOOST_AUTO_TEST_CASE(Test_RuleConstraint)
{
	BOOST_REQUIRE(engine != nullptr);
	RuleList rlist = engine->get_rules_vec();
	BOOST_REQUIRE(rlist.size() > 4);

	DepRuleNode* rule = rlist[4];
	const RuleConstraintList& rclist = rule->get_rule_constraints();

	BOOST_CHECK(rule->get_tag() == "abc");

	BOOST_CHECK(rclist[0]->valence == 0);
	BOOST_CHECK(rclist[0]->cond == RCCondition::ParentClass);
	BOOST_CHECK(rclist[0]->apply_node == -1);
	BOOST_CHECK(rclist[0]->rel_op == RCRelOp::Like);
	BOOST_CHECK(rclist[0]->arg == "PUNC");

	BOOST_CHECK(rclist[1]->valence == 1);
	BOOST_CHECK(rclist[1]->cond == RCCondition::Strategy);
	BOOST_CHECK(rclist[1]->apply_node == -1);
	BOOST_CHECK(rclist[1]->rel_op == RCRelOp::Equal);
	BOOST_CHECK(rclist[1]->arg == "FP");
	BOOST_CHECK(rclist[1]->weight == 10);
}

BOOST_AUTO_TEST_CASE(Test_NodeConnection)
{
	BOOST_REQUIRE(engine != nullptr);
	RuleList rlist = engine->get_rules_vec();
	BOOST_REQUIRE(rlist.size() > 5);

	DepRuleNode* rule = rlist[5];
	DepRuleNode* ch2 = rule->get_children(1);
	DepRuleNode* ch3 = rule->get_children(2);
	DepRuleNode* ch4 = rule->get_children(3);
	BOOST_CHECK(ch2->get_node_connection() == NodeConnRelation::Follow);
	BOOST_CHECK(ch3->get_node_connection() == NodeConnRelation::Immediate);
	BOOST_CHECK(ch4->get_node_connection() == NodeConnRelation::RightArrow);
}

BOOST_AUTO_TEST_CASE(Test_NodeHead)
{
	BOOST_REQUIRE(engine != nullptr);
	RuleList rlist = engine->get_rules_vec();
	BOOST_REQUIRE(rlist.size() > 6);

	DepRuleNode* rule = rlist[6];
	DepRuleNode* ch3 = rule->get_children(2);	
	BOOST_CHECK(ch3->get_head_flag());	
}

BOOST_AUTO_TEST_CASE(Test_NodeDummy)
{
	BOOST_REQUIRE(engine != nullptr);
	RuleList rlist = engine->get_rules_vec();
	BOOST_REQUIRE(rlist.size() > 7);

	DepRuleNode* rule = rlist[7];
	DepRuleNode* ch1 = rule->get_children(0);
	DepRuleNode* ch3 = rule->get_children(2);
	BOOST_CHECK(ch1->get_children_count() == 2);
	BOOST_CHECK(ch1->get_node_type() == NodeType::NdOrNode);	
	BOOST_CHECK(ch3->is_optional());
}

BOOST_AUTO_TEST_CASE(Test_NodeAnchoring)
{
	BOOST_REQUIRE(engine != nullptr);
	RuleList rlist = engine->get_rules_vec();
	BOOST_REQUIRE(rlist.size() > 8);

	DepRuleNode* rule = rlist[8];
	BOOST_REQUIRE(rule->get_children_count() == 5);
	DepRuleNode* ch1 = rule->get_children(0);
	DepRuleNode* ch2 = rule->get_children(1);
	DepRuleNode* ch5 = rule->get_children(4);
	BOOST_CHECK(ch1->get_anchor() == AnchorType::AnchorBoth);
	BOOST_CHECK(ch2->get_node_type() == NodeType::NdSeqStart);
	BOOST_CHECK(ch5->get_node_type() == NodeType::NdSeqEnd);	
}

BOOST_AUTO_TEST_CASE(Test_PassiveTree)
{
	BOOST_REQUIRE(engine != nullptr);
	RuleList rlist = engine->get_rules_vec();
	BOOST_REQUIRE(rlist.size() > 9);

	DepRuleNode* rule = rlist[9];
	MatchedDepRule* m1 = new MatchedDepRule(*rule, 0, 0);
	MatchedDepRule* m2 = new MatchedDepRule(*rlist[6], 0, 0);
	engine->set_context_to_all();
	PC_RuleList tdlist1 = engine->predict_td(*m1);		
	PC_RuleList tdlist2 = engine->predict_td(*m2);
	
	BOOST_CHECK(tdlist1.size() == 0);
	BOOST_CHECK(tdlist2.size() > 0);
	
}
BOOST_AUTO_TEST_SUITE_END()
#endif