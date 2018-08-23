#include "DepData.h"
#include "DepParser.h"
#include "DepRuleEngine.h"
#include "ChartParser.h"
#include "PropNodeGraph.h"
#include "PNGraphLayout.h"
#include "PNGraphBuilder.h"
#include "DocParser.h"
#include "PNTextLayout.h"
#include <boost/filesystem.hpp>


void dep_rule_parse()
{
	DepData dep;
	vector<vector<DepInfo>> dep_data;
	dep.load_pos_text_file("../etc/test_data_word.txt", dep_data);


	// test rule set bison parser
	DepRuleEngine dep_rule_eng;
	dep_rule_eng.import_dep_rules("../etc/dep_rules_gram.txt");
	dep_rule_eng.import_dep_rules("../etc/dep_rules_lex.txt");
	dep_rule_eng.post_process_rules();

	ChartParser chart_parser;
	chart_parser.set_engine(&dep_rule_eng);

	chart_parser.parse(dep_data.at(0));

	printf("Worry not. Parsing completed.\n");
	printf("== Enter to close program ==\n");
	getchar();
}

void parse_rule_file(const string& fpath)
{	
	if (boost::filesystem::exists(fpath)) {
		DepRuleEngine dep_rule_eng;
		dep_rule_eng.import_dep_rules(fpath);
		dep_rule_eng.post_process_rules();
	} else {
		printf("Cannot find %s\n", fpath.c_str());
	}
}

void doc_parse()
{
	DepData dep;
	vector<vector<DepInfo>> dep_data;
	vector<DepRuleNode*>    term_aux_repo;

	PropNodeGraph pn_graph;
	// string in_path = "../etc/test_data_word.txt";
	string in_path = "../etc/CN_992_G5_3.seg.txt";	
	dep.load_pos_text_file(in_path, dep_data);
	// dep.load_pos_text("h:/test_case.seg.txt", dep_data);

	// test rule set bison parser
	DepRuleEngine dep_rule_eng;
	dep_rule_eng.import_dep_rules("../etc/dep_rules_gram.txt");
	dep_rule_eng.import_dep_rules("../etc/dep_rules_lex.txt");
	dep_rule_eng.post_process_rules();

	ChartParser chart_parser;
	chart_parser.set_engine(&dep_rule_eng);

	for (int i = 0; i < dep_data.size(); ++i){
		int n_loop = chart_parser.parse(dep_data.at(i));
		// vector<EdgeInfoTypeUP> edge_info = chart_parser.get_edge_info();
		const MatchedDepRule* full_edge = chart_parser.get_full_edge();
		if (!full_edge){
			printf("[INFO] Parsing failed at sequence %d\n", i);
		} else {
			printf("[INFO] Parse sequence %d in %d loops\n", i, n_loop);
		}

		pn_graph.import_mrule(full_edge);
		chart_parser.move_aux_rule(term_aux_repo);

	}

	PNGraphBuilder gbuilder;
	DepRuleEngine doc_rule_engine;
	doc_rule_engine.import_dep_rules("../etc/dep_rules_sen.txt");
	doc_rule_engine.post_process_rules();
	doc_rule_engine.set_context(dep_data);

	gbuilder.set_engine(&doc_rule_engine);
	gbuilder.set_graph(&pn_graph);
	gbuilder.build_edges();

	string dot_path = in_path.substr(0, in_path.find_last_of("."));
	dot_path += ".dot";
	string png_path = in_path.substr(0, in_path.find_last_of("."));
	png_path += ".png";
	PNGraphLayout layout(pn_graph);
	layout.toDot(dot_path);
	layout.toPNG(dot_path, png_path);

	for (auto x : term_aux_repo) { delete x; }
	term_aux_repo.clear();

	printf("== Enter to close program ==\n");
	getchar();
}

void doc_parse_prop()
{	
	string test_rule_path = "../etc/dep_rules.txt";
	// string test_rule_path = "../etc/PAVC_Syn_rules.txt";
	// string default_rule_path = "../etc/docParser/default_rules.txt";
	string in_path = "../etc/test_data_word.txt";

	DocParser doc_parser;
	PropNodeGraph pn_graph;
	doc_parser.import_rule_file(test_rule_path);
	// doc_parser.import_rule_file(default_rule_path);
	doc_parser.parse_file(in_path, pn_graph);

	PNTextLayout text_layout;
	text_layout.transform(pn_graph, "h:/docParse_prop.txt");
	
}
