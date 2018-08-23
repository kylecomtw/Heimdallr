// --detect_memory_leak=0 --run_test=ChartParserTest/Test_SequenceRelation
#ifdef _TEST_
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include "../DepRuleEngine.h"
#include "../MatchedDepRule.h"
#include "../ChartParser.h"
#include "../ParserInitLexicalMarker.h"
#include "../DocParser.h"
#include "../PNTextLayout.h"
#include "unicode/unistr.h"


namespace fs = boost::filesystem;
#include <string>
#include <iostream>
using std::string;

extern string Boost_Test_Dir;

BOOST_AUTO_TEST_SUITE(ChartParserTest)

BOOST_AUTO_TEST_CASE(Test_SimpleParse)
{
	DepRuleEngine engine;
	string test_rule_path = Boost_Test_Dir + "/chartParse/test_chartparse_rules_1.txt";
	if (fs::exists(test_rule_path)){
		engine.import_dep_rules(test_rule_path);
	}
	engine.post_process_rules();

	DepData dep;
	vector<vector<DepInfo>> dep_data;
	
	string in_path = Boost_Test_Dir + "/chartParse/test_1.seg.txt";
	dep.load_pos_text(in_path, dep_data);
	BOOST_REQUIRE(dep_data.size() > 0);

	ChartParser chart_parser;
	chart_parser.set_engine(&engine);

	int n_loop = chart_parser.parse(dep_data.at(0));
	const MatchedDepRule* full_edge = chart_parser.get_full_edge();
	BOOST_CHECK(full_edge != nullptr);
}

BOOST_AUTO_TEST_CASE(Test_NestedParse)
{
	DepRuleEngine engine;
	string test_rule_path = Boost_Test_Dir + "/chartParse/test_chartparse_rules_2.txt";
	if (fs::exists(test_rule_path)){
		engine.import_dep_rules(test_rule_path);
	}
	engine.post_process_rules();

	DepData dep;
	vector<vector<DepInfo>> dep_data;

	string in_path = Boost_Test_Dir + "/chartParse/test_2.seg.txt";
	dep.load_pos_text(in_path, dep_data);
	BOOST_REQUIRE(dep_data.size() > 0);

	ChartParser chart_parser;
	chart_parser.set_engine(&engine);

	int n_loop = chart_parser.parse(dep_data.at(0));
	const MatchedDepRule* full_edge = chart_parser.get_full_edge();
	BOOST_CHECK(full_edge != nullptr);
	BOOST_CHECK(full_edge->get_tree_match_cost() == MatchedDepRule::alpha * 4 + 50 * 2);
}


BOOST_AUTO_TEST_CASE(TestLexicalMarkerParse)
{
	debug_out = fopen("h:/TestLexicalMarkerParse.log", "w");
	DepRuleEngine engine;
	string test_rule_path = Boost_Test_Dir + "/chartParse/test_chartparse_rules_3.txt";
	if (fs::exists(test_rule_path)){
		engine.import_dep_rules(test_rule_path);
	}
	engine.add_lexical_marker_category("TC03");
	engine.add_lexical_marker_category("TC02");
	engine.post_process_rules();

	DepData dep;
	vector<vector<DepInfo>> dep_data;

	string in_path = Boost_Test_Dir + "/chartParse/test_3.seg.txt";
	dep.load_pos_text(in_path, dep_data);
	BOOST_REQUIRE(dep_data.size() > 0);

	ChartParser chart_parser;	
	chart_parser.set_engine(&engine);
	chart_parser.on_init([](const ChartParserChart& chart)->void{
		BOOST_CHECK(chart.size() == 6);
	});

	int n_loop = chart_parser.parse(dep_data.at(0));
	const ChartParserChart& chart = chart_parser.get_chart();

	const MatchedDepRule* full_edge = chart_parser.get_full_edge();
	BOOST_REQUIRE(full_edge != nullptr);
	BOOST_CHECK(full_edge->get_rule_node().get_rule_label() == "TC03");


	fflush(debug_out);	fclose(debug_out);
}

BOOST_AUTO_TEST_CASE(Test_SequenceRelation)
{
	debug_out = fopen("h:/TestSequenceRelation.log", "w");	
	string test_rule_path = Boost_Test_Dir + "/chartParse/test_chartparse_rules_4.txt";		
	string in_path = Boost_Test_Dir + "/chartParse/test_4.seg.txt";	
	
	
	DocParser doc_parser;
	PropNodeGraph pn_graph;
	doc_parser.import_rule_file(test_rule_path);
	doc_parser.parse_document(in_path, pn_graph);

	bool has_seq = false;
	for (const auto& x : pn_graph.matched_rules()) {
		if (x.matched_rule_class() == "Sequence" &&
			x.start_pos() == 0 && x.end_pos() == 7) {
			has_seq = true;
			break;
		}
	}
	BOOST_CHECK(has_seq);
	fflush(debug_out);	fclose(debug_out);
}

BOOST_AUTO_TEST_CASE(Test_Connection)
{
	debug_out = fopen("h:/TestConnection.log", "w");
	string test_rule_path = Boost_Test_Dir + "/chartParse/test_chartparse_rules_5.txt";
	string in_path = Boost_Test_Dir + "/chartParse/test_5.seg.txt";

	DocParser doc_parser;
	PropNodeGraph pn_graph;
	doc_parser.import_rule_file(test_rule_path);
	doc_parser.parse_document(in_path, pn_graph);

	bool seq01 = false, seq02 = false, seq03 = false;
	for (const auto& x : pn_graph.matched_rules()) {
		if (x.matched_rule_label() == "Con01" &&
			x.start_pos() == 0 && x.end_pos() == 7) {
			seq01 = true;			
		}

		if (x.matched_rule_label() == "Con02" &&
			x.start_pos() == 1 && x.end_pos() == 6) {
			seq02 = true;
		}

		if (x.matched_rule_label() == "Con03" &&
			x.start_pos() == 2 && x.end_pos() == 7) {
			seq03 = true;
		}
	}
	BOOST_CHECK(seq01);
	BOOST_CHECK(seq02);
	BOOST_CHECK(seq03);
	PNTextLayout text_layout;
	text_layout.transform(pn_graph, "h:/Test_Connection_prop.txt");

	fflush(debug_out);	fclose(debug_out);
}

BOOST_AUTO_TEST_CASE(Test_Passage1)
{
	debug_out = fopen("h:/TestPassage1.log", "w");
	string test_rule_path = Boost_Test_Dir + "/chartParse/test_chartparse_rules_6.txt";
	string in_path = Boost_Test_Dir + "/chartParse/test_6.seg.txt";

	DocParser doc_parser;
	PropNodeGraph pn_graph;
	doc_parser.import_rule_file(test_rule_path);	
	doc_parser.parse_document(in_path, pn_graph);

	PNTextLayout text_layout;
	text_layout.transform(pn_graph, "h:/Test_Passage1_prop.txt");

	fflush(debug_out);	fclose(debug_out);
}

BOOST_AUTO_TEST_SUITE_END()

#endif // _TEST_