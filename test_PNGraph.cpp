#include "DepData.h"
#include "DepParser.h"
#include "DepRuleEngine.h"
#include "ChartParser.h"
#include "PropNodeGraph.h"
#include "PNGraphLayout.h"
#include "PNGraphBuilder.h"
#include "DocParser.h"
#include <cassert>

bool test_PNGraph()
{
	DepData dep;
	vector<vector<DepInfo>> dep_data;
	vector<DepRuleNode*>    term_aux_repo;

	PropNodeGraph pn_graph;
	string in_path = "../TestData/test_heimdallr/test_data_word.txt";	
	dep.load_pos_text_file(in_path, dep_data);

	// test rule set bison parser
	DepRuleEngine dep_rule_eng;
	dep_rule_eng.import_dep_rules("../TestData/test_heimdallr/dep_rules_gram.txt");	
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

	const CPPropNodeVec_t& term_nodes = pn_graph.term_nodes();
	assert(term_nodes.size() == 9); 
	assert(pn_graph.all_nodes().size() == 11);
	assert(pn_graph.edges().size() == 9);

	PNGraphBuilder gbuilder;
	DepRuleEngine doc_rule_engine;
	doc_rule_engine.import_dep_rules("../TestData/test_heimdallr/dep_rules_sen.txt");
	doc_rule_engine.post_process_rules();
	doc_rule_engine.set_context(dep_data);

	gbuilder.set_engine(&doc_rule_engine);
	gbuilder.set_graph(&pn_graph);
	gbuilder.build_edges();

	const CPPropNodeVec_t& uniq_nodes = pn_graph.unique_terms();
	assert(uniq_nodes.size() == 8);

	string dot_path = in_path.substr(0, in_path.find_last_of("."));
	dot_path += ".dot";
	string png_path = in_path.substr(0, in_path.find_last_of("."));
	png_path += ".png";
	PNGraphLayout layout(pn_graph);
	layout.toDot(dot_path);
	layout.toPNG(dot_path, png_path);

	for (auto x : term_aux_repo) { delete x; }
	term_aux_repo.clear();

	return false;
}

bool test_doc_parser()
{	
	DocParser seq_parser;
	PropNodeGraph graph;
	seq_parser.import_rule_file("../TestData/test_heimdallr/dep_rules.txt");
	seq_parser.parse_file("../TestData/test_heimdallr/test_data_word.txt", graph);
	
	seq_parser.build_graph(graph);

	return true;
}